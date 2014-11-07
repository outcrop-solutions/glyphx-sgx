#ifndef ROLEDATAFILTERPROXYMODEL_H
#define ROLEDATAFILTERPROXYMODEL_H

#include "sgxgui_global.h"
#include <QtCore/QIdentityProxyModel>

namespace SynGlyphX {

	class SGXGUI_EXPORT RoleDataFilterProxyModel : public QIdentityProxyModel
	{
		Q_OBJECT

	public:
		RoleDataFilterProxyModel(QObject* parent = nullptr);
		~RoleDataFilterProxyModel();

		virtual QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
		virtual QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

		void SetRole(int role);
		void SetFilterData(int data);

	private:
		int m_role;
		int m_filterData;
	};

} //namespace SynGlyphX

#endif // GROUPEDROWSITEMMODEL_H
