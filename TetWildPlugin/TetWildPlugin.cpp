#include "pch.h"

struct double3
{
	double x, y, z;
};

static IUnityInterfaces* s_UnityInterfaces = nullptr;
static IUnityLog* s_UnityLog = nullptr;

template<typename Mutex>
class UnitySink : public spdlog::sinks::base_sink<Mutex>
{
protected:
	void sink_it_(const spdlog::details::log_msg& msg) override
	{
		fmt::memory_buffer formatted;
		spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

		switch (msg.level)
		{
		case spdlog::level::trace:
		case spdlog::level::debug:
		case spdlog::level::info:
			UNITY_LOG(s_UnityLog, fmt::to_string(formatted).c_str());
			break;
		case spdlog::level::warn:
			UNITY_LOG_WARNING(s_UnityLog, fmt::to_string(formatted).c_str());
			break;
		case spdlog::level::err:
		case spdlog::level::critical:
			UNITY_LOG_ERROR(s_UnityLog, fmt::to_string(formatted).c_str());
			break;
		case spdlog::level::off: break;
		}
	}

	void flush_() override
	{}
};

using UnitySink_mt = UnitySink<std::mutex>;

static void TetrahedralizeMesh(int numVertices, const double3* vertices, int numIndices, const int* indices)
{
	tetwild::Args args;

	Eigen::MatrixXd VI, VO;
	Eigen::MatrixXi FI, TO;
	Eigen::VectorXd AO;

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

	tetwild::logger().info("Starting tetrahedralization...");

	tetwild::tetrahedralization(VI, FI, VO, TO, AO, args);

	tetwild::logger().info("Mesh tetrahedralization complete!");
}

#ifdef __cplusplus
extern "C"
{
#endif

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		s_UnityInterfaces = unityInterfaces;
		s_UnityLog = s_UnityInterfaces->Get<IUnityLog>();

		auto unitySink = std::make_shared<UnitySink_mt>();
		tetwild::logger().sinks().push_back(unitySink);

		assert(s_UnityLog);
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
	{

	}

	int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API TetWildPlugin_GetVersion()
	{
		return 1;
	}

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API TetWildPlugin_TetrahedralizeMesh(int numVertices, const double3* vertices, int numIndices, const int* indices)
	{
		std::thread t{ &TetrahedralizeMesh, numVertices, vertices, numIndices, indices };
		t.detach();
	}

#ifdef __cplusplus
}
#endif
