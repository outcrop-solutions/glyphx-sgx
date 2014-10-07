#ifndef SYNGLYPHX_FILESYSTEM
#define SYNGLYPHX_FILESYSTEM

#include "sgxfoundation.h"
#include <string>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	class SGXFOUNDATION_API Filesystem
	{
	public:
		Filesystem();
		~Filesystem();

		static void RemoveContentsOfDirectory(const std::string& directory);
		static void CopyDirectory(const std::string& sourceDir, const std::string& destinationDir, bool recursive = false);

	private:
		static void CopyDirectory(const boost::filesystem::path& sourcePath, const boost::filesystem::path& destinationPath, bool recursive);
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_FILESYSTEM