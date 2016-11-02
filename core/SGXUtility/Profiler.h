#pragma once

//#define SGX_ENABLE_PROFILING //here only for debugging, needs to be in project settings or global include

#if defined(SGX_ENABLE_PROFILING)
#include "sgxutility.h"
#include <vector>
#include <set>
#include <ostream>
#include <cstdint>

// Basic profiler implemtation, output to profile.txt 
// Use SGX_PROFILE_SCOPE to profile scope (common use - put in the beginning of the function) 
// or SGX_BEGIN_PROFILE/SGX_END_PROFILE for fot block of code
// Please note that nested profile blocks are exclusive i.e. time from inside block will not be counted for the outside block
// Currently does not support multithreading (i.e. for accurate results all profiled functions must run on the same thread)
//TODO implement custom log support, multithreading. 

#define STRINGIZE(x) #x
#define SGX_BEGIN_PROFILE(x) SynGlyphX::Profiler::ProfileBlock PROFILE_##x(__FILE__, STRINGIZE(x), __LINE__);
#define SGX_PROFILE_SCOPE SynGlyphX::Profiler::ProfileBlock SGX_PROFILE_BLOCK(__FILE__, __FUNCTION__, __LINE__);
#define SGX_END_PROFILE(x) PROFILE_##x.Stop();

namespace SynGlyphX
{
	namespace Profiler
	{
		class ProfilerConfig;
		//currently unused, will be used in the future to pass profiler settings, for example to pass the Log class 
		//(current imlemetation just save resuts to profile.txt
		void SGXUTILITY_API Configure(const ProfilerConfig& config);

		class SGXUTILITY_API ProfileBlock
		{
		public:
			ProfileBlock(const char * const file, const char * const function, uint_fast32_t line);
			~ProfileBlock();
			void Stop();
		private:
			bool m_isActive;
		};
	}
}
#else
	#define SGX_BEGIN_PROFILE(x) 
	#define SGX_PROFILE_SCOPE 
	#define SGX_END_PROFILE(x) 
#endif //#if defined(SGX_ENABLE_PROFILING)
