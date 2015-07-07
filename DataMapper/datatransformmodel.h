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

#ifndef DATATRANSFORMMODEL_H
#define DATATRANSFORMMODEL_H

#include "datatransformmapping.h"
#include "sourcedatamanager.h"
#include <QtCore/QAbstractItemModel>

class DataTransformModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	static const int DataTypeRole = Qt::UserRole;

	enum DataType {

		GlyphTrees = 0,
		BaseObjects = 1,
		DataSources = 2
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
		GeometryShape = RotationRateZ + 1,
		GeometrySurface = GeometryShape + 1,
		GeometryTorusRatio = GeometrySurface + 1,
		VirtualTopology = GeometryTorusRatio + 1
	};

	DataTransformModel(QObject *parent);
	~DataTransformModel();

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

	SynGlyphX::DataTransformMapping::ConstSharedPtr GetDataMapping() const;

	void SetDefaults(const SynGlyphX::DataMappingDefaults& defaults);
	void SetSceneProperties(const SynGlyphX::SceneProperties& sceneProperties);

	void LoadDataTransformFile(const QString& filename);
	void SaveDataTransformFile(const QString& filename);
	void ClearAndReset();

	void AddGlyphFile(const QString& filename);
	void AddGlyphTree(SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphTree);
	void UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyph& newGlyph);
	void UpdateGlyphGeometry(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGeometry& structure);
	void UpdateVirtualTopology(const QModelIndex& index, const SynGlyphX::DataMappingVirtualTopology& virtualTopology);
	const SynGlyphX::DataMappingGlyph& GetGlyph(const QModelIndex& index) const;
	void AddChildGlyph(const QModelIndex& parent, const SynGlyphX::DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren = 1);

	void SetBaseObject(unsigned int position, const SynGlyphX::BaseImage& baseImage);
	void AddBaseObject(const SynGlyphX::BaseImage& baseImage);

	boost::uuids::uuid AddFileDatasource(SynGlyphX::FileDatasource::SourceType type, const std::wstring& name);

	void SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field, const SynGlyphX::InputField& inputfield);
	void SetInputField(const boost::uuids::uuid& treeID, const QModelIndex& index, SynGlyphX::DataMappingGlyph::MappableField field, const SynGlyphX::InputField& inputfield);
	void ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field);
	void ClearInputBinding(const boost::uuids::uuid& treeID, const QModelIndex& index, SynGlyphX::DataMappingGlyph::MappableField field);

	void EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableSet& tables, bool enable = true);

	void ResetDataMappingID();

	virtual QStringList mimeTypes() const;
	virtual bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const;
	virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
	virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

	const boost::uuids::uuid& GetCacheConnectionID() const;

private:
	void Clear();
	QString GetCacheLocationForID(const boost::uuids::uuid& id);
	QVariant GetGlyphData(const QModelIndex& index) const;
	bool IsParentlessRowInDataType(DataType type, int row) const;
	DataType GetDataType(const QModelIndex& index) const;
	boost::uuids::uuid GetTreeId(int row) const;
	boost::uuids::uuid GetTreeId(const QModelIndex& index) const;

	SynGlyphX::SourceDataManager m_sourceDataManager;
	SynGlyphX::DataTransformMapping::SharedPtr m_dataMapping;
};

Q_DECLARE_METATYPE(SynGlyphX::DoubleMinDiff)
Q_DECLARE_METATYPE(SynGlyphX::ColorMinDiff)
Q_DECLARE_METATYPE(SynGlyphX::NumericMappingProperty)
Q_DECLARE_METATYPE(SynGlyphX::ColorMappingProperty)
Q_DECLARE_METATYPE(SynGlyphX::TextMappingProperty)
Q_DECLARE_METATYPE(SynGlyphX::GeometryShapeMappingProperty)
Q_DECLARE_METATYPE(SynGlyphX::GlyphGeometryInfo::Shape)
Q_DECLARE_METATYPE(SynGlyphX::GlyphGeometryInfo::Surface)
Q_DECLARE_METATYPE(SynGlyphX::VirtualTopologyMappingProperty)

#endif // DATATRANSFORMMODEL_H
