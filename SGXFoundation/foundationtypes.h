#ifndef SYNGLYPHX_FOUNDATION_TYPES_H
#define SYNGLYPHX_FOUNDATION_TYPES_H

#include <array>
#include <vector>
#include <unordered_map>

namespace SynGlyphX {

	typedef std::vector<std::wstring> WStringVector;
	typedef std::vector<std::string> StringVector;

    typedef std::array<double, 3> Vector3;
    typedef std::array<unsigned char, 4> Color;

} //namespace SynGlyphX

#endif //SYNGLYPHX_FOUNDATION_TYPES_H