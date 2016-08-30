
#pragma once

namespace SynGlyphX
{
	namespace hal
	{
		class ref_counted
		{
		public:
			ref_counted() : refcount( 1u ) { }
			ref_counted( const ref_counted& ) = delete;
			virtual ~ref_counted() { }
			void addref() { ++refcount; }
			void decref() { --refcount; }
			unsigned int ref_count() { return refcount; }
		private:
			unsigned int refcount;
		};
	}
}
