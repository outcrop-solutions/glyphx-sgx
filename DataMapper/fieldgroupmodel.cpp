#include "fieldgroupmodel.h"

FieldGroupModel::FieldGroupModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	//There is no header data for section 0 since that is where the select all/select none checkbox is
	setHeaderData(1, Qt::Horizontal, tr("Datasource"));
	setHeaderData(2, Qt::Horizontal, tr("Table"));
	setHeaderData(3, Qt::Horizontal, tr("Field"));
}

FieldGroupModel::~FieldGroupModel()
{

}

Qt::ItemFlags FieldGroupModel::flags(const QModelIndex& index) const {

	if (index.isValid()) {

		Qt::ItemFlags flags = Qt::ItemIsEnabled || Qt::ItemIsSelectable || Qt::ItemNeverHasChildren;
		if (index.column() == 0) {

			return flags || Qt::ItemIsUserCheckable;
		} 
		else {

			return flags;
		}
	}
	else {

		return QAbstractTableModel::flags(index);
	}
}

int FieldGroupModel::rowCount(const QModelIndex& parent) const {

	if (parent.isValid()) {

		return 0;
	}
	else {

		
	}
}

int FieldGroupModel::columnCount(const QModelIndex& parent) const {

	if (parent.isValid()) {

		return 0;
	}
	else {

		return 4;
	}
}

QVariant FieldGroupModel::data(const QModelIndex& index, int role) const {

	if ((index.column() == 0) && (role == Qt::CheckStateRole)) {

		if (m_checkedItems.count(GetInputFieldForRow(index.row())) > 0) {

			return Qt::Checked;
		}
		else {

			return Qt::Unchecked;
		}
	}
	else if ((index.column() > 0) && (role == Qt::DisplayRole)) {


	}
	
	return QVariant();
}

bool FieldGroupModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if ((index.column() == 0) && (role == Qt::CheckStateRole)) {

		int checkState = value.toInt();
		if (checkState == Qt::Checked) {

			m_checkedItems.insert(GetInputFieldForRow(index.row()));
		}
		else {

			m_checkedItems.erase(GetInputFieldForRow(index.row()));
		}
		emit dataChanged(index, index, QVector<int>(1, Qt::CheckStateRole));
		return true;
	}

	return false;
}

void FieldGroupModel::SetCheckedItems(const SynGlyphX::FieldGroup& fieldGroup) {

	m_checkedItems = fieldGroup;
	emit dataChanged(index(0, 0), index(rowCount() - 1, 0), QVector<int>(1, Qt::CheckStateRole));
}

void FieldGroupModel::UncheckAllItems() {

	m_checkedItems.clear();
	emit dataChanged(index(0, 0), index(rowCount() - 1, 0), QVector<int>(1, Qt::CheckStateRole));
}

void FieldGroupModel::ResetTable(const SynGlyphX::SourceDataManager::NumericFieldsByTable& numericFields) {

	beginResetModel();
	m_numericFields = numericFields;
	m_checkedItems.clear();
	m_countOfFieldsPerTable.clear();
	unsigned int previousTotal = 0;
	for (auto& fields : m_numericFields) {

		previousTotal += fields.second.size();
		m_countOfFieldsPerTable.push_back(previousTotal);
	}
	endResetModel();
}

const SynGlyphX::FieldGroup& FieldGroupModel::GetCheckedItems() const {

	return m_checkedItems;
}

SynGlyphX::InputField FieldGroupModel::GetInputFieldForRow(int row) const {

	unsigned int i = 0;
	for (; i < m_countOfFieldsPerTable.size(); ++i) {

		if (row < m_countOfFieldsPerTable[i]) {

			break;
		}
	}

	unsigned int index = 0;
	if (i == 0) {

		index = row;
	}
	else {

		index = row - m_countOfFieldsPerTable[i - 1];
	}

	auto table = m_numericFields.begin();
	std::advance(table, i);
	auto field = table->second.begin();
	std::advance(field, index);

	return SynGlyphX::InputField(table->first.GetDatasourceID(), table->first.GetTable(), *field, SynGlyphX::InputField::Type::Real);
}

const SynGlyphX::InputTable& FieldGroupModel::GetTableForRow(int row) const {

	unsigned int i = 0;
	for (; i < m_countOfFieldsPerTable.size(); ++i) {

		if (row < m_countOfFieldsPerTable[i]) {

			break;
		}
	}

	auto table = m_numericFields.begin();
	std::advance(table, i);

	return table->first;
}