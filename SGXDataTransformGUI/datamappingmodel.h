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

#ifndef SYNGLYPHX_DATATRANSFORMMODEL_H
#define SYNGLYPHX_DATATRANSFORMMODEL_H

#include "sgxdatatransformgui_global.h"
#include "datatransformmapping.h"
#include <QtCore/QAbstractItemModel>
#include "glyphenumcombobox.h"
#include "surfaceradiobuttonwidget.h"

namespace SynGlyphX {

	class SGXDATATRANSFORMGUI_EXPORT DataMappingModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		static const int UUIDRole = Qt::UserRole;
		static const int DataTypeRole = UUIDRole + 1;

		enum DataType {

			GlyphTrees = 0,
			BaseObjects = 1,
			DataSources = 2,
			FieldGroups = 3
		};

		enum PropertyRole {

			PositionX = DataTypeRole + 1,
			PositionY = PositionX + 1,
			PositionZ = PositionY + 1,
			RotationX = PositionZ + 1,
			RotationY = RotationX + 1,
			RotationZ = RotationY + 1,
			ScaleX = RotationZ + 1,
			ScaleY = ScaleX + 1,
			ScaleZ = ScaleY + 1,
			Color = ScaleZ + 1,
			Transparency = Color + 1,
			Tag = Transparency + 1,
			Description = Tag + 1,
			RotationRateX = Description + 1,
			RotationRateY = RotationRateX + 1,
			RotationRateZ = RotationRateY + 1,
			VirtualTopology = RotationRateZ + 1,
			GeometryShape = VirtualTopology + 1,
			GeometrySurface = GeometryShape + 1,
			GeometryTorusRatio = GeometrySurface + 1
		};

		DataMappingModel(QObject *parent);
		~DataMappingModel();

		//Functions from QAbstractItemModel that need to be implemented
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
		virtual QModelIndex	parent(const QModelIndex& index) const;
		virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
		virtual Qt::DropActions supportedDropActions() const;
		virtual Qt::ItemFlags flags(const QModelIndex& index) const;
		virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

		QVariant GetDisplayData(const QModelIndex& index) const;
		QVariant GetDataTypeData(const QModelIndex& index) const;
		QVariant GetPropertyData(const QModelIndex& index, int role) const;

		DataTransformMapping::ConstSharedPtr GetDataMapping() const;

		void SetDefaults(const DataMappingDefaults& defaults);
		void SetSceneProperties(const SceneProperties& sceneProperties);

		void LoadDataTransformFile(const QString& filename);
		void SaveDataTransformFile(const QString& filename);
		void Clear();

		void AddGlyphFile(const QString& filename);
		void AddGlyphTree(DataMappingGlyphGraph::SharedPtr glyphTree);
		void UpdateGlyph(const QModelIndex& index, const DataMappingGlyph& newGlyph);
		void UpdateGlyph(const QModelIndex& index, const DataMappingGlyphGraph& subgraph);
		void UpdateGlyphGeometry(const QModelIndex& index, const DataMappingGlyphGeometry& structure);
		void UpdateVirtualTopology(const QModelIndex& index, const DataMappingVirtualTopology& virtualTopology);
		const DataMappingGlyph& GetGlyph(const QModelIndex& index) const;
		DataMappingGlyphGraph GetSubgraph(const QModelIndex& index, bool includeChildren);
		void AddChildGlyph(const QModelIndex& parent, const DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren = 1);
		void AddChildGlyphGraph(const QModelIndex& parent, const DataMappingGlyphGraph& graph);

		void SetBaseObject(unsigned int position, const BaseImage& baseImage);
		void AddBaseObject(const BaseImage& baseImage);

		boost::uuids::uuid AddFileDatasource(const FileDatasource& datasource);

		//void SetInputField(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field, const InputField& inputfield);
		void SetInputField(const boost::uuids::uuid& treeID, const QModelIndex& index, DataMappingGlyph::MappableField field, const InputField& inputfield);
		//void ClearInputBinding(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field);
		void ClearInputBinding(const boost::uuids::uuid& treeID, const QModelIndex& index, DataMappingGlyph::MappableField field);
		void ClearAllInputBindings(const boost::uuids::uuid& treeID, const QModelIndex& index);

		void EnableTables(const boost::uuids::uuid& id, const Datasource::TableNames& tables, bool enable = true);

		void ResetDataMappingID();

		virtual QStringList mimeTypes() const;
		virtual bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const;
		virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
		virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

		const DataTransformMapping::FieldGroupMap& GetFieldGroupMap() const;
		void UpdateFieldGroup(const DataTransformMapping::FieldGroupName& groupName, const FieldGroup& fieldGroup);
		void RemoveFieldGroup(const DataTransformMapping::FieldGroupName& groupName);

	private:
		QString GetCacheLocationForID(const boost::uuids::uuid& id);
		QVariant GetGlyphData(const QModelIndex& index) const;
		bool IsParentlessRowInDataType(DataType type, int row) const;
		unsigned int GetFirstIndexForDataType(DataType type) const;
		DataType GetDataType(const QModelIndex& index) const;
		boost::uuids::uuid GetTreeId(int row) const;
		boost::uuids::uuid GetTreeId(const QModelIndex& index) const;
		boost::uuids::uuid GetDatasourceId(int row) const;
		const DataTransformMapping::FieldGroupName& GetFieldGroupName(int row) const;
		void RemoveFieldGroup(const DataTransformMapping::FieldGroupName& groupName, bool emitGlyphDataChanged);

		DataTransformMapping::SharedPtr m_dataMapping;
	};

} //namespace SynGlyphX

Q_DECLARE_METATYPE(SynGlyphX::NumericMappingProperty)
Q_DECLARE_METATYPE(SynGlyphX::ColorMappingProperty)
Q_DECLARE_METATYPE(SynGlyphX::TextMappingProperty)
Q_DECLARE_METATYPE(SynGlyphX::GeometryShapeMappingProperty)
Q_DECLARE_METATYPE(SynGlyphX::VirtualTopologyMappingProperty)

#endif // SYNGLYPHX_DATATRANSFORMMODEL_H
