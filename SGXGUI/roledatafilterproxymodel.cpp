#include "roledatafilterproxymodel.h"

namespace SynGlyphX {

	RoleDataFilterProxyModel::RoleDataFilterProxyModel(QObject* parent) :
		QSortFilterProxyModel(parent),
		m_not(false) {

		setDynamicSortFilter(true);
		setSortRole(Qt::InitialSortOrderRole);
	}

	RoleDataFilterProxyModel::~RoleDataFilterProxyModel() {


	}

	bool RoleDataFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {

		QAbstractItemModel* model = sourceModel();

		if ((model != nullptr) || HasFilterData()) {

			QModelIndex sourceIndex = model->index(source_row, 0, source_parent);
			bool match = HasMatch(model->data(sourceIndex, filterRole()));

			if (m_not) {

				return (!match);
			}
			else {

				return match;
			}
		}
		else {

			return true;
		}
	}

	void RoleDataFilterProxyModel::Clear() {

		RemoveFilterData();
		invalidateFilter();
	}

	void RoleDataFilterProxyModel::SetNot(bool not) {

		m_not = not;
	}

	StringRoleDataFilterProxyModel::StringRoleDataFilterProxyModel(QObject* parent) :
		RoleDataFilterProxyModel(parent) {


	}

	StringRoleDataFilterProxyModel::~StringRoleDataFilterProxyModel() {

		
	}

	void StringRoleDataFilterProxyModel::SetFilterData(const QString& data) {

		SetFilterData(QStringList(data));
	}

	void StringRoleDataFilterProxyModel::SetFilterData(const QStringList& data) {

		RemoveFilterData();

		m_stringFilterData = data;
		invalidateFilter();
	}

	bool StringRoleDataFilterProxyModel::HasMatch(const QVariant& var) const {

		return (m_stringFilterData.contains(var.toString()));
	}

	void StringRoleDataFilterProxyModel::RemoveFilterData() {

		m_stringFilterData.clear();
	}

	bool StringRoleDataFilterProxyModel::HasFilterData() const {

		return (!m_stringFilterData.empty());
	}

	IntRoleDataFilterProxyModel::IntRoleDataFilterProxyModel(QObject* parent) :
		RoleDataFilterProxyModel(parent) {


	}

	IntRoleDataFilterProxyModel::~IntRoleDataFilterProxyModel() {


	}

	void IntRoleDataFilterProxyModel::SetFilterData(int data) {

		QList<int> intList;
		intList.push_back(data);
		SetFilterData(intList);
	}

	void IntRoleDataFilterProxyModel::SetFilterData(const QList<int>& data) {

		RemoveFilterData();

		m_intFilterData = data;
		invalidateFilter();
	}

	bool IntRoleDataFilterProxyModel::HasMatch(const QVariant& var) const {

		return (m_intFilterData.contains(var.toInt()));
	}

	void IntRoleDataFilterProxyModel::RemoveFilterData() {

		m_intFilterData.clear();
	}

	bool IntRoleDataFilterProxyModel::HasFilterData() const {

		return (!m_intFilterData.empty());
	}

} //namespace SynGlyphX