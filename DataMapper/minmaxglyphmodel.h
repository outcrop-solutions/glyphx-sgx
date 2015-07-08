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

#ifndef MINMAXGLYPHMODEL_H
#define MINMAXGLYPHMODEL_H

#include <QtCore/QAbstractTableModel>
#include "datamappingglyphgraph.h"
#include "datatransformmodel.h"
#include "datamappingfunction.h"

class MinMaxGlyphModel : public QAbstractTableModel
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

	MinMaxGlyphModel(DataTransformModel* dataTransformModel, QObject *parent = nullptr);
	~MinMaxGlyphModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	//virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	SynGlyphX::DataTransformMapping::ConstSharedPtr GetDataTransformMapping() const;
	bool IsCurrentGlyphRoot() const;
	bool IsClear() const;

	SynGlyphX::MappingFunctionData::ConstSharedPtr GetMappingFunction(int row) const;
	void SetMappingFunction(int row, SynGlyphX::MappingFunctionData::SharedPtr mappingFunction);

	//const SynGlyphX::DataMappingGlyphGeometry& GetGlyphGeometry() const;
	//const SynGlyphX::DataMappingVirtualTopology& GetVirtualTopology() const;
	//void SetGlyphGeometryAndVirtualTopology(const SynGlyphX::GlyphGeometry& structure, const SynGlyphX::VirtualTopology& virtualTopology);

public slots:
	void SetMinMaxGlyph(const QModelIndex& index);
	void Clear();
	void ClearInputBindings();

private:
	//QVariant GetDataByRow(SynGlyphX::DataMappingGlyph& glyph, const QModelIndex& index) const;
	//bool SetDataByRow(SynGlyphX::DataMappingGlyph& glyph, const QVariant& value, const QModelIndex& index);
	//SynGlyphX::NumericMappingProperty& GetGlyphProperty(SynGlyphX::DataMappingGlyph& glyph, int row) const;
	PropertyType GetFieldType(int row) const;
	SynGlyphX::MappingFunctionData::SharedPtr CreateNewMappingFunction(SynGlyphX::MappingFunctionData::Function function, PropertyType type) const;
	const SynGlyphX::InputField& GetInputField(SynGlyphX::InputField::HashID fieldID) const;

	//SynGlyphX::DataMappingGlyphGraph::const_iterator m_glyph;
	//SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr m_glyphTree;
	boost::uuids::uuid m_glyphTreeID;
	QStringList m_propertyHeaders;
	QStringList m_columnHeaders;
	DataTransformModel* m_dataTransformModel;

	QPersistentModelIndex m_selectedDataTransformModelIndex;
};

#endif // MINMAXGLYPHMODEL_H
