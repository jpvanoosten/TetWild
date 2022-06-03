#pragma once

#include "PluginAPI/IUnityInterface.h"

#include <string>

/// <summary>
/// Define a callback function that can be used to send progress messages back to Unity.
///	The callback function should return `false` if it wants to cancel the operation.
/// </summary>
using SendProgressMessageCallback = bool(*)(float, const char*);

class ProgressHandler
{
public:
	ProgressHandler(SendProgressMessageCallback cb)
		: unityCallback(cb)
		, continueOperation(true)
	{}

	/// <summary>
	/// Send a progress message to Unity through the message callback.
	/// </summary>
	/// <param name="progress">The progress of the operations (0..100%)</param>
	/// <param name="message">An additional message to send to Unity.</param>
	///	<returns>`true` to continue processing, or `false` to stop processing the operation.</returns>
	bool Update(float progress, const std::string& message)
	{
		//if (unityCallback)
		//	continueOperation = unityCallback(progress, message.c_str());

		return continueOperation;
	}

	/// <summary>
	/// Check to see if the operation should continue or be canceled.
	/// </summary>
	/// <returns>`true` if the operation should be continued, or `false` to cancel the operation.</returns>
	bool ShouldContinue() const
	{
		return continueOperation;
	}

private:
	SendProgressMessageCallback unityCallback = nullptr;
	bool continueOperation = true;
};

// Export functions to Unity to create and destroy the progress handlers.
extern "C" UNITY_INTERFACE_EXPORT ProgressHandler * UNITY_INTERFACE_API ProgressHandler_new(SendProgressMessageCallback cb);
extern "C" UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API ProgressHandler_delete(ProgressHandler * ph);