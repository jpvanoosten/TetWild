#include "pch.h"

// Define a callback function that can be used to send progress updates back to Unity.
using ProgressCallback = void(*)(float, const char*);
static ProgressCallback progressCallback = nullptr;

// Define a callback function that can be used to send a complete notification to Unity.
using CompleteCallback = void(*)();
static CompleteCallback completeCallback = nullptr;

void SendProgressUpdate(float progress, const char* msg)
{
	if (progressCallback)
		progressCallback(progress, msg);
}

void SendCompleteMessage()
{
	if (completeCallback)
		completeCallback();
}

struct double3
{
	double x, y, z;
};

static IUnityInterfaces* s_UnityInterfaces = nullptr;
static IUnityLog* s_UnityLog = nullptr;

// A spdlog sink to send log messages to Unity.
template<typename Mutex>
class UnitySink : public spdlog::sinks::base_sink<Mutex>
{
protected:
	void sink_it_(const spdlog::details::log_msg& msg) override
	{
		fmt::memory_buffer formatted;
		spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
		std::string message = fmt::to_string(formatted);

		switch (msg.level)
		{
		case spdlog::level::trace:
		case spdlog::level::debug:
		case spdlog::level::info:
			UNITY_LOG(s_UnityLog, message.c_str());
			break;
		case spdlog::level::warn:
			UNITY_LOG_WARNING(s_UnityLog, message.c_str());
			break;
		case spdlog::level::err:
		case spdlog::level::critical:
			UNITY_LOG_ERROR(s_UnityLog, message.c_str());
			break;
		case spdlog::level::off:
			break;
		}
	}

	void flush_() override
	{}
};

using UnitySink_mt = UnitySink<std::mutex>;

// A progress handler that sends progress messages back to Unity.
class ProgressHandler : public tetwild::ProgressHandler
{
public:
	ProgressHandler(int totalSteps)
		:totalSteps(totalSteps)
		, currentStep(0)
	{}

protected:
	void DoMessage(spdlog::level::level_enum level, const std::string& message) const override
	{
		switch (level)
		{
		case spdlog::level::trace:
		case spdlog::level::debug:
			break;
		case spdlog::level::info:
			currentStep++;
			SendProgressUpdate(static_cast<float>(currentStep) / static_cast<float>(totalSteps) * 100.0f, message.c_str());
			//SendProgressUpdate(currentStep, message.c_str());
			break;
		case spdlog::level::warn:
		case spdlog::level::err:
		case spdlog::level::critical:
		case spdlog::level::off:
			break;
		}
	}

private:
	int totalSteps = 1;
	mutable int currentStep = 0;
};

void saveFinalTetmesh(const fs::path& output_volume, const fs::path& output_surface, const Eigen::MatrixXd& V, const Eigen::MatrixXi& T, const Eigen::VectorXd& A)
{
	ProgressHandler::Info("Writing tetmesh mesh to {}...", output_volume.string());

	PyMesh::MshSaver mSaver(output_volume.string(), true);
	PyMesh::VectorF V_flat(V.size());
	PyMesh::VectorI T_flat(T.size());
	Eigen::MatrixXd VV = V.transpose();
	Eigen::MatrixXi TT = T.transpose();
	std::copy_n(VV.data(), V.size(), V_flat.data());
	std::copy_n(TT.data(), T.size(), T_flat.data());

	mSaver.save_mesh(V_flat, T_flat, 3, mSaver.TET);
	mSaver.save_elem_scalar_field("min_dihedral_angle", A);

	ProgressHandler::Info("Writing surface mesh to {}...", output_surface.string());

	// Extract and save a surface mesh.
	Eigen::MatrixXd V_sf;
	Eigen::MatrixXi F_sf;
	tetwild::extractSurfaceMesh(V, T, V_sf, F_sf);

	igl::writeOBJ(output_surface.string(), V_sf, F_sf);
}

static void TetrahedralizeMesh(const Eigen::MatrixXd& VI, const Eigen::MatrixXi& FI, const fs::path& output_mesh )
{
	tetwild::ProgressHandler::SetProgressHandler(std::make_shared<ProgressHandler>(55));

	tetwild::Args args;

	Eigen::MatrixXd VO;
	Eigen::MatrixXi TO;
	Eigen::VectorXd AO;

	tetwild::ProgressHandler::Info("Starting mesh tetrahedralization...");

	tetrahedralization(VI, FI, VO, TO, AO, args);

	const fs::path output_surface = (output_mesh.parent_path() / output_mesh.stem()).string() + "_sf.obj";
	saveFinalTetmesh(output_mesh, output_surface, VO, TO, AO);

	tetwild::ProgressHandler::Info("Mesh tetrahedralization complete!");

	// Notify Unity the operation is complete.
	SendCompleteMessage();

	// Remove progress handler.
	tetwild::ProgressHandler::SetProgressHandler(nullptr);
}

#ifdef __cplusplus
extern "C"
{
#endif
	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		s_UnityInterfaces = unityInterfaces;
		s_UnityLog = s_UnityInterfaces->Get<IUnityLog>();

		assert(s_UnityLog);

		// Add a spdlog sink that sends messages to Unity's console.
		tetwild::logger().sinks().push_back(std::make_shared<UnitySink_mt>());
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
	{

	}

	int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API TetWildPlugin_GetVersion()
	{
		return 1;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API TetWildPlugin_RegisterProgressCallback(ProgressCallback cb)
	{
		progressCallback = cb;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API TetWildPlugin_RegisterCompleteCallback(CompleteCallback cb)
	{
		completeCallback = cb;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API TetWildPlugin_TetrahedralizeMesh(int numVertices, const double3* vertices, int numIndices, const int* indices, const char* output_mesh )
	{
		Eigen::MatrixXd VI;
		Eigen::MatrixXi FI;

		// Populate the input vertices.
		VI.resize(numVertices, 3);
		for (int i = 0; i < numVertices; ++i)
		{
			VI.row(i) << vertices[i].x, vertices[i].y, vertices[i].z;
		}

		// Populate the input faces (triangles lists).
		FI.resize(numIndices / 3, 3);
		for (int i = 0; i < FI.rows(); ++i)
		{
			FI.row(i) << indices[i * 3 + 0], indices[i * 3 + 1], indices[i * 3 + 2];
		}

		std::thread t{ &TetrahedralizeMesh, std::move(VI), std::move(FI), output_mesh };
		t.detach();
	}

#ifdef __cplusplus
}
#endif
