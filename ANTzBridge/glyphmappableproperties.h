#ifndef SYNGLYPHX_GLYPHMAPPABLEPROPERTIES
#define SYNGLYPHX_GLYPHMAPPABLEPROPERTIES

#include "glyphnumericmappableproperties.h"
#include <string>

namespace SynGlyphXANTz {

    class ANTZBRIDGE_API GlyphMappableProperties : public GlyphNumericMappableProperties
    {
    public:
        GlyphMappableProperties();
        GlyphMappableProperties(const GlyphMappableProperties& properties);
        virtual ~GlyphMappableProperties();

		GlyphMappableProperties& operator=(const GlyphMappableProperties& properties);
		bool operator==(const GlyphMappableProperties& properties) const;
		bool operator!=(const GlyphMappableProperties& properties) const;
		//bool operator<(const GlyphMappableProperties& properties) const;

		void SetTag(const std::wstring& tag);
		const std::wstring& GetTag() const;

    protected:
		std::wstring m_tag;
    };

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_GLYPHMAPPABLEPROPERTIES
