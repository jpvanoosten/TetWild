#include "pch.h"

#include "ProgressHandler.h"

// Define a callback function that can be used to send progress updates back to unity.
using ProgressCallback = void(*)(float, const char*);
static ProgressCallback progressCallback = nullptr;

void SendProgressUpdate(float progress, const char* msg)
{
	if (progressCallback)
		progressCallback(progress, msg);
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

		SendProgressUpdate(0.0f, message.c_str());
	}

	void flush_() override
	{}
};

using UnitySink_mt = UnitySink<std::mutex>;

static void TetrahedralizeMesh(Eigen::MatrixXd VI, Eigen::MatrixXi FI, ProgressHandler* progressHandler)
{
	tetwild::Args args;

	Eigen::MatrixXd VO;
	Eigen::MatrixXi TO;
	Eigen::VectorXd AO;

	progressHandler->Update(0.0f, "Starting tetrahedralization...");

	tetwild::tetrahedralization(VI, FI, VO, TO, AO, args);

	progressHandler->Update(100.0f, "Mesh tetrahedralization complete!");
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


	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API TetWildPlugin_TetrahedralizeMesh(int numVertices, const double3* vertices, int numIndices, const int* indices, ProgressHandler* progressHandler)
	{
		Eigen::MatrixXd VI;
		Eigen::MatrixXi FI;

		tetwild::logger().info("Num vertices: {}", numVertices);
		tetwild::logger().info("Num indices: {}", numIndices);

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

		std::thread t{ &TetrahedralizeMesh, std::move(VI), std::move(FI), progressHandler };
		t.detach();
	}

#ifdef __cplusplus
}
#endif
