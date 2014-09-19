#include "filesystem.h"
#include <stdexcept>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	Filesystem::Filesystem()
	{
	}


	Filesystem::~Filesystem()
	{
	}

	void Filesystem::CopyDirectory(const std::string& sourceDir, const std::string& destinationDir, bool recursive) {

		boost::filesystem::path sourcePath(sourceDir);
		if (!boost::filesystem::exists(sourcePath)) {

			throw std::invalid_argument("CopyDirectory: Source does not exist");
		}
		if (!boost::filesystem::is_directory(sourcePath)) {

			throw std::invalid_argument("CopyDirectory: Source is not a directory");
		}

		boost::filesystem::path destinationPath(destinationDir);
		if (boost::filesystem::exists(destinationPath)) {

			if (!boost::filesystem::is_directory(destinationPath)) {

				throw std::invalid_argument("CopyDirectory: Destination is not a directory");
			}
		}

		CopyDirectory(sourcePath, destinationPath, recursive);
	}

	void Filesystem::CopyDirectory(const boost::filesystem::path& sourcePath, const boost::filesystem::path& destinationPath, bool recursive) {

		if (!boost::filesystem::exists(destinationPath)) {

			if (!boost::filesystem::create_directory(destinationPath)) {

				throw std::invalid_argument("CopyDirectory: Unable to create destination directory");
			}
		}

		for (boost::filesystem::directory_iterator iT(sourcePath); iT != boost::filesystem::directory_iterator(); ++iT) {

			if ((boost::filesystem::is_directory(iT->status())) && recursive) {

				CopyDirectory(iT->path(), destinationPath / iT->path().filename(), recursive);
			}
			else {

				boost::filesystem::copy_file(iT->path(), destinationPath / iT->path().filename());
			}
		}
	}

} //namespace SynGlyphX