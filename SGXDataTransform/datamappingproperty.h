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

#ifndef SYNGLYPHX_DATAMAPPINGPROPERTY_H
#define SYNGLYPHX_DATAMAPPINGPROPERTY_H

#include "sgxdatatransform_global.h"
#include <array>
#include <boost/property_tree/ptree.hpp>
#include "glyphcolor.h"
#include "inputbinding.h"
#include <memory>
#include "datamappingfunction.h"
#include "glyphgeometryinfo.h"
#include "virtualtopologyinfo.h"

namespace SynGlyphX {

	template<typename PropertyType> //, typename MappingOutputType = PropertyType>
	class SGXDATATRANSFORM_EXPORT DataMappingProperty {

	public:
		DataMappingProperty();
		DataMappingProperty(const PropertyType& initialValue);
		DataMappingProperty(const boost::property_tree::wptree& propertyTree);
		DataMappingProperty(const DataMappingProperty& prop);
		~DataMappingProperty();

		DataMappingProperty& operator=(const DataMappingProperty& prop);
		bool operator==(const DataMappingProperty& prop) const;
		bool operator!=(const DataMappingProperty& prop) const;

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		PropertyType& GetValue();
		const PropertyType& GetValue() const;

		InputBinding& GetBinding();
		const InputBinding& GetBinding() const;

		MappingFunctionData::ConstSharedPtr GetMappingFunctionData() const;
		void SetMappingFunctionData(MappingFunctionData::SharedPtr mappingFunctionData);

	protected:
		bool IsMappingFunctionDataCompatible(MappingFunctionData::SharedPtr mappingFunctionData) const;
		void ExportValueToPropertyTree(boost::property_tree::wptree& propertyTree) const;
		void ImportValueToPropertyTree(const boost::property_tree::wptree& propertyTree);

		void ChangeMappingFunction(MappingFunctionData::Function function, const boost::property_tree::wptree& propertyTree);

		PropertyType m_value;
		InputBinding m_binding;
		MappingFunctionData::SharedPtr m_mappingFunctionData;
	};

	typedef DataMappingProperty<std::wstring> TextMappingProperty;

	typedef DataMappingProperty<std::pair<double, double>> NumericMappingProperty;
	typedef DataMappingProperty<std::pair<GlyphColor, GlyphColor>> ColorMappingProperty;

	typedef std::array<NumericMappingProperty, 3> NumericMappingPropertyXYZ;

	typedef DataMappingProperty<GlyphGeometryInfo::Shape> GeometryShapeMappingProperty;
	typedef DataMappingProperty<VirtualTopologyInfo::Type> VirtualTopologyMappingProperty;

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGPROPERTY_H