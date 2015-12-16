#include "datastatsmodel.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QtCore/QMimeData>
#include "sourcedatamanager.h"
#include "inputfieldmimedata.h"
#include <boost/uuid/uuid_io.hpp>
#include "dataenginestatement.h"

DataStatsModel::DataStatsModel(const boost::uuids::uuid& id, QString filename, QString tablename, DataEngine::DataEngineConnection *dec, QObject *parent)
	: QAbstractTableModel(parent),
	m_id(id),
	m_tableName(tablename)
{
	GenerateStats(filename, dec);
}

DataStatsModel::~DataStatsModel()
{

}

void DataStatsModel::GenerateStats(QString filename, DataEngine::DataEngineConnection *dec) {

	DataEngine::DataEngineStatement des;
	des.prepare(dec->getEnv(), dec->getJcls());

	std::vector<std::string> colNames = dec->getColumnNames();
	std::vector<std::wstring> numericCols;
	
	for (int i = 0; i < colNames.size(); i++) {

		std::string str = des.getColumnName(i);

		m_fieldNames.append(QString::fromStdString(str));

		if (des.getType(i) == "real"){
			m_fieldTypes.append(QVariant::Type::Double);
			numericCols.push_back(QString(str.c_str()).toStdWString());
			
		}
		else{
			m_fieldTypes.append(QVariant(QString::fromStdString(des.getType(i))).type());
		}

		QStringList fieldStats;

		fieldStats.append(QString::fromStdString(des.getType(i)));
		fieldStats.append(QString::fromStdString(des.getMin(i)));
		fieldStats.append(QString::fromStdString(des.getMax(i)));
		fieldStats.append(QString::fromStdString(des.getAverage(i)));
		fieldStats.append(QString::fromStdString(des.getCount(i)));
		fieldStats.append(QString::fromStdString(des.getDistinct(i)));
		m_stats.append(fieldStats);

	}
	dec->addTableNumericFields(m_id, numericCols);
}

int DataStatsModel::getNumericFieldCount(){
	return numericFieldCount;
}

int DataStatsModel::rowCount(const QModelIndex& parent) const {

	return m_fieldNames.length();
}

int DataStatsModel::columnCount(const QModelIndex& parent) const {

	return 7;
}

QVariant DataStatsModel::data(const QModelIndex& index, int role) const {

	if ((role == Qt::DisplayRole) && (index.isValid())) {
		
		if (index.column() == 0) {
			return m_fieldNames[index.row()];
		} else {
			return m_stats.at(index.row()).at(index.column()-1);
		}
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
	
	QString fieldName = m_fieldNames[indexes.front().row()];

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
		type = SynGlyphX::InputField::Type::Date;
	}
	
	SynGlyphX::InputField inputfield(m_id, m_tableName.toStdWString(), fieldName.toStdWString(), type);
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