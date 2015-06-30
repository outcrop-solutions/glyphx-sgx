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

#ifndef SYNGLYPHX_TRANSFORMER_H
#define SYNGLYPHX_TRANSFORMER_H

#include "sgxdatatransform_global.h"
#include "glyphgraph.h"
#include "datamappingglyphgraph.h"
#include "geographicboundingbox.h"
#include "inputfielddata.h"
#include "datamappingdefaults.h"
#include "sourcedatacache.h"
#include "baseimage.h"
#include "enumdatamappingproperty.h"

namespace SynGlyphX {

	class DataTransformMapping;

	class SGXDATATRANSFORM_EXPORT Transformer
	{
	public:
		typedef std::unordered_map<SynGlyphX::InputField::HashID, InputFieldData::SharedPtr> InputFieldDataMap;

		Transformer();
		~Transformer();

		void Transform(const DataTransformMapping& mapping);

		const QString& GetSourceDataCacheLocation() const;

		const QString& GetError() const;

	protected:
		virtual void Prepare() = 0;
		virtual void CreateGlyphsFromMapping(const DataTransformMapping& mapping) = 0;

		void SetSourceDataCacheLocation(const QString& sourceDataCacheLocation);
		bool HaveDatasourcesBeenUpdated(const DataTransformMapping& mapping, std::time_t lastUpdateTime) const;
		bool HasFileBeenUpdated(const std::wstring& filename, std::time_t lastUpdateTime) const;

		GlyphGraph::ConstSharedVector CreateGlyphTreesFromMinMaxTrees(const DataTransformMapping& mapping) const;
		GlyphGraph::ConstSharedVector CreateGlyphTreesFromMinMaxTree(DataMappingGlyphGraph::ConstSharedPtr minMaxTree) const;
		Glyph ProcessMinMaxGlyph(const DataMappingGlyphGraph::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const;
		void AddChildrenToGlyphTree(GlyphGraph::SharedPtr tree, GlyphGraph::iterator newNode, DataMappingGlyphGraph::ConstSharedPtr minMaxTree, DataMappingGlyphGraph::const_iterator node, const InputFieldDataMap& queryResultData, unsigned int index) const;
		
		double TransformProperty(const InputBinding& binding, const NumericMappingProperty& mappingProperty, const InputFieldDataMap& queryResultData, unsigned int index) const;
		GlyphColor TransformProperty(const InputBinding& binding, const ColorMappingProperty& mappingProperty, const InputFieldDataMap& queryResultData, unsigned int index) const;
		GlyphGeometryInfo::Shape TransformProperty(const InputBinding& binding, const GeometryShapeMappingProperty& mappingProperty, const InputFieldDataMap& queryResultData, unsigned int index) const;
		VirtualTopologyInfo::Type TransformProperty(const InputBinding& binding, const VirtualTopologyMappingProperty& mappingProperty, const InputFieldDataMap& queryResultData, unsigned int index) const;

		void GetPositionXYForAllGlyphTrees(const SynGlyphX::DataTransformMapping& mapping, std::vector<GeographicPoint>& points) const;
		
		void GetDataMinAndDifference(const InputBinding& binding, const InputFieldData& fieldData, double& dataMin, double& dataDifference) const;
		
		std::wstring GenerateTag(const DataMappingGlyphGraph::const_iterator& minMaxGlyph, const InputFieldDataMap& queryResultData, unsigned int index) const;

		void RunSelectSqlQuery(const InputField& inputfield, QVariantList& data) const;

		bool DownloadBaseImage(const SynGlyphX::DataTransformMapping& mapping, const SynGlyphX::BaseImage& baseImage, const QString& baseImageFilename);
		void CopyImage(const QString& sourceFilename, const QString& baseImageFilename);
		QString GetUserImageFilename(const SynGlyphX::BaseImage& baseImage) const;

		QString m_sourceDataCacheLocation;
		SourceDataCache m_sourceDataCache;
		GeographicBoundingBox m_overrideRootXYBoundingBox;
		DataMappingDefaults m_defaults;

		QString m_error;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_TRANSFORMER_H