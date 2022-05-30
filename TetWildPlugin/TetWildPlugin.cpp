#include "pch.h"

struct double3
{
	double x, y, z;
};

static IUnityInterfaces* s_UnityInterfaces = nullptr;
static IUnityLog* s_UnityLog = nullptr;

#ifdef __cplusplus
extern "C"
{
#endif

	void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		s_UnityInterfaces = unityInterfaces;
		s_UnityLog = s_UnityInterfaces->Get<IUnityLog>();

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
		tetwild::Args args;

		Eigen::MatrixXd VI, VO;
		Eigen::MatrixXi FI, TO;
		Eigen::VectorXd AO;

		std::stringstream ss;
		ss << std::endl;
		ss << "[TetWild] Num vertices: " << numVertices << std::endl;
		ss << "[TetWild] Num indices:  " << numIndices << std::endl;

		UNITY_LOG(s_UnityLog, ss.str().c_str());

		ss.clear();

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

		UNITY_LOG(s_UnityLog, "Starting tetrahedralization...");

		tetwild::tetrahedralization(VI, FI, VO, TO, AO, args);

		UNITY_LOG(s_UnityLog, "Mesh tetrahedralization complete!");

	}

#ifdef __cplusplus
}
#endif
