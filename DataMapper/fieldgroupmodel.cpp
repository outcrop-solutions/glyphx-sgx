#include "fieldgroupmodel.h"

FieldGroupModel::FieldGroupModel(QObject *parent)
	: QAbstractTableModel(parent),
	m_dataTransformModel(nullptr)
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

	if ((parent.isValid()) || (m_dataTransformModel == nullptr) || m_countOfFieldsPerTable.empty()) {

		return 0;
	}
	else {

		m_countOfFieldsPerTable.back();
	}
}

int FieldGroupModel::columnCount(const QModelIndex& parent) const {

	if ((parent.isValid()) || (m_dataTransformModel == nullptr)) {

		return 0;
	}
	else {

		return 4;
	}
}

QVariant FieldGroupModel::data(const QModelIndex& index, int role) const {

	if (m_dataTransformModel != nullptr) {

		if ((index.column() == 0) && (role == Qt::CheckStateRole)) {

			if (m_checkedItems.count(GetInputFieldForRow(index.row())) > 0) {

				return Qt::Checked;
			}
			else {

				return Qt::Unchecked;
			}
		}
		else if (role == Qt::DisplayRole) {

			if (index.column() == 3) {

				return QString::fromStdWString(GetInputFieldForRow(index.row).GetField());
			}
			else {
				
				const SynGlyphX::InputTable& datasourceTableInfo = GetTableForRow(index.row);
				const SynGlyphX::Datasource& datasource = m_dataTransformModel->GetDataMapping()->GetDatasources().GetDatasourceByID(GetTableForRow(index.row).GetDatasourceID());
				if (index.column() == 1) {

					if (datasource.CanDatasourceHaveMultipleTables()) {

						return QString::fromStdWString(datasourceTableInfo.GetTable());
					}
					else {

						return tr("N/A");
					}
				}
				else {

					return QString::fromStdWString(datasource.GetFormattedName());
				}
			}
		}
	}
	
	return QVariant();
}

bool FieldGroupModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if (m_dataTransformModel != nullptr) {

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

void FieldGroupModel::ResetTable(DataTransformModel* model) {

	beginResetModel();
	m_dataTransformModel = model;
	m_checkedItems.clear();
	m_countOfFieldsPerTable.clear();
	unsigned int previousTotal = 0;
	for (auto& fields : m_dataTransformModel->GetSourceDataManager().GetNumericFieldsByTable()) {

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

	auto table = m_dataTransformModel->GetSourceDataManager().GetNumericFieldsByTable().begin();
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

	auto table = m_dataTransformModel->GetSourceDataManager().GetNumericFieldsByTable().begin();
	std::advance(table, i);

	return table->first;
}

bool FieldGroupModel::AreAllFieldsChecked() const {

	return (m_checkedItems.size() == m_countOfFieldsPerTable.back());
}