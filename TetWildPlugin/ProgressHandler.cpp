#include "pch.h"

#include "ProgressHandler.h"

extern "C" UNITY_INTERFACE_EXPORT ProgressHandler* UNITY_INTERFACE_API ProgressHandler_new(SendProgressMessageCallback cb)
{
	return new ProgressHandler(cb);
}

extern "C" UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API ProgressHandler_delete(ProgressHandler* progressHandler)
{
	delete progressHandler;
}
