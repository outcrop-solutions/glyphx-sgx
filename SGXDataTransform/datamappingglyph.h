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

#ifndef SYNGLYPHX_DATAMAPPINGGLYPH_H
#define SYNGLYPHX_DATAMAPPINGGLYPH_H

#include "sgxdatatransform_global.h"
#include "glyphgraph.h"
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "mindiffdatamappingproperty.h"
#include "datamappingglyphgeometry.h"

namespace SynGlyphX {

	typedef VirtualTopologyTemplate<VirtualTopologyMappingProperty> DataMappingVirtualTopology;

	class SGXDATATRANSFORM_EXPORT DataMappingGlyph : public GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty, DataMappingGlyphGeometry, DataMappingVirtualTopology> {

	public:
		enum MappableField {
			PositionX = 0,
			PositionY,
			PositionZ,
			RotationX,
			RotationY,
			RotationZ,
			ScaleX,
			ScaleY,
			ScaleZ,
			Color,
			Transparency,
			Tag,
			Description,
			RotationRateX,
			RotationRateY,
			RotationRateZ,
			MappableFieldSize //This needs to be at the bottom to get the number of enums
		};

		DataMappingGlyph();
		DataMappingGlyph(const DataMappingGlyph& glyph);
		DataMappingGlyph(const Glyph& glyph, bool isRoot = false);
		DataMappingGlyph(const boost::property_tree::wptree& propertyTree);
		~DataMappingGlyph();

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		SynGlyphX::Glyph GetMinGlyph() const;
		SynGlyphX::Glyph GetDifference() const;
		SynGlyphX::Glyph GetMaxGlyph() const;

		DataMappingGlyph& operator=(const DataMappingGlyph& glyph);
		bool operator==(const DataMappingGlyph& glyph) const;
		bool operator!=(const DataMappingGlyph& glyph) const;

		void SetInputBinding(MappableField field, const InputBinding& binding);
		void ClearInputBinding(MappableField field);
		const InputBinding& GetInputBinding(MappableField field) const;

		bool IsPositionXYBoundToInputFields() const;
		bool IsAnInputFieldBoundToAPosition() const;

	protected:
		void GetXYZNumericPropertiesFromPropertyTree(const boost::property_tree::wptree& propertyTree, NumericMappingPropertyXYZ& prop);
		void AddXYZNumericPropertiesToPropertyTree(boost::property_tree::wptree& propertyTree, const NumericMappingPropertyXYZ& prop) const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGGLYPH_H