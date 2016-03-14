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

		if (model != nullptr) {

			bool match = false;
			QModelIndex sourceIndex = model->index(source_row, 0, source_parent);
			if (!m_intFilterData.empty()) {

				match = (m_intFilterData.contains(model->data(sourceIndex, filterRole()).toInt()));
			}
			else {

				match = (m_stringFilterData.contains(model->data(sourceIndex, filterRole()).toString()));
			}

			if (m_not) {

				return (!match);
			}
			else {

				return match;
			}
		}
		else {

			return false;
		}
	}

	void RoleDataFilterProxyModel::SetFilterData(int data) {

		m_stringFilterData.clear();

		m_intFilterData.clear();
		m_intFilterData.push_back(data);
		invalidateFilter();
	}

	void RoleDataFilterProxyModel::SetFilterData(QList<int> data) {

		m_stringFilterData.clear();

		m_intFilterData = data;
		invalidateFilter();
	}

	void RoleDataFilterProxyModel::SetFilterData(QString data) {

		m_intFilterData.clear();

		m_stringFilterData = QStringList(data);
		invalidateFilter();
	}
	void RoleDataFilterProxyModel::SetFilterData(QStringList data) {

		m_intFilterData.clear();

		m_stringFilterData = data;
		invalidateFilter();
	}

	void RoleDataFilterProxyModel::SetNot(bool not) {

		m_not = not;
	}

} //namespace SynGlyphX