
#pragma once

#include <string>

namespace SynGlyphX
{
	// Increment this for the first release of a new version (that is, the first
	// time we provide it to non-developers).
	inline const char* getAppVersionString() { return "1.2.00"; }

	// Increment this for EVERY build that's released (so we can use it to know
	// EXACTLY which build everyone is using).
	// @todo - eventually we should automate incrementing this.
	inline unsigned int getBuildNumber() { return 361u; }

	// Combine the two above strings.
	inline std::string getFullVersionString()
	{
		char buf[128];
		sprintf(buf, "%s build %04d", getAppVersionString(), getBuildNumber());
		return std::string(buf);
	}
}
