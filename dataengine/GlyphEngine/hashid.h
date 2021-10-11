
#pragma once

#include <cstdint>
#include <string>
#include "uuid.h"

namespace GlyphEngine
{
	// Hash functionality to replace our previous use of boost::hash for stored hashes. That
	// functionality is dependent on sizeof(size_t), so it's platform-dependent, but we stored
	// results in some of our data files. So this replicates (a subset of) the original
	// functionality but for stricly 32-bit hashes, to preserve compatibility.

	typedef uint32_t HashID;

    namespace HashImpl
    {
        template <class T>
        inline HashID Signed(T val)
        {
             const int hashid_bits = std::numeric_limits<HashID>::digits;
             // ceiling(std::numeric_limits<T>::digits / hashid_bits) - 1
             const int length = (std::numeric_limits<T>::digits - 1)
                 / hashid_bits;

             HashID seed = 0;
             T positive = val < 0 ? -1 - val : val;
             for(unsigned int i = length * hashid_bits; i > 0; i -= hashid_bits)
                 seed ^= (HashID) (positive >> i) + (seed<<6) + (seed>>2);
             seed ^= (HashID) val + (seed<<6) + (seed>>2);

             return seed;
        }

        template <class T>
        inline HashID Unsigned(T val)
        {
             const int hashid_bits = std::numeric_limits<HashID>::digits;
             // ceiling(std::numeric_limits<T>::digits / hashid_bits) - 1
             const int length = (std::numeric_limits<T>::digits - 1)
                 / hashid_bits;

             HashID seed = 0;
             for(unsigned int i = length * hashid_bits; i > 0; i -= hashid_bits)
                 seed ^= (HashID) (val >> i) + (seed<<6) + (seed>>2);
             seed ^= (HashID) val + (seed<<6) + (seed>>2);

             return seed;
        }
    }

    // Add overloads for new types here as necessary, making sure to call the appropriate
    // signed/unsigned hash function.
	inline HashID MakeHashID(unsigned int v) { return HashImpl::Unsigned(v); }
	inline HashID MakeHashID(int v) { return HashImpl::Signed(v); }

	inline HashID MakeHashID(std::wstring str)
	{
	    HashID seed = 0;
	    for(auto i = str.begin(); i != str.end(); ++i)
	    {
	        seed ^= static_cast<HashID>(*i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	    }

	    return seed;
	}

	inline HashID MakeHashID(boost::uuids::uuid const& u)
	{
	    HashID seed = 0;
	    for(auto i=u.begin(), e=u.end(); i != e; ++i)
	    {
	        seed ^= static_cast<HashID>(*i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	    }

	    return seed;
	}

	// Combine two hash ids. Note that order matters with HashID operations.
	template<typename T>
	inline void CombineHashID(HashID& seed, const T& v)
	{
        seed ^= MakeHashID(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
    
    inline void CombineHashID(HashID& seed, const boost::uuids::uuid& v)
    {
        seed ^= MakeHashID(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}
