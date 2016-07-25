#include "LoadingFilterWidget.h"
#include "TitleListWidget.h"
#include <QtWidgets/QLayout>

LoadingFilterWidget::LoadingFilterWidget(QWidget *parent)
	: QSplitter(Qt::Horizontal, parent)
{
	setChildrenCollapsible(false);
	setHandleWidth(2);
	setStyleSheet("QSplitter::handle:horizontal { background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #eee, stop:1 #ccc);"
				  "border: 1px solid #777; width: 0px; margin - top: 0px; margin - bottom: 0px; border - radius: 2px; }");
}

LoadingFilterWidget::~LoadingFilterWidget()
{

}

void LoadingFilterWidget::AddFilter(const QString& name, bool allowMultiselect, const QStringList& filterValues) {

	SynGlyphX::TitleListWidget* filterWidget = new SynGlyphX::TitleListWidget(this);
	filterWidget->layout()->setContentsMargins(0, 0, 0, 0);
	filterWidget->SetAllowMultiselect(allowMultiselect);
	filterWidget->ShowSelectAllButton(true);
	filterWidget->SetTitle(name);
	filterWidget->SetItems(filterValues);

	addWidget(filterWidget);
	m_filterListWidgets.append(filterWidget);
}

QSet<QString> LoadingFilterWidget::GetFilterData(unsigned int index) const {

	return m_filterListWidgets[index]->GetSelectedLabels().toSet();
}

bool LoadingFilterWidget::AreAllValuesSelected(unsigned int index) const {

	return m_filterListWidgets[index]->AreAllItemsSelected();
}

bool LoadingFilterWidget::AreAnyValuesSelected(unsigned int index) const {

	return m_filterListWidgets[index]->AreAnyItemsSelected();
}