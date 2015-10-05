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

#ifndef SYNGLYPHX_INTERPOLATIONMAPPINGDATA_H
#define SYNGLYPHX_INTERPOLATIONMAPPINGDATA_H

#include "sgxdatamapping.h"
#include "datamappingfunction.h"
#include "mindiff.h"
#include "datatransformmapping.h"
#include <boost/bimap.hpp>

namespace SynGlyphX {

	class SGXDATAMAPPING_API InterpolationMappingData : public MappingFunctionData
	{ 
	public:
		enum InputMinMaxType {
			BoundInputField,
			UserSpecified,
			InputFieldGroup
		};

		typedef std::shared_ptr<InterpolationMappingData> SharedPtr;
		typedef std::shared_ptr<const InterpolationMappingData> ConstSharedPtr;

		InterpolationMappingData(bool useLogarithmic = false);
		InterpolationMappingData(const boost::property_tree::wptree& propertyTree);
		InterpolationMappingData(const InterpolationMappingData& data);
		virtual ~InterpolationMappingData();

		InterpolationMappingData& operator=(const InterpolationMappingData& data);
		virtual bool IsSame(const MappingFunctionData& data) const;

		virtual Input GetSupportedInput() const;
		virtual Output GetSupportedOutput() const;

		virtual boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		double Interpolate(const DoubleMinDiff& outputMinDiff, double inputMin, double inputMax, double input) const;
		GlyphColor Interpolate(const ColorMinDiff& outputMinDiff, double inputMin, double inputMax, double input) const;

		InputMinMaxType GetInputMinMaxType() const;

		void SetInputMinMaxToBoundField();

		void SetUserSpecifiedInputMinMax(const DoubleMinDiff& minMax);
		const DoubleMinDiff& GetUserSpecifiedInputMinMax() const;

		void SetInputMinMaxFieldGroup(const DataTransformMapping::FieldGroupName& minMaxFieldGroup);
		const DataTransformMapping::FieldGroupName& GetInputMinMaxFieldGroup() const;

		static const boost::bimap<InputMinMaxType, std::wstring> s_minMaxTypeNames;

	protected:
		double Interpolate(double outputMin, double outputDifference, double inputMin, double inputDifference, double input) const;

		InputMinMaxType m_inputMinMaxType;
		DoubleMinDiff m_userSpecifiedInputMinMax;
		DataTransformMapping::FieldGroupName m_inputMinMaxFieldGroup;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_INTERPOLATIONMAPPINGDATA_H