#ifndef SYNGLYPHX_MINMAXGLYPH_H
#define SYNGLYPHX_MINMAXGLYPH_H

#include "sgxdatatransform_global.h"
#include "glyphproperties.h"
#include "glyph.h"
#include <vector>
#include <containers/ntree.hpp>
#include <boost/property_tree/ptree.hpp>
#include "inputfield.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT MinMaxGlyph
	{
	public:
		static const unsigned int NumInputFields = 13;
		
		typedef boost::property_tree::wptree PropertyTree;

		MinMaxGlyph(const GlyphProperties& minGlyph);
		MinMaxGlyph(const boost::property_tree::wptree& propertyTree);
		MinMaxGlyph(const MinMaxGlyph& glyph);
		~MinMaxGlyph();

		MinMaxGlyph& operator=(const MinMaxGlyph& glyph);

		const GlyphProperties& GetMinGlyph() const;
		const GlyphMappableProperties& GetDifference() const;

		void SetMinGlyphProperties(const GlyphMappableProperties& glyph);
		void SetDifference(const GlyphMappableProperties& difference);

		PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		const InputField& GetInputField(unsigned int index) const;
		void SetInputField(unsigned int index, const InputField& inputfield);

	private:
		void AddVector3ToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, const Vector3& min, const Vector3& difference, const InputField inputfields[3]) const;
		void AddValueToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, double min, double difference, const InputField& inputfield) const;
		void AddColorToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const Color& min, const Color& difference, const InputField inputfield[2]) const;
		void AddStringToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, const InputField& inputfield) const;

		void GetVector3FromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, Vector3& min, Vector3& difference, InputField inputfields[3]) const;
		void GetValueFromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, double& min, double& difference, InputField& inputfield) const;
		void GetColorFromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, Color& min, Color& difference, InputField inputfield[2]) const;
		void GetStringFromPropertyTree(const boost::property_tree::wptree& propertyTreeParent, InputField& inputfield) const;

		GlyphProperties m_minGlyph;
		//Rather than store both a min and a max glyph, the min glyph is stored (listed above) in this class plus an object that stores the difference between min and max
		//(listed below).  If a value in m_difference is 0 then min and max is the same.  Otherwise the max value is the min value plus the difference value.
		GlyphMappableProperties m_difference;

		InputField m_inputfields[NumInputFields];
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPH_H