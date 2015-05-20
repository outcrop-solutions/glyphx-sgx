#include "sourcedatainfomodel.h"
#include <boost/uuid/uuid_io.hpp>

const int SourceDataInfoModel::IDRole = Qt::UserRole;

SourceDataInfoModel::SourceDataInfoModel(SynGlyphX::DataTransformMapping::SharedPtr dataTransformMapping, SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, QObject *parent)
	: QStandardItemModel(parent),
	m_dataTransformMapping(dataTransformMapping),
	m_sourceDataCache(sourceDataCache),
	m_datasourceFlags(Qt::ItemIsEnabled),
	m_tableFlags(Qt::ItemIsEnabled),
	m_columnFlags(Qt::ItemIsEnabled),
	m_showUnmappedTables(false)
{
	setColumnCount(1);
}

SourceDataInfoModel::~SourceDataInfoModel()
{

}

void SourceDataInfoModel::SetShowColumns(bool show) {

	if (show) {

		m_tableFlags |= Qt::ItemNeverHasChildren;
	}
	else {

		m_tableFlags &= ~Qt::ItemNeverHasChildren;
	}

	if (rowCount() > 0) {

		Reset();
	}
}

void SourceDataInfoModel::SetSelectable(bool datasourcesSelectable, bool tableSelectable, bool columnsSelectable) {

	if (datasourcesSelectable) {

		m_datasourceFlags |= Qt::ItemIsSelectable;
	}
	else {

		m_datasourceFlags &= ~Qt::ItemIsSelectable;
	}

	if (tableSelectable) {

		m_tableFlags |= Qt::ItemIsSelectable;
	}
	else {

		m_tableFlags &= ~Qt::ItemIsSelectable;
	}

	if (columnsSelectable) {

		m_columnFlags |= Qt::ItemIsSelectable;
	}
	else {

		m_columnFlags &= ~Qt::ItemIsSelectable;
	}

	if (rowCount() > 0) {

		Reset();
	}
}

void SourceDataInfoModel::Reset() {

	beginResetModel();

	//Blocking signals so that this whole operation is treated as a single reset model and nothing else
	bool signalsWereBlocked = signalsBlocked();
	blockSignals(true);

	clear();
	const SynGlyphX::DatasourceMaps& datasourceMaps = m_dataTransformMapping->GetDatasources();
	for (auto fileDatasource : datasourceMaps.GetFileDatasources()) {

		AddDatasource(fileDatasource.first, fileDatasource.second);
	}

	blockSignals(signalsWereBlocked);

	endResetModel();
}

void SourceDataInfoModel::AddDatasource(const boost::uuids::uuid& id, const SynGlyphX::Datasource& datasource) {

	QStandardItem* newDatasourceItem = new QStandardItem(QString::fromStdWString(datasource.GetFormattedName()));
	newDatasourceItem->setData(QString::fromStdString(boost::uuids::to_string(id)), IDRole);
	newDatasourceItem->setFlags(m_datasourceFlags);
	for (auto table : datasource.GetTables()) {

		AddTable(id, QString::fromStdWString(table), newDatasourceItem);
	}

	if (newDatasourceItem->rowCount() > 0) {

		appendRow(newDatasourceItem);
	}
	else {

		delete newDatasourceItem;
	}
}

void SourceDataInfoModel::AddTable(const boost::uuids::uuid& id, const QString& table, QStandardItem* newDatasourceItem) {

	bool isTableInCache = m_sourceDataCache->IsTableInCache(id, table);
	if (isTableInCache || m_showUnmappedTables) {

		bool addColumns = !m_tableFlags.testFlag(Qt::ItemNeverHasChildren);
		QStandardItem* newTableItem = new QStandardItem(table);
		if (isTableInCache) {

			newTableItem->setFlags(m_tableFlags);
		}
		else {

			newTableItem->setFlags(addColumns ? Qt::NoItemFlags : Qt::ItemNeverHasChildren);
		}
		newDatasourceItem->appendRow(newTableItem);

		if (addColumns) {

			SynGlyphX::TableColumns columns = m_sourceDataCache->GetColumnsForTable(id, table);
			for (auto column : columns) {

				QStandardItem* newColumnItem = new QStandardItem(column);
				newColumnItem->setFlags(m_columnFlags | Qt::ItemNeverHasChildren);
				newTableItem->appendRow(newColumnItem);
			}
		}
	}
}
