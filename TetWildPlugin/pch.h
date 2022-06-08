// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
// Windows Header Files
#include <Windows.h>

#include <igl/writeOBJ.h>
#include <pymesh/MshSaver.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <tetwild/tetwild.h>
#include <tetwild/ProgressHandler.h>

#include <atomic>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <thread>

namespace fs = std::filesystem;

#include "PluginAPI/IUnityEventQueue.h"
//#include "PluginAPI/IUnityGraphics.h"
//#include "PluginAPI/IUnityGraphicsD3D11.h"
//#include "PluginAPI/IUnityGraphicsD3D12.h"
//#include "PluginAPI/IUnityGraphicsMetal.h"
//#include "PluginAPI/IUnityGraphicsVulkan.h"
#include "PluginAPI/IUnityInterface.h"
#include "PluginAPI/IUnityLog.h"
#include "PluginAPI/IUnityEventQueue.h"
#include "PluginAPI/IUnityProfiler.h"
#include "PluginAPI/IUnityProfilerCallbacks.h"
#include "PluginAPI/IUnityRenderingExtensions.h"
#include "PluginAPI/IUnityShaderCompilerAccess.h"

#endif //PCH_H
