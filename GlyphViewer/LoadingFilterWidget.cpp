#include "LoadingFilterWidget.h"
#include "TitleListWidget.h"
#include <QtWidgets/QLayout>
#include "FrontEndFilter.h"
#include "glyphengine.h"
#include <QtWidgets/QSplitter>

LoadingFilterWidget::LoadingFilterWidget(QWidget *parent)
	: QWidget(parent)
{
	
}

LoadingFilterWidget::~LoadingFilterWidget()
{

}

void LoadingFilterWidget::SetFilters(DataEngine::GlyphEngine& glyphEngine, const SynGlyphX::MultiTableFrontEndFilters& filters) {

	for (const auto& filtersForTable : filters) {

		QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

		splitter->setChildrenCollapsible(false);
		splitter->setHandleWidth(2);
		splitter->setStyleSheet("QSplitter::handle:horizontal { background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #eee, stop:1 #ccc);"
			"border: 1px solid #777; width: 0px; margin - top: 0px; margin - bottom: 0px; border - radius: 2px; }");

		
	}
}

void LoadingFilterWidget::AddFilter(const QString& name, const SynGlyphX::FrontEndFilterOptions& options, const QStringList& filterValues) {

	SynGlyphX::TitleListWidget* filterWidget = new SynGlyphX::TitleListWidget(this);
	filterWidget->layout()->setContentsMargins(0, 0, 0, 0);
	filterWidget->SetAllowMultiselect(options.IsMultiselectAllowed());
	filterWidget->ShowSelectAllButton(true);
	filterWidget->SetTitle(name);
	filterWidget->SetItems(filterValues);

	addWidget(filterWidget);
	m_filterListWidgets.append(filterWidget);
	m_isRequired.push_back(options.IsRequired());
}

bool LoadingFilterWidget::AreSelectionsValid() const {

	for (unsigned int i = 0; i < m_filterListWidgets.size(); ++i) {

		if (m_isRequired[i] && !m_filterListWidgets[i]->AreAnyItemsSelected()) {

			return false;
		}
	}

	return true;
}

QString LoadingFilterWidget::GenerateQuery() const {


}