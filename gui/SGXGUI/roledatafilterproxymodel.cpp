#include "roledatafilterproxymodel.h"
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
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

		if ((model != nullptr) && HasFilterData()) {

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

	bool  RoleDataFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
	{
		QVariant leftData = sourceModel()->data(left);
		QVariant rightData = sourceModel()->data(right);

		Q_ASSERT(leftData.type() == rightData.type());

		if (leftData.type() == QVariant::String)
		{
			//try to see if String is acually a number or a date
			// ideally we need to sourceModel to have the correct type to avoid costly conversion here
			bool o1, o2;
			double ld = leftData.toDouble(&o1);
			double rd = rightData.toDouble(&o2);
			if (o1 && o2)
			{
				return ld < rd;
			}
			QDateTime l = QDateTime::fromString(leftData.toString(),"d/m/yyyy");
			QDateTime r = QDateTime::fromString(rightData.toString(),"d/m/yyyy");
			if (l.isValid() && r.isValid())
				return l < r;
		}

		return (leftData < rightData);
	}

	void RoleDataFilterProxyModel::Clear() {

		RemoveFilterData();
		invalidateFilter();
	}

	void RoleDataFilterProxyModel::SetNot(bool val) {

		m_not = val;
	}

	StringRoleDataFilterProxyModel::StringRoleDataFilterProxyModel(QObject* parent) :
		RoleDataFilterProxyModel(parent) {


	}

	StringRoleDataFilterProxyModel::~StringRoleDataFilterProxyModel() {

		
	}

	void StringRoleDataFilterProxyModel::SetFilterData(const QString& data) {

		QSet<QString> stringSet;
		stringSet.insert(data);
		SetFilterData(stringSet);
	}

	void StringRoleDataFilterProxyModel::SetFilterData(const QSet<QString>& data) {

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

		QSet<int> intList;
		intList.insert(data);
		SetFilterData(intList);
	}

	void IntRoleDataFilterProxyModel::SetFilterData(const QSet<int>& data) {

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
