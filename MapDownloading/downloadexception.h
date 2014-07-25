#ifndef DOWNLOADEXCEPTION_H
#define DOWNLOADEXCEPTION_H

#include "mapdownloading_global.h"
#include <exception>

class MAPDOWNLOADING_EXPORT DownloadException : public std::exception
{

public:
	DownloadException(const char* const& message);
	~DownloadException();
};

#endif //DOWNLOADEXCEPTION_H