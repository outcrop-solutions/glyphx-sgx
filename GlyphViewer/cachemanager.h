#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <string>
#include <boost/filesystem.hpp>
#include "uuid.h"

class CacheManager
{
public:
	CacheManager();
	~CacheManager();

	void SetBaseCacheDirectory(const std::wstring& newbaseCacheDirectory);
	std::wstring GetCacheDirectory(const boost::uuids::uuid& id);

private:
	boost::filesystem::path m_baseCacheDirectoryPath;
};

#endif // CACHEMANAGER_H
