#include "runtimeloadedlibrary.h"
#include <stdexcept>

#ifndef WIN32
#include <dlfcn.h> 
#endif 

namespace SynGlyphX {

	RuntimeLoadedLibrary::RuntimeLoadedLibrary() :
		m_library(nullptr) {
	
	}

	RuntimeLoadedLibrary::RuntimeLoadedLibrary(const std::string& filename) :
		m_library(nullptr) {

		Load(filename);
	}

	RuntimeLoadedLibrary::~RuntimeLoadedLibrary()
	{
		Unload();
	}

	bool RuntimeLoadedLibrary::IsLibraryLoaded() const {

		return (m_library != 0);
	}

	bool RuntimeLoadedLibrary::Load(const std::string& filename) {

		if (IsLibraryLoaded()) {

			Unload();
		}

#ifdef WIN32
		m_library = LoadLibraryA(filename.c_str());
#else
		m_library = dlopen(filename.c_str(), RTLD_NOW);
#endif

		return (m_library != nullptr);
	}

	void RuntimeLoadedLibrary::Unload() {

		if (IsLibraryLoaded()) {

#ifdef WIN32
			FreeLibrary(m_library);
#else
			dlclose(m_library);
#endif
			m_library = nullptr;
		}
	}

	void* RuntimeLoadedLibrary::GetAddress(const std::string& proc) const {

		if (!IsLibraryLoaded()) {

			std::_Xruntime_error("Can not get address when no library is loaded.");
		}

#ifdef WIN32
		return GetProcAddress(m_library, proc.c_str());
#else
		return dlsym(m_library, proc.c_str());
#endif

	}

} //namespace SynGlyphX