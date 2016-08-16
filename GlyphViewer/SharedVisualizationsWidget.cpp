#include "SharedVisualizationsWidget.h"
#include "TitleTreeWidget.h"
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLayout>
#include "LoadingFilterWidget.h"
#include <QtCore/QDir>
#include "glyphbuilderapplication.h"
#include "datatransformmapping.h"
#include "stringconvert.h"
#include "glyphengine.h"

void SharedVisualizationsFile::ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) {

	m_filenameToTitleMap.clear();
	m_groupedVisualizations.erase();
	m_groupedVisualizations.insert(QStringList());

	const boost::property_tree::wptree& sharedVizPropertyTree = filePropertyTree.get_child(L"SharedVisualizations");
	ImportFromPropertyTree(sharedVizPropertyTree, m_groupedVisualizations.root());
}

void SharedVisualizationsFile::ImportFromPropertyTree(const boost::property_tree::wptree& propertyTree, stlplus::ntree<QStringList>::iterator parent) {

	for (const boost::property_tree::wptree::value_type& propertyTreeEntry : propertyTree) {

		QStringList titleAndFile;
		//titleAndFile.push_back(QString::fromStdWString(propertyTreeEntry.second.get<std::wstring>(L"<xmlattr>.name")));
		if (propertyTreeEntry.first == L"Visualization") {

			titleAndFile.push_back(QString::fromStdWString(propertyTreeEntry.second.get<std::wstring>(L"<xmlattr>.name")));
			titleAndFile.push_back(QString::fromStdWString(propertyTreeEntry.second.get<std::wstring>(L"")));
			m_groupedVisualizations.append(parent, titleAndFile);
			m_filenameToTitleMap.insert(titleAndFile[1], titleAndFile[0]);
		}
		else if (propertyTreeEntry.first == L"Group") {

			titleAndFile.push_back(QString::fromStdWString(propertyTreeEntry.second.get<std::wstring>(L"<xmlattr>.name")));
			stlplus::ntree<QStringList>::iterator child = m_groupedVisualizations.append(parent, titleAndFile);
			ImportFromPropertyTree(propertyTreeEntry.second.get_child(L""), child);
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

	m_viewListWidget = new SynGlyphX::TitleTreeWidget(this);
	m_viewListWidget->SetAllowMultiselect(false);
	m_viewListWidget->SetTitle(tr("View(s)"));
	m_viewListWidget->layout()->setContentsMargins(0, 0, 0, 0);

	addWidget(m_viewListWidget);

	QObject::connect(m_viewListWidget, &SynGlyphX::TitleTreeWidget::SelectedTooltipChanged, this, &SharedVisualizationsWidget::OnFileSelected);
}

SharedVisualizationsWidget::~SharedVisualizationsWidget()
{

}

void SharedVisualizationsWidget::Reset(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection) {

	m_viewListWidget->blockSignals(true);

	m_loadingFilterWidgetMap.clear();
	m_filenameToTitleMap.clear();
	if (m_loadingFilterWidgetsStack != nullptr) {

		delete m_loadingFilterWidgetsStack;
	}
	m_loadingFilterWidgetsStack = new QStackedWidget(this);
	addWidget(m_loadingFilterWidgetsStack);

	bool visualizationsAdded = false;
	QString sharedVizListing = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/sharedvisualizations.xml");
	if (QFile::exists(sharedVizListing)) {

		SharedVisualizationsFile sharedVisualizationsFile;
		sharedVisualizationsFile.ReadFromFile(sharedVizListing.toStdString());
		m_filenameToTitleMap = sharedVisualizationsFile.GetFilenameToTitleMap();

		for (auto iT = m_filenameToTitleMap.begin(); iT != m_filenameToTitleMap.end(); ++iT) {
			
			visualizationsAdded = true;
			std::string filename = iT.key().toStdString();

			SynGlyphX::DataTransformMapping mapping;
			mapping.ReadFromFile(filename);

			if (!mapping.GetFrontEndFilters().empty()) {

				DataEngine::GlyphEngine ge;
				ge.initiate(dataEngineConnection->getEnv(), filename, "", "", "", "GlyphViewer");

				LoadingFilterWidget* loadingFilterWidget = new LoadingFilterWidget(this);
				loadingFilterWidget->SetFilters(ge, mapping);
				loadingFilterWidget->layout()->setContentsMargins(0, 0, 0, 0);
				m_loadingFilterWidgetsStack->addWidget(loadingFilterWidget);
				m_loadingFilterWidgetMap[filename] = loadingFilterWidget;
			}
		}

		m_viewListWidget->SetItems(sharedVisualizationsFile.GetInfoTree());
	}

	m_viewListWidget->blockSignals(false);

	if (visualizationsAdded) {

		m_viewListWidget->SelectFirstSelectableItem();
	}
	else {

		m_loadingFilterWidgetsStack->setVisible(false);
	}
}

bool SharedVisualizationsWidget::DoCurrentNecessaryFiltersHaveSelection() const {

	if (CanCurrentHaveFilters()) {

		return m_loadingFilterWidgetMap.at(m_currentFilename)->AreSelectionsValid();
	}

	return true;
}

QString SharedVisualizationsWidget::GetCurrentFilename() const {

	return QString::fromStdString(m_currentFilename);
}

bool SharedVisualizationsWidget::CanCurrentHaveFilters() const {

	return (m_loadingFilterWidgetMap.count(m_currentFilename) != 0);
}

MultiTableDistinctValueFilteringParameters SharedVisualizationsWidget::GetCurrentFilterValues() const {

	if (m_loadingFilterWidgetMap.count(m_currentFilename) != 0) {

		return m_loadingFilterWidgetMap.at(m_currentFilename)->GetFilterValues();
	}
	else {

		return MultiTableDistinctValueFilteringParameters();
	}
}

void SharedVisualizationsWidget::OnFileSelected(QString filename) {

	m_currentFilename = filename.toStdString();
	if (m_loadingFilterWidgetMap.count(m_currentFilename) != 0) {

		m_loadingFilterWidgetsStack->setCurrentWidget(m_loadingFilterWidgetMap.at(m_currentFilename));
		m_loadingFilterWidgetsStack->setVisible(true);
	}
	else {

		m_loadingFilterWidgetsStack->setVisible(false);
	}
}