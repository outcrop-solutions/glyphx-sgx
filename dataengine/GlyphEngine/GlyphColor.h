///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///
#pragma once
#ifndef GLYPHCOLOR_H
#define GLYPHCOLOR_H

#include "GlyphEngine_Exports.h"
#include <array>
#include<boost/property_tree/ptree_fwd.hpp>

namespace GlyphEngine {

	class GLYPHENGINE GlyphColor
	{
	public:
		enum Space {RGB, HSV};

		typedef std::array<short, 3> ColorArray;

		GlyphColor(Space space = Space::RGB);
		GlyphColor(const ColorArray& color, Space space = Space::RGB);
		//GlyphColor(const boost::property_tree::wptree& propertyTree, Space space = Space::RGB);
		GlyphColor(const GlyphColor& color);
		~GlyphColor();

		GlyphColor& operator=(const GlyphColor& color);
		void Set(unsigned int index, short value);
		void Set(short red, short green, short blue);
		void SetF( float r, float g, float b );
		short operator[](unsigned int index) const;
		bool operator==(const GlyphColor& color) const;
		bool operator!=(const GlyphColor& color) const;
		GlyphColor& operator+=(const GlyphColor& color);
		GlyphColor& operator-=(const GlyphColor& color);
		const GlyphColor operator+(const GlyphColor& color) const;
		const GlyphColor operator-(const GlyphColor& color) const;

		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		std::wstring ToHexString(unsigned int length = 3) const;
		void FromHexString(const std::wstring& hexString);

		Space GetSpace() const;

		static GlyphColor ConvertRGBtoHSV(const GlyphColor& color);
		static GlyphColor ConvertHSVtoRGB(const GlyphColor& color);

		static const GlyphColor s_black;
		static const GlyphColor s_white;

	private:
		ColorArray m_color;
		Space m_space;
	};

	//This translator is so that GlyphColor can be automatically used by boost::property_tree
	class GLYPHENGINE GlyphColorTranslator
	{
	public:
		typedef std::wstring internal_type;
		typedef GlyphColor external_type;

		GlyphColorTranslator();

		boost::optional<GlyphColor> get_value(std::wstring const &v);
		boost::optional<std::wstring> put_value(GlyphColor const& v);

	private:
		
	};

} 

namespace boost {

	namespace property_tree {

		template<>
		struct translator_between<std::wstring, GlyphEngine::GlyphColor>
		{
			typedef GlyphEngine::GlyphColorTranslator type;
		};
	}
} //namespace boost


#endif //GLYPHCOLOR_H