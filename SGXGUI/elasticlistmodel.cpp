#include "elasticlistmodel.h"

namespace SynGlyphX {

	ElasticListModel::ElasticListModel(QObject *parent)
		: QAbstractTableModel(parent)
	{

	}

	ElasticListModel::~ElasticListModel()
	{

	}

	void ElasticListModel::ResetData(const Data& data) {

		beginResetModel();
		
		m_rawData.clear();
		m_formattedData.clear();
		m_counts.clear();
		m_percentage.clear();

		for (const auto& dataPair : data) {

			m_formattedData.push_back(dataPair.first.toString());
			m_counts.push_back(dataPair.second);
			if (noTotal){
				total += dataPair.second;
			}
		}
		noTotal = false;

		for (int i = 0; i < m_counts.size(); i++){
			QString percent(QString::number((m_counts.at(i) / total)*100,'f',2));
			m_percentage.push_back(percent);
		}

		//If data is double then we need to make sure that the conversion to string for raw data goes to 15 signifigant digits
		//just in case because SQLite numeric affinity holds that many digits.
		if (data.begin()->first.type() == QVariant::Type::Double) {

			for (const auto& dataPair : data) {

				m_rawData.push_back(ConvertDoubleToRawString(dataPair.first.toDouble()));
			}
		}
		else {

			m_rawData = m_formattedData;
		}

		endResetModel();
	}

	int ElasticListModel::rowCount(const QModelIndex& parent) const {

		return m_rawData.size();
	}

	int ElasticListModel::columnCount(const QModelIndex& parent) const {

		return 3;
	}

	QVariant ElasticListModel::data(const QModelIndex& index, int role) const {

		if (index.isValid()) {

			if (role == Qt::DisplayRole) {

				if (index.column() == 2) {
					return m_percentage.at(index.row());
				}
				else if (index.column() == 1) {

					return m_counts.at(index.row());
				}
				else if (index.column() == 0) {

					return m_formattedData.at(index.row());
				}
			}
			else if (role == Qt::TextAlignmentRole) {

				if (index.column() == 2) {
					return Qt::AlignRight | Qt::AlignVCenter;
				}
				else if (index.column() == 1) {

					return Qt::AlignHCenter | Qt::AlignVCenter;
				}
				else if (index.column() == 0) {

					return Qt::AlignLeft | Qt::AlignVCenter;
				}
			}
			else if (role == RawDataRole) {

				return m_rawData.at(index.row());
			}
		}

		return QVariant();
	}

	QVariant ElasticListModel::headerData(int section, Qt::Orientation orientation, int role) const {

		if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {

			if (section == 0) {

				return tr("Value");
			}
			else if (section == 1) {

				return tr("Count");
			}
			else if (section == 2){
				return tr("%");
			}
		}

		return QVariant();
	}

	QString ElasticListModel::ConvertDoubleToRawString(double value) {

		return QString::number(value, 'f', 15);
	}

	QString ElasticListModel::ConvertQVariantToRawString(const QVariant& value) {

		if (value.type() == QVariant::Type::Double) {

			return ConvertDoubleToRawString(value.toDouble());
		}
		else {

			return value.toString();
		}
	}

} //namespace SynGlyphX