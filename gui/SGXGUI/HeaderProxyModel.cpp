#include "HeaderProxyModel.h"

namespace SynGlyphX {

	HeaderProxyModel::HeaderProxyModel(QObject *parent)
		: QIdentityProxyModel(parent)
	{

	}

	HeaderProxyModel::~HeaderProxyModel()
	{

	}

	void HeaderProxyModel::SetHorizontalHeaderMap(const std::unordered_map<std::wstring, std::wstring>& horizontalHeaderMap) {

		SetHorizontalHeaderMap(ConvertToQHash(horizontalHeaderMap));
	}

	void HeaderProxyModel::SetHorizontalHeaderMap(const QHash<QString, QString>& horizontalHeaderMap) {

		m_horizontalHeaderMap = horizontalHeaderMap;
		QAbstractItemModel* source = sourceModel();
		if (source != nullptr) {

			unsigned int numberOfColumns = source->columnCount();
			if (numberOfColumns > 0) {

				emit headerDataChanged(Qt::Horizontal, 0, numberOfColumns - 1);
			}
		}
	}

	QVariant HeaderProxyModel::headerData(int section, Qt::Orientation orientation, int role) const {

		QAbstractItemModel* source = sourceModel();
		if (source == nullptr) {

			return QVariant();
		}

		QVariant sourceHeaderData = source->headerData(section, orientation, role);
		if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {

			QString sourceHeaderString = sourceHeaderData.toString();
			if (m_horizontalHeaderMap.contains(sourceHeaderString)) {

				return m_horizontalHeaderMap[sourceHeaderString];
			}
		}

		return sourceHeaderData;
	}

	QHash<QString, QString> HeaderProxyModel::ConvertToQHash(const std::unordered_map<std::wstring, std::wstring>& map) {

		QHash<QString, QString> hash;

		for (const auto& pair : map) {

			hash.insert(QString::fromStdWString(pair.first), QString::fromStdWString(pair.second));
		}

		return hash;
	}

} //namespace SynGlyphX