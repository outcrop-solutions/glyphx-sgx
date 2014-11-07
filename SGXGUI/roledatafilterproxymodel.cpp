#include "roledatafilterproxymodel.h"

namespace SynGlyphX {

	RoleDataFilterProxyModel::RoleDataFilterProxyModel(QObject* parent) :
		QIdentityProxyModel(parent),
		m_role(-1),
		m_filterData(-1) {


	}

	RoleDataFilterProxyModel::~RoleDataFilterProxyModel() {


	}

	QModelIndex RoleDataFilterProxyModel::mapFromSource(const QModelIndex& sourceIndex) const {

		if ((sourceModel() != nullptr) && (sourceIndex.isValid())) {

			if (sourceIndex.data(m_role) == m_filterData) {

				return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
			}
		}
		
		return QModelIndex();
	}

	QModelIndex RoleDataFilterProxyModel::mapToSource(const QModelIndex& proxyIndex) const {

		return QIdentityProxyModel::mapToSource(proxyIndex);
	}

	void RoleDataFilterProxyModel::SetRole(int role) {

		m_role = role;
	}

	void RoleDataFilterProxyModel::SetFilterData(int data) {

		m_filterData = data;
	}

} //namespace SynGlyphX