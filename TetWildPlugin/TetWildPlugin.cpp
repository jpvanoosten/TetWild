#include "pch.h"

#if defined(TETWILDPLUGIN_EXPORTS)
#define TETWILD_API __declspec(dllexport) 
#elif defined(TETWILDPLUGIN_IMPORTS)
#define TETWILD_API __declspec(dllimport)
#else
#define TETWILD_API
#endif


#ifdef __cplusplus
extern "C"
{
#endif

	TETWILD_API int TetWildPlugin_GetVersion()
	{
		return 1;
	}

	TETWILD_API void TetWildPlugin_TetrahedralizeMesh()
	{
		tetwild::Args args;

		Eigen::MatrixXd VI, VO;
		Eigen::MatrixXi FI, TO;
		Eigen::VectorXd AO;

		tetwild::tetrahedralization(VI, FI, VO, TO, AO, args);
	}

#ifdef __cplusplus
}
#endif