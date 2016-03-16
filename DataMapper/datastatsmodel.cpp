#include "datastatsmodel.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QtCore/QMimeData>
#include "inputfieldmimedata.h"
#include <boost/uuid/uuid_io.hpp>

DataStatsModel::DataStatsModel(const boost::uuids::uuid& id, QString tablename, DataTransformModel* model, QObject *parent)
	: QAbstractTableModel(parent),
	m_table(id, tablename.toStdWString()),
	m_model(model)
{
	const DataTransformModel::TableStats& tableStats = m_model->GetTableStatsMap().at(m_table);
	for (const auto& fieldStats : tableStats) {

		if (fieldStats.at(1) == "real") {

			m_fieldTypes.append(QVariant::Type::Double);
		}
		else {

			m_fieldTypes.append(QVariant::Type::String);
		}
	}
}

DataStatsModel::~DataStatsModel()
{

}

int DataStatsModel::rowCount(const QModelIndex& parent) const {

	return m_model->GetTableStatsMap().at(m_table).size();
}

int DataStatsModel::columnCount(const QModelIndex& parent) const {

	return 7;
}

QVariant DataStatsModel::data(const QModelIndex& index, int role) const {

	if ((role == Qt::DisplayRole) && (index.isValid())) {
		
		return m_model->GetTableStatsMap().at(m_table).at(index.row()).at(index.column());
	}

	return QVariant();
}

QVariant DataStatsModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {

			if (section == 0) {
				return "Field Name";
			}
			else if (section == 1) {
				return "Type";
			}
			else if (section == 2) {
				return "Min";
			}
			else if (section == 3) {
				return "Max";
			}
			else if (section == 4) {
				return "Average";
			}
			else if (section == 5) {
				return "Count (Non-Null)";
			}
			else if (section == 6) {
				return "Count (Distinct)";
			}
		}
	}

	return QVariant();
}


QStringList DataStatsModel::mimeTypes() const {

	QStringList list;
	list.push_back("application/datasource-field");
	return list;
}

QMimeData* DataStatsModel::mimeData(const QModelIndexList& indexes) const {
	
	if (indexes.isEmpty() || (!indexes.front().isValid())) {

		return nullptr;
	}
	
	QString fieldName = m_model->GetTableStatsMap().at(m_table).at(indexes.front().row()).at(0);

	SynGlyphX::InputField::Type type = SynGlyphX::InputField::Type::Null;
	QVariant::Type fieldType = m_fieldTypes[indexes.front().row()];
	if ((fieldType == QVariant::Type::Char) || (fieldType == QVariant::Type::String) || (fieldType == QVariant::Type::Url)) {
		type = SynGlyphX::InputField::Type::Text;
	}
	else if (fieldType == QVariant::Type::Double) {
		type = SynGlyphX::InputField::Type::Real;
	}
	else if ((fieldType == QVariant::Type::Int) || (fieldType == QVariant::Type::UInt) || (fieldType == QVariant::Type::LongLong) || (fieldType == QVariant::Type::ULongLong)) {
		type = SynGlyphX::InputField::Type::Integer;
	}
	else if ((fieldType == QVariant::Type::Date) || (fieldType == QVariant::Type::DateTime) || (fieldType == QVariant::Type::Time)) {
		type = SynGlyphX::InputField::Type::DateTime;
	}
	
	SynGlyphX::InputField inputfield(m_table.GetDatasourceID(), m_table.GetTable(), fieldName.toStdWString(), type);
	InputFieldMimeData* mimeData = new InputFieldMimeData(inputfield);
	
	return mimeData;
}

Qt::ItemFlags DataStatsModel::flags(const QModelIndex& index) const {

	Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

	if (index.isValid()) {

		return Qt::ItemIsDragEnabled | defaultFlags;
	}
	else {

		return defaultFlags;
	}
}