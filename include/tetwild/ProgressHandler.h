#pragma once

#include "Logger.h"

#include <spdlog/spdlog.h>
#include <memory>


namespace tetwild
{
	/// <summary>
	/// An interface for defining a progress handler used by the `tetrahedralization` method.
	/// </summary>
	class ProgressHandler
	{
	public:
		/// <summary>
		/// Send a trace message to the progress handler.
		/// </summary>
		/// <param name="fmt">A trace message to display.</param>
		/// <param name="args">Additional arguments to pass to the formatter.</param>
		template<typename... FmtArgs>
		static void Trace(const char* fmt, FmtArgs&&... args)
		{
			if (const auto progressHandler = s_ProgressHandler.lock())
			{
				progressHandler->DoMessage(spdlog::level::trace, FormatToString(fmt, std::forward<FmtArgs>(args)...));
			}
		}

		/// <summary>
		/// Send a debug message to the progress handler.
		/// </summary>
		/// <param name="fmt">A debug message to display.</param>
		/// <param name="args">Additional arguments to pass to the formatter.</param>
		template<typename... FmtArgs>
		static void Debug(const char* fmt, FmtArgs&&... args)
		{
			if (const auto progressHandler = s_ProgressHandler.lock())
			{
				progressHandler->DoMessage(spdlog::level::debug, FormatToString(fmt, std::forward<FmtArgs>(args)...));
			}
		}
		/// <summary>
		/// Send an info message to the progress handler.
		/// </summary>
		/// <param name="fmt">An info message to display.</param>
		/// <param name="args">Additional arguments to pass to the formatter.</param>
		template<typename... FmtArgs>
		static void Info(const char* fmt, FmtArgs&&... args)
		{
			if (const auto progressHandler = s_ProgressHandler.lock())
			{
				progressHandler->DoMessage(spdlog::level::info, FormatToString(fmt, std::forward<FmtArgs>(args)...));
			}
		}

		/// <summary>
		/// Send a warning message to the progress handler.
		/// </summary>
		/// <param name="fmt">A warning message to display.</param>
		/// <param name="args">Additional arguments to pass to the formatter.</param>
		/// <returns>`true` to continue the operation, or `false` to cancel the operation at the next possible opportunity.</returns>
		template<typename... FmtArgs>
		static void Warn(const char* fmt, FmtArgs&&... args)
		{
			if (const auto progressHandler = s_ProgressHandler.lock())
			{
				progressHandler->DoMessage(spdlog::level::warn, FormatToString(fmt, std::forward<FmtArgs>(args)...));
			}
		}

		/// <summary>
		/// Send an error message to the progress handler.
		/// </summary>
		/// <param name="fmt">An error message to display.</param>
		/// <param name="args">Additional arguments to pass to the formatter.</param>
		/// <returns>`true` to continue the operation, or `false` to cancel the operation at the next possible opportunity.</returns>
		template<typename... FmtArgs>
		static void Error(const char* fmt, FmtArgs&&... args)
		{
			if (const auto progressHandler = s_ProgressHandler.lock())
			{
				progressHandler->DoMessage(spdlog::level::err, FormatToString(fmt, std::forward<FmtArgs>(args)...));
			}
		}

		/// <summary>
		/// Send a critical message to the progress handler.
		/// </summary>
		/// <param name="fmt">A critical message to display.</param>
		/// <param name="args">Additional arguments to pass to the formatter.</param>
		/// <returns>`true` to continue the operation, or `false` to cancel the operation at the next possible opportunity.</returns>
		template<typename... FmtArgs>
		static void Critical(const char* fmt, FmtArgs&&... args)
		{
			if (const auto progressHandler = s_ProgressHandler.lock())
			{
				progressHandler->DoMessage(spdlog::level::critical, FormatToString(fmt, std::forward<FmtArgs>(args)...));
			}
		}

		/// <summary>
		/// Check if the current operation should continue or quit.
		/// </summary>
		/// <returns>`true` to continue the current operation, or `false` to cancel the current operation.</returns>
		static bool ShouldContinue()
		{
			if (const auto progressHandler = s_ProgressHandler.lock())
			{
				return progressHandler->DoShouldContinue();
			}

			return true;
		}

		/// <summary>
		/// Set the global progress handler.
		/// </summary>
		/// <param name="progressHandler">A pointer to a progress handler.</param>
		static void SetProgressHandler(std::shared_ptr<ProgressHandler> progressHandler);

		/// <summary>
		/// Get a pointer to the current progress handler.
		/// </summary>
		/// <returns>A valid pointer to a progress handler or nullptr if no progress handler has been registered.</returns>
		static std::shared_ptr<ProgressHandler> GetProgressHandler();

	protected:
		/// <summary>
		/// Override this method in your own implementation.
		/// </summary>
		/// <param name="level"></param>
		/// <param name="message">A progress message to display.</param>
		virtual void DoMessage(spdlog::level::level_enum level, const std::string& message) const = 0;

		/// <summary>
		/// Check if the current operation should continue or quit.
		/// </summary>
		///	<remarks>
		/// Override this method in your own progress handler implementations.
		///	</remarks>
		/// <returns>`true` to continue the current operation, or `false` to cancel the current operation.</returns>
		virtual bool DoShouldContinue() const = 0;

		ProgressHandler() = default;
		virtual ~ProgressHandler() = default;
		ProgressHandler(const ProgressHandler&) = default;
		ProgressHandler(ProgressHandler&&) = default;
		ProgressHandler& operator=(const ProgressHandler&) = default;
		ProgressHandler& operator=(ProgressHandler&&) = default;

	private:
		/// <summary>
		/// Convert a format message to a string.
		/// </summary>
		/// <param name="fmt">A format message.</param>
		/// <param name="args">Additional format arguments.</param>
		/// <returns>The formatted string.</returns>
		template<typename... Args>
		static std::string FormatToString(const char* fmt, Args&&... args)
		{
			fmt::memory_buffer buf;
			fmt::format_to(buf, fmt, std::forward<Args>(args)...);
			return fmt::to_string(buf);
		}

		/// <summary>
		/// A global pointer to the progress handler.
		/// </summary>
		static std::weak_ptr<ProgressHandler> s_ProgressHandler;
	};

	// Implementation of a default progress handler:
	class DefaultProgressHandler : public ProgressHandler
	{
	public:
		bool DoShouldContinue() const override
		{
			return true;
		}

		void DoMessage(spdlog::level::level_enum lvl, const std::string& message ) const override
		{
			logger().log(lvl, message);
		}
	};
}
