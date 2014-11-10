#ifndef ROLEDATAFILTERPROXYMODEL_H
#define ROLEDATAFILTERPROXYMODEL_H

#include "sgxgui_global.h"
#include <QtCore/QSortFilterProxyModel>

namespace SynGlyphX {

	class SGXGUI_EXPORT RoleDataFilterProxyModel : public QSortFilterProxyModel
	{
		Q_OBJECT

	public:
		RoleDataFilterProxyModel(QObject* parent = nullptr);
		~RoleDataFilterProxyModel();

		void SetFilterData(int data);

	protected:
		virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

	private:
		int m_filterData;
	};

} //namespace SynGlyphX

#endif // GROUPEDROWSITEMMODEL_H
