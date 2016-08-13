#include "SharedViewsWidget.h"
#include "TitleListWidget.h"
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLayout>
#include "LoadingFilterWidget.h"
#include <QtCore/QDir>
#include "glyphbuilderapplication.h"
#include "datatransformmapping.h"
#include "stringconvert.h"
#include "glyphengine.h"

void SharedVisualizationsFile::ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) {

	m_visualizations.clear();

	const boost::property_tree::wptree& sharedVizPropertyTree = filePropertyTree.get_child(L"SharedVisualizations");
	for (const boost::property_tree::wptree::value_type& vizPropertyTree : sharedVizPropertyTree) {

		if (vizPropertyTree.first == L"Visualization") {

			std::pair<std::wstring, std::wstring> visualization(vizPropertyTree.second.get<std::wstring>(L"<xmlattr>.name"),
				vizPropertyTree.second.get<std::wstring>(L""));
			m_visualizations.push_back(visualization);
		}
	}
}

void SharedVisualizationsFile::ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const {


}

SharedVisualizationsWidget::SharedVisualizationsWidget(QWidget *parent)
	: QSplitter(Qt::Horizontal, parent),
	m_loadingFilterWidgetsStack(nullptr)
{
	setChildrenCollapsible(false);
	setHandleWidth(2);
	setStyleSheet("QSplitter::handle:horizontal { background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #eee, stop:1 #ccc);"
		"border: 1px solid #777; width: 0px; margin - top: 0px; margin - bottom: 0px; border - radius: 2px; }");

	m_viewListWidget = new SynGlyphX::TitleListWidget(this);
	m_viewListWidget->SetAllowMultiselect(false);
	m_viewListWidget->ShowSelectAllButton(false);
	m_viewListWidget->SetTitle(tr("View(s)"));
	m_viewListWidget->layout()->setContentsMargins(0, 0, 0, 0);

	addWidget(m_viewListWidget);

	QObject::connect(m_viewListWidget, &SynGlyphX::TitleListWidget::CurrentRowChanged, this, &SharedVisualizationsWidget::OnFileSelected);
}

SharedVisualizationsWidget::~SharedVisualizationsWidget()
{

}

void SharedVisualizationsWidget::Reset(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection) {

	m_viewListWidget->blockSignals(true);

	m_loadingFilterWidgetMap.clear();
	m_sharedVisualizationsInfo.clear();
	if (m_loadingFilterWidgetsStack != nullptr) {

		delete m_loadingFilterWidgetsStack;
	}
	m_loadingFilterWidgetsStack = new QStackedWidget(this);
	addWidget(m_loadingFilterWidgetsStack);

	QStringList titles;
	QStringList files;
	bool visualizationsAdded = false;
	QString sharedVizListing = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/sharedvisualizations.xml");
	if (QFile::exists(sharedVizListing)) {

		SharedVisualizationsFile sharedVisualizationsFile;
		sharedVisualizationsFile.ReadFromFile(sharedVizListing.toStdString());
		m_sharedVisualizationsInfo = sharedVisualizationsFile.Get();

		for (unsigned int i = 0; i < m_sharedVisualizationsInfo.size(); ++i) {
			
			visualizationsAdded = true;
			titles.push_back(QString::fromStdWString(m_sharedVisualizationsInfo[i].first));
			files.push_back(QString::fromStdWString(m_sharedVisualizationsInfo[i].second));

			SynGlyphX::DataTransformMapping mapping;
			mapping.ReadFromFile(SynGlyphX::StringConvert::ToStdString(m_sharedVisualizationsInfo[i].second));

			DataEngine::GlyphEngine ge;
			ge.initiate(dataEngineConnection->getEnv(), SynGlyphX::StringConvert::ToStdString(m_sharedVisualizationsInfo[i].second), "", "", "", "GlyphViewer");

			if (!mapping.GetFrontEndFilters().empty()) {

				LoadingFilterWidget* loadingFilterWidget = new LoadingFilterWidget(this);
				loadingFilterWidget->SetFilters(ge, mapping.GetFrontEndFilters());
				loadingFilterWidget->layout()->setContentsMargins(0, 0, 0, 0);
				m_loadingFilterWidgetsStack->addWidget(loadingFilterWidget);
				m_loadingFilterWidgetMap[i] = loadingFilterWidget;
			}
		}
	}

	m_viewListWidget->SetItems(titles, files);

	m_viewListWidget->blockSignals(false);

	if (visualizationsAdded) {

		m_viewListWidget->SelectItem(0);
		OnFileSelected(0);
	}
	else {

		m_loadingFilterWidgetsStack->setVisible(false);
	}
}

bool SharedVisualizationsWidget::DoCurrentNecessaryFiltersHaveSelection() const {

	if (CanCurrentHaveFilters()) {

		unsigned int current = m_loadingFilterWidgetsStack->currentIndex();
		return m_loadingFilterWidgetMap.at(current)->AreSelectionsValid();
	}

	return true;
}

QString SharedVisualizationsWidget::GetCurrentFilename() const {

	return m_viewListWidget->GetSelectedTooltips().first();
}

bool SharedVisualizationsWidget::CanCurrentHaveFilters() const {

	return (m_loadingFilterWidgetMap.count(m_loadingFilterWidgetsStack->currentIndex()) != 0);
}

MultiTableDistinctValueFilteringParameters SharedVisualizationsWidget::GetCurrentFilterValues() const {

	return m_loadingFilterWidgetMap.at(m_loadingFilterWidgetsStack->currentIndex())->GetFilterValues();
}

void SharedVisualizationsWidget::OnFileSelected(int row) {

	if (m_loadingFilterWidgetMap.count(row) != 0) {

		m_loadingFilterWidgetsStack->setCurrentWidget(m_loadingFilterWidgetMap.at(row));
		m_loadingFilterWidgetsStack->setVisible(true);
	}
	else {

		m_loadingFilterWidgetsStack->setVisible(false);
	}
}