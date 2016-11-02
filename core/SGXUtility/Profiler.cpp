#include "Profiler.h"
#if defined(SGX_ENABLE_PROFILING)
#include <fstream>
#include <iomanip>
#include <chrono>

namespace SynGlyphX
{
namespace Profiler
{ 
	
void Configure(const ProfilerConfig& /*config*/){}

struct ProfileStat
{
	ProfileStat(const char* const file, const char* const function, unsigned line,
		uint64_t cycles = 0u, uint64_t calls = 0u) :
		m_file(file), m_function(function), m_line(line),
		m_ticks(cycles),
		m_calls(calls)
	{}
	const char* const   m_file = nullptr;
	const char* const   m_function = nullptr;
	const unsigned      m_line = 0u;
	uint64_t            m_ticks = 0u; //milliseconds in current implementation
	uint64_t            m_calls = 0u;
};

class StatSort
{
public:
	bool operator()(const ProfileStat& lhs, const ProfileStat& rhs) const
	{
		if (rhs.m_line == lhs.m_line)
		{
			return 0 < strcmp(rhs.m_file, lhs.m_file);
		}
		else
		{
			return rhs.m_line > lhs.m_line;
		}
	}
};

class ProfilerImpl
{
public:
	~ProfilerImpl();
	static ProfilerImpl* Instance();
	void EnterBlock(const char * const file, const char * const function, const unsigned line);
	void ExitBlock();
	void CreateReport(std::ostream &os);

private:
	ProfilerImpl();
	uint64_t GetTicks() const;
	std::set<ProfileStat, StatSort>  m_log;
	std::vector<ProfileStat* >     m_stack;
	uint64_t         m_callTime;  //profiler overhead time, currently unused 
	uint64_t         m_totalTime;
	uint64_t         m_lastTime;  //The last time we got hte CPU count.
};

ProfilerImpl::ProfilerImpl():
	m_callTime(0),
	m_totalTime(0)
{
	m_log.insert(ProfileStat{ __FILE__, "Root", 0 });
	m_stack.push_back(&const_cast<ProfileStat&>(*(m_log.begin())));
	m_lastTime = GetTicks();
}

ProfilerImpl::~ProfilerImpl()
{
	std::ofstream myfile;
	myfile.open("profile.txt");
	Instance()->CreateReport(myfile);
	myfile.close();
}

ProfilerImpl* ProfilerImpl::Instance()
{
	static ProfilerImpl instance;
	return &instance;
}


void ProfilerImpl::EnterBlock(const char * const file, const char * const function, const unsigned line)

{
	uint64_t endTime = GetTicks();
	if (m_stack.size())
	{
		auto lastMarker = m_stack.rbegin();
		uint64_t runTime = (endTime - m_lastTime) - m_callTime;
		(**lastMarker).m_ticks += runTime;
		m_totalTime += runTime;
	}
	auto result = m_log.insert(ProfileStat{ file, function, line });
	ProfileStat* stat = &const_cast<ProfileStat&>(*(result.first));
	stat->m_calls++;
	m_stack.push_back(stat);
	m_lastTime = GetTicks();
}

void ProfilerImpl::ExitBlock()

{
	uint64_t endTime = GetTicks();
	auto lastStat = m_stack.rbegin();
	uint64_t runTime = endTime - m_lastTime;
	(**lastStat).m_ticks += runTime;
	m_totalTime += runTime;
	m_stack.pop_back();
	m_lastTime = GetTicks();
}



uint64_t ProfilerImpl::GetTicks() const 
{
	//_asm rdtsc
	using namespace std::chrono;
	auto now = high_resolution_clock::now();
	return duration_cast<milliseconds>(now.time_since_epoch()).count();
}

//#pragma warning(disable: 4996) //sprintf

void ProfilerImpl::CreateReport(std::ostream &os)

{
	class ReportSort
	{
	public:
		bool operator()(const ProfileStat& lhs, const ProfileStat& rhs) const
		{
			return lhs.m_ticks > rhs.m_ticks;
		}
	};

	std::set<ProfileStat, ReportSort> sortedLog;
	for (const auto& entry : m_log)
	{
		sortedLog.insert(entry);
	}

	for (const auto& entry : sortedLog)
	{	
		if (entry.m_line)
		{
			os << std::setw(10) << entry.m_ticks << "ms, ";
			os << std::setw(5) << entry.m_calls << " call(s) by " << entry.m_file << "::" << entry.m_function\
				<< ", line " << entry.m_line << std::endl;
		}
	}
}

ProfileBlock::ProfileBlock(const char * const file, const char * const function, uint_fast32_t line):m_isActive(true)
{
	ProfilerImpl::Instance()->EnterBlock(file, function, line);
}

ProfileBlock::~ProfileBlock()
{
	Stop();
}

void ProfileBlock::Stop()
{
	if (m_isActive)
	{
		ProfilerImpl::Instance()->ExitBlock();
		m_isActive = false;
	}
}
}//namespace SynGlyphX
}//namespace Profiler
#endif //#if defined(SGX_ENABLE_PROFILING)