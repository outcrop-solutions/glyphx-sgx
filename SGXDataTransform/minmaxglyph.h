#ifndef SYNGLYPHX_MINMAXGLYPH_H
#define SYNGLYPHX_MINMAXGLYPH_H

#include "sgxdatatransform_global.h"
#include "glyphgraph.h"
#include <vector>
#include <containers/ntree.hpp>
#include <boost/property_tree/ptree.hpp>
#include "inputbinding.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT TemplateGlyph {

	public:
		template<typename PropertyType>
		class TemplateProperty {

		public:
			PropertyType value;
			InputBinding binding;

			TemplateProperty();
			TemplateProperty(const PropertyType& initialValue);
			TemplateProperty(const boost::property_tree::wptree& propertyTree);
			TemplateProperty(const TemplateProperty& prop);
			~TemplateProperty();

			boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;
		};

		typedef TemplateProperty<std::pair<double, double>> NumericProperty;
		typedef TemplateProperty<std::pair<Color, Color>> ColorProperty;
		typedef TemplateProperty<std::wstring> TextProperty;
		typedef std::array<NumericProperty, 3> NumericPropertyXYZ;

		NumericPropertyXYZ position;
		NumericPropertyXYZ rotation;
		NumericPropertyXYZ scale;
		ColorProperty color;
		NumericProperty transparency;
		TextProperty tag;
		TextProperty description;

		Glyph::Shape geometryShape;
		Glyph::Surface geometrySurface;
		Glyph::VirtualTopology virtualTopolgy;

		TemplateGlyph();
		TemplateGlyph(const Glyph& glyph);
		TemplateGlyph(const boost::property_tree::wptree& propertyTree);
		~TemplateGlyph();

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		SynGlyphX::Glyph GetMinGlyph() const;
		SynGlyphX::Glyph GetDifference() const;
		SynGlyphX::Glyph GetMaxGlyph() const;

		/*MinMaxGlyph& operator=(const MinMaxGlyph& glyph);
		bool operator==(const MinMaxGlyph& glyph) const;
		bool operator!=(const MinMaxGlyph& glyph) const;

		void SetMinGlyph(const SynGlyphXANTz::GlyphProperties& glyph);
		void SetDifference(const SynGlyphXANTz::GlyphNumericMappableProperties& difference);

		const InputBinding& GetInputBinding(unsigned int index) const;
		void SetInputBinding(unsigned int index, const InputBinding& binding);
		void ClearInputBinding(unsigned int index);*/

		bool IsPositionXYBoundToInputFields() const;

	private:
		void GetXYZNumericPropertiesFromPropertyTree(const boost::property_tree::wptree& propertyTree, NumericPropertyXYZ& prop);
		void AddXYZNumericPropertiesToPropertyTree(boost::property_tree::wptree& propertyTree, const NumericPropertyXYZ& prop) const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPH_H