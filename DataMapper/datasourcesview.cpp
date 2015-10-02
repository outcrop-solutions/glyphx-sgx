#include "datasourcesview.h"
#include "roledatafilterproxymodel.h"

DataSourcesView::DataSourcesView(DataTransformModel* sourceModel, QWidget *parent)
	: SynGlyphX::TreeView(parent),
	m_sourceModel(sourceModel)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	SynGlyphX::RoleDataFilterProxyModel* filterModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	filterModel->setFilterRole(DataTransformModel::DataTypeRole);
	filterModel->setSourceModel(m_sourceModel);
	filterModel->SetFilterData(DataTransformModel::DataType::DataSources);
	setModel(filterModel);

	setDragEnabled(false);
	setAcceptDrops(false);
	SetScrollOnSelection(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setHeaderHidden(true);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_sharedActions.AddSeparator();

	QAction* propertiesAction = m_sharedActions.AddAction(tr("Properties"));
	QObject::connect(propertiesAction, &QAction::triggered, this, &DataSourcesView::OnShowProperties);

	m_sharedActions.EnableActions(false);

	addActions(m_sharedActions);
}

DataSourcesView::~DataSourcesView()
{

}

const SynGlyphX::SharedActionList& DataSourcesView::GetSharedActions() {

	return m_sharedActions;
}

void DataSourcesView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	m_sharedActions.EnableActions(!selected.isEmpty());

	SynGlyphX::TreeView::selectionChanged(selected, deselected);
}

void DataSourcesView::OnShowProperties() {


}