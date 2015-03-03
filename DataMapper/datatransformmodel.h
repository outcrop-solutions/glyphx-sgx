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

	DataTransformModel(QObject *parent);
	~DataTransformModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex	parent(const QModelIndex& index) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	QVariant GetDisplayData(const QModelIndex& index) const;
	QVariant GetDataTypeData(const QModelIndex& index) const;

	SynGlyphX::DataTransformMapping::ConstSharedPtr GetDataMapping() const;

	void SetDefaults(const SynGlyphX::DataMappingDefaults& defaults);

	void LoadDataTransformFile(const QString& filename);
	void SaveDataTransformFile(const QString& filename);
	void Clear();

	void AddGlyphFile(const QString& filename);
	void AddGlyphTree(SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphTree);
	void UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyph& newGlyph);
	void UpdateGlyphStructure(const QModelIndex& index, const SynGlyphX::GlyphStructuralProperties& structure);
	const SynGlyphX::DataMappingGlyph& GetGlyph(const QModelIndex& index) const;
	void AddChildGlyph(const QModelIndex& parent, const SynGlyphX::DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren = 1);

	void SetBaseObject(unsigned int position, const SynGlyphX::BaseImage& baseImage);
	void AddBaseObject(const SynGlyphX::BaseImage& baseImage);

	boost::uuids::uuid AddFileDatasource(SynGlyphX::FileDatasource::SourceType type, const std::wstring& name);

	void SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field, const SynGlyphX::InputField& inputfield);
	void ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field);

	void EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableSet& tables, bool enable = true);

	void ResetDataMappingID();

private:
	void SetIntermediateDirectoryToCurrentID();
	QString GetCacheLocationForID(const boost::uuids::uuid& id);
	QVariant GetGlyphData(const QModelIndex& index) const;
	bool IsRowInDataType(DataType type, int row) const;
	boost::uuids::uuid GetTreeId(int row) const;
	boost::uuids::uuid GetTreeId(const QModelIndex& index) const;

	SynGlyphX::SourceDataManager m_sourceDataManager;
	SynGlyphX::DataTransformMapping::SharedPtr m_dataMapping;
};

#endif // DATATRANSFORMMODEL_H
