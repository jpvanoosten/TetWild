cmake_minimum_required(VERSION 3.3)

set( INC
	ProgressHandler.h
	PluginAPI/IUnityShaderCompilerAccess.h
	PluginAPI/IUnityEventQueue.h
	PluginAPI/IUnityGraphics.h
	PluginAPI/IUnityGraphicsD3D11.h
	PluginAPI/IUnityGraphicsD3D12.h
	PluginAPI/IUnityGraphicsMetal.h
	PluginAPI/IUnityGraphicsVulkan.h
	PluginAPI/IUnityInterface.h
	PluginAPI/IUnityLog.h
	PluginAPI/IUnityProfiler.h
	PluginAPI/IUnityProfilerCallbacks.h
	PluginAPI/IUnityRenderingExtensions.h
	pch.h
)

set( SRC
	dllmain.cpp
	pch.cpp
	ProgressHandler.cpp
	TetWildPlugin.cpp
)

add_library(TetWildPlugin SHARED ${INC} ${SRC})

target_compile_definitions( TetWildPlugin
	INTERFACE TetWildPlugin_IMPORTS
)

target_link_libraries( TetWildPlugin 
		libTetWild
)
igl_copy_cgal_dll(TetWildPlugin)

