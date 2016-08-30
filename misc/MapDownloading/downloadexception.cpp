#include "downloadexception.h"


DownloadException::DownloadException(const char* const& message) :
	std::exception(message)
{
}


DownloadException::~DownloadException()
{
}
