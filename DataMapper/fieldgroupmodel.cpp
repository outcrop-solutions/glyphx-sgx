#include "fieldgroupmodel.h"
#include <QtCore/QDebug>

FieldGroupModel::FieldGroupModel(QObject *parent)
	: QAbstractTableModel(parent),
	m_dataTransformModel(nullptr)
{
	
}

FieldGroupModel::~FieldGroupModel()
{

}

Qt::ItemFlags FieldGroupModel::flags(const QModelIndex& index) const {

	if (index.isValid()) {
		
		Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
		if (index.column() == 0) {

			return flags | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
		}
		else {

			return flags;
		}
	}
	
	return QAbstractTableModel::flags(index);
}

int FieldGroupModel::rowCount(const QModelIndex& parent) const {

	if ((parent.isValid()) || (m_dataTransformModel == nullptr) || m_countOfFieldsPerTable.empty()) {

		return 0;
	}
	else {

		return m_countOfFieldsPerTable.back();
		//return 1;
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

		if (index.column() == 0) {
			
			if (role == Qt::CheckStateRole) {

				if (m_checkedItems.count(GetInputFieldForRow(index.row())) > 0) {

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
		else if (role == Qt::DisplayRole) {

			if (index.column() == 3) {

				return QString::fromStdWString(GetInputFieldForRow(index.row()).GetField());
			}
			else {
				
				//const SynGlyphX::InputTable& datasourceTableInfo = GetTableForRow(index.row());
				//std::advance(table, index.row());
				std::map<boost::uuids::uuid, std::vector<std::wstring>> tb = m_dataTransformModel->GetDataEngineConn().getNumericFieldsTable();
				std::map<boost::uuids::uuid, std::vector<std::wstring>>::iterator iT = tb.begin();
				std::advance(iT, GetTableForRow(index.row()));
				const SynGlyphX::Datasource& datasource = m_dataTransformModel->GetDataMapping()->GetDatasources().GetDatasourceByID(iT->first);
				if (index.column() == 2) {

					if (datasource.CanDatasourceHaveMultipleTables()) {

						return QString::fromStdWString(m_dataTransformModel->GetDataEngineConn().getTableName(iT->first));
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

QVariant FieldGroupModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (orientation == Qt::Horizontal) {

		if ((section == 0) && (role == Qt::ToolTipRole)) {

			return tr("Select/Unselect All");
		}
		else if (role == Qt::DisplayRole) {

			if (section == 1) {

				return tr("Datasource");
			}
			else if (section == 2) {

				return tr("Table");
			}
			else if (section == 3) {

				return tr("Field");
			}
		}
	}

	return QVariant();
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
	int total = 0;
	const SynGlyphX::DatasourceMaps::FileDatasourceMap& fileDatasources = model->GetDataMapping()->GetDatasources().GetFileDatasources();
	SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator iT = fileDatasources.begin();
	for (; iT != fileDatasources.end(); ++iT) {
		total += model->GetDataEngineConn().getTableNumericFields(iT->first).size();
		m_countOfFieldsPerTable.push_back(total);
	}/*
	for (auto& fields : m_dataTransformModel->GetSourceDataManager().GetNumericFieldsByTable()) {

		previousTotal += fields.second.size();
		m_countOfFieldsPerTable.push_back(previousTotal);
	}*/
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
	
	std::map<boost::uuids::uuid, std::vector<std::wstring>> tb = m_dataTransformModel->GetDataEngineConn().getNumericFieldsTable();
	std::map<boost::uuids::uuid, std::vector<std::wstring>>::iterator iT = tb.begin();
	std::advance(iT, i);
	auto field = iT->second.begin();
	std::advance(field, index);

	return SynGlyphX::InputField(iT->first, m_dataTransformModel->GetDataEngineConn().getTableName(iT->first), *field, SynGlyphX::InputField::Type::Real);
}

unsigned int FieldGroupModel::GetTableForRow(int row) const {

	unsigned int i = 0;
	for (; i < m_countOfFieldsPerTable.size(); ++i) {

		if (row < m_countOfFieldsPerTable[i]) {

			break;
		}
	}

	//auto table = m_dataTransformModel->GetSourceDataManager().GetNumericFieldsByTable().begin();
	//auto table = m_dataTransformModel->GetDataEngineConn().getNumericFieldsTable().begin();
	//std::advance(table, i);

	return i;
}

SynGlyphX::AllSomeNone FieldGroupModel::AreFieldsChecked() const {

	if (m_checkedItems.empty()) {

		return SynGlyphX::AllSomeNone::None;
	}
	else if(m_checkedItems.size() == m_countOfFieldsPerTable.back()) {

		return SynGlyphX::AllSomeNone::All;
	}
	else {

		return SynGlyphX::AllSomeNone::Some;
	}
}