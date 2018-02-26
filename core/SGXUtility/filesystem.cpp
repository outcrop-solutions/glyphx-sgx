#include "filesystem.h"
#include "stringconvert.h"
#include <stdexcept>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	Filesystem::Filesystem()
	{
	}


	Filesystem::~Filesystem()
	{
	}

	void Filesystem::RemoveContentsOfDirectory(const std::string& directory) {

		boost::filesystem::path dirPath(directory);
		for (boost::filesystem::directory_iterator iT(dirPath); iT != boost::filesystem::directory_iterator(); ++iT) {

			boost::filesystem::remove_all(iT->path());
		}
	}

	void Filesystem::CopyDirectoryOverwrite(const std::string& sourceDir, const std::string& destinationDir, bool recursive) {

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

		CopyDirectoryOverwrite(sourcePath, destinationPath, recursive);
	}

	void Filesystem::CopyDirectoryOverwrite(const boost::filesystem::path& sourcePath, const boost::filesystem::path& destinationPath, bool recursive) {

		if (!boost::filesystem::exists(destinationPath)) {

			if (!boost::filesystem::create_directories(destinationPath)) {

				throw std::invalid_argument("CopyDirectory: Unable to create destination directory");
			}
		}

		for (boost::filesystem::directory_iterator iT(sourcePath); iT != boost::filesystem::directory_iterator(); ++iT) {
			bool isDirectory = boost::filesystem::is_directory(iT->status());
			if (isDirectory && recursive) {

				CopyDirectoryOverwrite(iT->path(), destinationPath / iT->path().filename(), recursive);
			}
			else if (!isDirectory) {

				boost::filesystem::copy_file(iT->path(), destinationPath / iT->path().filename(), boost::filesystem::copy_option::overwrite_if_exists);
			}
		}
	}

	void Filesystem::CopyFileOverwrite(const std::string& sourceFile, const std::string& destinationFile) {

		boost::filesystem::path sourcePath(sourceFile);
		if (!boost::filesystem::exists(sourcePath)) {

			throw std::invalid_argument("CopyFileOverwrite: Source does not exist");
		}
		if (!boost::filesystem::is_regular_file(sourcePath)) {

			throw std::invalid_argument("CopyFileOverwrite: Source is not a file");
		}

		boost::filesystem::path destinationPath(destinationFile);
		if (boost::filesystem::exists(destinationPath)) {

			if (!boost::filesystem::is_regular_file(destinationPath)) {

				throw std::invalid_argument("CopyFileOverwrite: Destination exists but it is not a file");
			}
		}

		boost::filesystem::copy_file(sourcePath, destinationPath, boost::filesystem::copy_option::overwrite_if_exists);
	}

	std::wstring Filesystem::IsFileInDirectory(const std::wstring& filename, const std::wstring& directory) {

		boost::filesystem::path path(directory);
		std::wstring rPath = directory, directory2, tempFileName = filename;

		if (boost::filesystem::is_directory(path)) {

			boost::filesystem::path match = boost::filesystem::absolute(boost::filesystem::path(filename).filename(), path);

			if (boost::filesystem::exists(match)) {

				//return boost::filesystem::canonical(match).wstring();
#ifndef WIN32
                return StringConvert::ToStdWString(match.native());
#else
				return match.native();
#endif
			}

		}

		//for the subset visualization- look into the "filename_data/" directory.
		int counter = 0;
		size_t index;

		for (index = filename.length()-1; index > 0; index--)
		{
			if (filename.at(index) == '/'){
				counter++;
			}

			if (counter == 2){
				break;
			}
		}

		rPath.append(tempFileName.substr(index));

		tempFileName = rPath;

		std::wstring::size_type found = tempFileName.rfind(L"/");
		if (found != std::string::npos)
			directory2 = tempFileName.substr(0, found);

		boost::filesystem::path path2(directory2);

		if (boost::filesystem::is_directory(path2)) {
			boost::filesystem::path match2 = boost::filesystem::absolute(boost::filesystem::path(rPath).filename(), path2);

			if (boost::filesystem::exists(match2)) {

#ifndef WIN32
				return StringConvert::ToStdWString(match.native());
#else
				return match2.native();
#endif
			}

		}

		return L"";
	}
    
    void Filesystem::SetExecutable(const std::string &file)
    {
        boost::filesystem::path path(file);
        boost::filesystem::permissions(path, boost::filesystem::all_all);
    }

} //namespace SynGlyphX
