#include "downloadexception.h"


DownloadException::DownloadException(const char* const& message) :
	std::runtime_error(message)
{
}


DownloadException::~DownloadException()
{
}
