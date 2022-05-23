#include "pch.h"

#if defined(TetWildPlugin_EXPORTS)
#define TETWILD_API __declspec(dllexport) 
#elif defined(TetWildPlugin_IMPORTS)
#define TETWILD_API __declspec(dllimport)
#else
#define TETWILD_API
#endif

struct double3
{
	double x, y, z;
};

// Forward declare.
// Definition is below.
void TetWild_DebugLog(const std::stringstream& ss);

#ifdef __cplusplus
extern "C"
{
#endif

	TETWILD_API int TetWildPlugin_GetVersion()
	{
		return 1;
	}

	// Register a callback function to print messages in Unity.
	using UnityDebugCallback = void(*)(const char*);
	static UnityDebugCallback unityDebugCallback = nullptr;

	TETWILD_API void TetWildPlugin_RegisterDebugCallback(UnityDebugCallback cb)
	{
		unityDebugCallback = cb;
	}

	void TetWild_DebugLog(const char* message)
	{
		if (unityDebugCallback)
			unityDebugCallback(message);
	}

	TETWILD_API void TetWildPlugin_TetrahedralizeMesh(int numVertices, const double3* vertices, int numIndices, const int* indices)
	{
		tetwild::Args args;

		Eigen::MatrixXd VI, VO;
		Eigen::MatrixXi FI, TO;
		Eigen::VectorXd AO;

		std::stringstream ss;
		ss << std::endl;
		ss << "[TetWild] Num vertices: " << numVertices << std::endl;
		ss << "[TetWild] Num indices:  " << numIndices << std::endl;

		TetWild_DebugLog(ss);
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

		TetWild_DebugLog("Starting tetrahedralization...");

		tetwild::tetrahedralization(VI, FI, VO, TO, AO, args);

		TetWild_DebugLog("Mesh tetrahedralization complete!");

	}

#ifdef __cplusplus
}
#endif

void TetWild_DebugLog(const std::stringstream& ss)
{
	const std::string& s = ss.str();
	const char* c = s.c_str();
	TetWild_DebugLog(c);
}
