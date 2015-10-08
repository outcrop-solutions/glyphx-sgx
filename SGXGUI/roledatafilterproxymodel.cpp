#include "roledatafilterproxymodel.h"

namespace SynGlyphX {

	RoleDataFilterProxyModel::RoleDataFilterProxyModel(QObject* parent) :
		QSortFilterProxyModel(parent),
		m_filterData(-1) {

		setDynamicSortFilter(true);
		setSortRole(Qt::InitialSortOrderRole);
	}

	RoleDataFilterProxyModel::~RoleDataFilterProxyModel() {


	}

	bool RoleDataFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {

		QAbstractItemModel* model = sourceModel();

		if (model != nullptr) {

			QModelIndex sourceIndex = model->index(source_row, 0, source_parent);
			return (model->data(sourceIndex, filterRole()).toInt() == m_filterData);
		}
		else {

			return false;
		}
	}

	void RoleDataFilterProxyModel::SetFilterData(int data) {

		m_filterData = data;
		invalidateFilter();
	}

} //namespace SynGlyphX