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

#ifndef GLYPHROLESTABLEMODEL_H
#define GLYPHROLESTABLEMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtGui/QFont>
#include "datamappingglyphgraph.h"
#include "datatransformmodel.h"
#include "datamappingfunction.h"

class GlyphRolesTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum PropertyType {
		Numeric,
		Color,
		Text,
		VirtualTopology,
		GeometryShape,
		NonMappable
	};

	static const unsigned int s_propertyNameColumn = 0;
	static const unsigned int s_valueColumn = 1;
	static const unsigned int s_mappingDataColumn = 2;
	static const unsigned int s_mappedFieldColumn = 3;

	GlyphRolesTableModel(DataTransformModel* dataTransformModel, QObject *parent = nullptr);
	~GlyphRolesTableModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	SynGlyphX::DataTransformMapping::ConstSharedPtr GetDataTransformMapping() const;
	bool IsCurrentGlyphRoot() const;
	bool IsClear() const;

	SynGlyphX::MappingFunctionData::ConstSharedPtr GetMappingFunction(int row) const;
	void SetMappingFunction(int row, SynGlyphX::MappingFunctionData::SharedPtr mappingFunction);

	bool IsInputFieldCompatible(const SynGlyphX::InputField& inputField) const;

	bool DoesGlyphHaveAssociatedDatasoruceTable() const;
	const SynGlyphX::DatasourceTable& GetAssociatedDatasourceTable() const;

	DataTransformModel* GetSourceModel() const;

public slots:
	void SetMinMaxGlyph(const QModelIndex& index);
	void Clear();
	void ClearInputBindings();

private slots:
	void OnSourceModelDataUpdated(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	void OnAllDataUpdated();

private:
	QString GetPropertyHeader(const QModelIndex& index) const;
	bool IsAllDataAtIndexTheSame(const QModelIndex& index) const;
	QVariant GetEditData(const QModelIndex& index) const;
	void DisconnectAllSignalsFromSourceModel();
	PropertyType GetFieldType(int row) const;
	SynGlyphX::MappingFunctionData::SharedPtr CreateNewMappingFunction(SynGlyphX::MappingFunctionData::Function function, PropertyType type) const;
	const SynGlyphX::InputField GetInputField(SynGlyphX::InputField::HashID fieldID) const;
	bool IsSelectedIndexWithinIndexes(const QModelIndex& topLeft, const QModelIndex& bottomRight) const;

	boost::uuids::uuid m_glyphTreeID;
	QStringList m_propertyHeaders;
	QStringList m_columnHeaders;
	DataTransformModel* m_dataTransformModel;

	QPersistentModelIndex m_selectedDataTransformModelIndex;
	std::vector<QMetaObject::Connection> m_sourceModelConnections;

	QFont m_headerFont;
};

#endif // GLYPHROLESTABLEMODEL_H
