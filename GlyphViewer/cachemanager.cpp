#include "cachemanager.h"
#include <boost/uuid/uuid_io.hpp>
#include <stdexcept>

CacheManager::CacheManager()
{

}

CacheManager::~CacheManager()
{

}

void CacheManager::SetBaseCacheDirectory(const std::wstring& newbaseCacheDirectory) {

	boost::filesystem::path newbaseCacheDirectoryPath(newbaseCacheDirectory);
	if (boost::filesystem::exists(newbaseCacheDirectoryPath)) {

		if (!boost::filesystem::is_directory(newbaseCacheDirectoryPath)) {
		
			throw std::invalid_argument("New base directory for cache is not a directory");
		}
	}
	else {

		if (!boost::filesystem::create_directory(newbaseCacheDirectoryPath)) {

			throw std::invalid_argument("New base directory for cache could not be created");
		}
	}

	m_baseCacheDirectoryPath = newbaseCacheDirectoryPath;
}

std::wstring CacheManager::GetCacheDirectory(const boost::uuids::uuid& id) {

	boost::filesystem::path cacheDirectoryPath = m_baseCacheDirectoryPath / (L"cache_" + boost::uuids::to_wstring(id));

	if (boost::filesystem::exists(cacheDirectoryPath)) {

		if (!boost::filesystem::is_directory(cacheDirectoryPath)) {

			throw std::exception("Cache directory exists but it is not a directory");
		}
	}
	else {

		if (!boost::filesystem::create_directory(cacheDirectoryPath)) {

			throw std::exception("Cache directory could not be created");
		}
	}

	return cacheDirectoryPath.wstring();
}