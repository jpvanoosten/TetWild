#include "pch.h"

#if defined(TETWILDPLUGIN_EXPORTS)
#define TETWILD_API __declspec(dllexport) 
#elif defined(TETWILDPLUGIN_IMPORTS)
#define TETWILD_API __declspec(dllimport)
#else
#define TETWILD_API
#endif

struct double3
{
	double x, y, z;
};

#ifdef __cplusplus
extern "C"
{
#endif

	TETWILD_API int TetWildPlugin_GetVersion()
	{
		return 1;
	}

	TETWILD_API void TetWildPlugin_TetrahedralizeMesh(int numVertices, const double3* vertices, int numIndices, const int* indices)
	{
		tetwild::Args args;

		Eigen::MatrixXd VI, VO;
		Eigen::MatrixXi FI, TO;
		Eigen::VectorXd AO;

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

		// Generate the tetrahedral mesh...
		tetwild::tetrahedralization(VI, FI, VO, TO, AO, args);

		// Save the tetrahedral mesh to a file...
	}

#ifdef __cplusplus
}
#endif