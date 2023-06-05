#include "tablechoicemodel.h"
#include <stdexcept>
const quintptr TableChoiceModel::s_rootID = std::numeric_limits<quintptr>::max();

TableChoiceModel::TableChoiceModel(bool includeCheckboxes, QObject *parent)
	: SynGlyphX::AbstractTreeModel(parent),
	m_includeCheckboxes(includeCheckboxes)
{

}

TableChoiceModel::~TableChoiceModel()
{

}

void TableChoiceModel::SetTables(const QStringList& tables, const ForiegnKeyLinkedTableEntries& foriegnKeyTables) {

	//if (foriegnKeyTables.size() != m_tables.size()) {

	//	throw std::invalid_argument("Mismatch between number of tables and number of tables linked by foriegn key");
	//}

	beginResetModel();
	m_tables.clear();
	for (auto& table : tables) {

		m_tables.push_back(TableChoiceModel::TableEntry(table, false));
	}
	m_tableToTablesLinkedByForiegnKeys = foriegnKeyTables;
	endResetModel();
}

QStringList TableChoiceModel::GetChosenTables() const {

	QStringList tables;
	for (auto& table : m_tables) {

		if (table.second) {

			tables << table.first;
		}
	}
	return tables;
}

const TableChoiceModel::TableEntries& TableChoiceModel::GetChosenLinkedTables(unsigned int tableIndex) const {

	if (tableIndex >= m_tables.size()) {

		throw std::invalid_argument("Can't get tables linked by foriegn keys for table index that is greater than the number of tables");
	}

	return m_tableToTablesLinkedByForiegnKeys.at(tableIndex);
}

unsigned int TableChoiceModel::GetMaxDepth() const {
	
	if (m_tableToTablesLinkedByForiegnKeys.empty()) {

		return 1;
	}
	else {

		return 2;
	}
}

Qt::ItemFlags TableChoiceModel::flags(const QModelIndex& index) const {

	if (!index.isValid()) {

		return 0;
	}

	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	if (index.internalId() != s_rootID) {

		flags |= Qt::ItemNeverHasChildren;
	}
	if (index.column() == 0) {

		return flags | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
	}
	else {

		return flags;
	}
}

int TableChoiceModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		return (int)m_tables.size();
	}
	else if ((!m_tableToTablesLinkedByForiegnKeys.empty()) && (parent.column() == 0)) {

		return (int)m_tableToTablesLinkedByForiegnKeys.at(parent.row()).size();
	}
	else {

		return 0;
	}
}

int TableChoiceModel::columnCount(const QModelIndex& parent) const {

	if (m_includeCheckboxes) {

		return 2;
	}
	else {

		return 1;
	}
}

const TableChoiceModel::TableEntry& TableChoiceModel::GetTableEntry(const QModelIndex& index) const {

	const QModelIndex& parentIndex = index.parent();
	if (parentIndex.isValid()) {

		return m_tableToTablesLinkedByForiegnKeys.at(parentIndex.row()).at(index.row());
	}
	else {

		return m_tables.at(index.row());
	}
}

QVariant TableChoiceModel::data(const QModelIndex& index, int role) const {

	if (index.isValid()) {

		if ((m_includeCheckboxes) && (index.column() == 0)) {

			if (role == Qt::CheckStateRole) {

				if (GetTableEntry(index).second) {

					return Qt::Checked;
				}
				else {

					return Qt::Unchecked;
				}
			}
			else if (role == Qt::ToolTipRole) {

				return tr("Select/Unselect");
			}
		}
		else if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {

			return GetTableEntry(index).first;
		}
	}

	return QVariant();
}

bool TableChoiceModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if (index.isValid() && (index.column() == 0) && (role == Qt::CheckStateRole) && m_includeCheckboxes) {

		bool checkState = (value.toInt() == Qt::Checked);

		const QModelIndex& parentIndex = index.parent();
		if (parentIndex.isValid()) {

			m_tableToTablesLinkedByForiegnKeys.at(parentIndex.row()).at(index.row()).second = checkState;
		}
		else {

			m_tables.at(index.row()).second = checkState;
		}

		emit dataChanged(index, index, QVector<int>(1, Qt::CheckStateRole));
		return true;
	}

	return false;
}

QModelIndex TableChoiceModel::index(int row, int column, const QModelIndex& parent) const {

	if (hasIndex(row, column, parent)) {

		if (parent.isValid()) {

			return createIndex(row, column, parent.row());
		}
		else {

			return createIndex(row, column, s_rootID);
		}
	}

	return QModelIndex();
}

QModelIndex TableChoiceModel::parent(const QModelIndex& index) const {

	if (!index.isValid()) {

		return QModelIndex();
	}

	if (index.internalId() == s_rootID) {

		return QModelIndex();
	}
	else {

		return createIndex(index.internalId(), 0, s_rootID);
	}
}

QVariant TableChoiceModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (orientation == Qt::Horizontal) {

		if ((section == 0) && (role == Qt::ToolTipRole)) {

			return tr("Select/Unselect All");
		}
		else if (role == Qt::DisplayRole) {

			if (section == 1) {

				return tr("Table");
			}
		}
	}

	return QVariant();
}