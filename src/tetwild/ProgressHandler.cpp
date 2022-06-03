#include <tetwild/ProgressHandler.h>

using namespace tetwild;

static std::shared_ptr<ProgressHandler> gs_DefaultProgressHandler = std::make_shared<DefaultProgressHandler>();

std::shared_ptr<ProgressHandler> ProgressHandler::s_ProgressHandler = gs_DefaultProgressHandler;

void ProgressHandler::SetProgressHandler(std::shared_ptr<ProgressHandler> progressHandler)  // NOLINT(performance-unnecessary-value-param)
{
	s_ProgressHandler = progressHandler ? progressHandler : gs_DefaultProgressHandler;
}

std::shared_ptr<ProgressHandler> ProgressHandler::GetProgressHandler()
{
	return s_ProgressHandler;
}

