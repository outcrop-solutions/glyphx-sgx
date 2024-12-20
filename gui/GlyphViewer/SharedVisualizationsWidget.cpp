#include "SharedVisualizationsWidget.h"
#include "TitleTreeWidget.h"
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLayout>
#include "LoadingFilterWidget.h"
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include "glyphbuilderapplication.h"
#include "datatransformmapping.h"
#include "stringconvert.h"
#include <QtWidgets/QProgressBar>
//#include "glyphengine.h"
#include <cassert>

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
		if (propertyTreeEntry.first == L"Visualization") {

			titleAndFile.push_back(QString::fromStdWString(propertyTreeEntry.second.get<std::wstring>(L"<xmlattr>.name")));
			titleAndFile.push_back(QString::fromStdWString(propertyTreeEntry.second.get<std::wstring>(L"")));
			// standardize path separators
			titleAndFile[1].replace("\\", "/");
			m_baseDir.replace("\\", "/");
			// change titleAndFile[1] to absolute path
			if (titleAndFile[1].startsWith(m_baseDir))
			{
				// If the base and SDT paths have a common base, we're good...
				QFileInfo finfo(QDir(m_baseDir), titleAndFile[1]);
				titleAndFile[1] = finfo.absoluteFilePath();
			}
			else
			{
				// Otherwise, handle SDT paths being hardcoded for Windows (ugh)
				
				// get common base dir (take us up past the user and customer folders), i.e.
				// the SynGlyphX app data folder on this machine
				auto basedir = QDir(m_baseDir);
				basedir.cdUp(); basedir.cdUp();
				auto basedirpath = basedir.absolutePath();
				assert(basedirpath.endsWith("SynGlyphX"));

				// get the part of the filename that's INSIDE the SynGlyphX folder (the part
				// after "SynGlyphX")
				auto split = titleAndFile[1].split("/SynGlyphX/");
				assert(split.size() >= 2);
				auto combined_dir = basedir.absolutePath() + QString("/") + split[1];
				titleAndFile[1] = combined_dir;
			}
			m_filenameToTitleMap.insert(titleAndFile[1], titleAndFile[0]);
			m_groupedVisualizations.append(parent, titleAndFile);
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
	m_loadingFilterWidgetsStack(nullptr),
	IsGlyphEd(false)
{
	setChildrenCollapsible(false);
	setHandleWidth(1);
	setStyleSheet("QSplitter::handle:horizontal { background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #eee, stop:1 #ccc);"
		"border: 1px solid #777; width: 0px; margin-top: 0px; margin-bottom: 0px; border-radius: 2px; }");

	m_viewListWidget = new SynGlyphX::TitleTreeWidget(this);
	m_viewListWidget->setStyleSheet("background-color: #eff2f7;");
	m_viewListWidget->SetAllowMultiselect(false);
	m_viewListWidget->SetTitle(tr("View(s)"));
	m_viewListWidget->layout()->setContentsMargins(0, 0, 0, 0);

	addWidget(m_viewListWidget);

	QObject::connect(m_viewListWidget, &SynGlyphX::TitleTreeWidget::SelectedTooltipChanged, this, &SharedVisualizationsWidget::OnFileSelected);
}

SharedVisualizationsWidget::~SharedVisualizationsWidget()
{

}

/*void SharedVisualizationsWidget::Setup(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QProgressBar* prog){

	IsGlyphEd = true;
	progress = prog;
	Reset(dataEngineConnection);
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

	//QString sharedVizListing = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation()) + "/sharedvisualizations.xml");
	QString sharedVizListing = QDir::toNativeSeparators(QDir::cleanPath(dataEngineConnection->UserAccessControls()->GlyphEdPath()) + "/sharedvisualizations.xml");

	if (QFile::exists(sharedVizListing)) {

		SharedVisualizationsFile sharedVisualizationsFile(QFileInfo(sharedVizListing).absoluteDir().absolutePath());
		sharedVisualizationsFile.ReadFromFile(sharedVizListing.toStdString());
		m_filenameToTitleMap = sharedVisualizationsFile.GetFilenameToTitleMap();

		DataEngine::GlyphEngine ge;

		for (auto iT = m_filenameToTitleMap.begin(); iT != m_filenameToTitleMap.end(); ++iT) {
			
			visualizationsAdded = true;
 
			//QFileInfo finfo(QFileInfo(sharedVizListing).absoluteDir(), iT.key());

			//std::string filename = finfo.absoluteFilePath().toStdString();

			std::string filename = iT.key().toStdString();

			SynGlyphX::DataTransformMapping mapping;
			mapping.ReadFromFile(filename);

			if (!mapping.GetFrontEndFilters().empty()) {

				//ge.initiate(dataEngineConnection->getEnv(), filename, "", "", "", "GlyphViewer");
				ge.AddVisualization(dataEngineConnection->getEnv(), filename);

				LoadingFilterWidget* loadingFilterWidget = new LoadingFilterWidget(this);
				loadingFilterWidget->SetFilters(ge, mapping);
				loadingFilterWidget->layout()->setContentsMargins(0, 0, 0, 0);
				m_loadingFilterWidgetsStack->addWidget(loadingFilterWidget);
				m_loadingFilterWidgetMap[filename] = loadingFilterWidget;
			}

			if (IsGlyphEd){
				progress->setValue(progress->value() + (progress->maximum() / m_filenameToTitleMap.count()));
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
}*/

void SharedVisualizationsWidget::ClearAll() {

	if (m_viewListWidget != nullptr) {

		delete m_viewListWidget;
	}
	m_viewListWidget = new SynGlyphX::TitleTreeWidget(this);
	m_viewListWidget->setStyleSheet("background-color: #eff2f7;");
	m_viewListWidget->SetAllowMultiselect(false);
	m_viewListWidget->SetTitle(tr("View(s)"));
	m_viewListWidget->layout()->setContentsMargins(0, 0, 0, 0);
	addWidget(m_viewListWidget);

	QObject::connect(m_viewListWidget, &SynGlyphX::TitleTreeWidget::SelectedTooltipChanged, this, &SharedVisualizationsWidget::OnFileSelected);

	m_viewListWidget->blockSignals(true);

	m_loadingFilterWidgetMap.clear();
	m_filenameToTitleMap.clear();
	if (m_loadingFilterWidgetsStack != nullptr) {

		delete m_loadingFilterWidgetsStack;
	}
	m_loadingFilterWidgetsStack = new QStackedWidget(this);
	addWidget(m_loadingFilterWidgetsStack);

	m_viewListWidget->blockSignals(false);

	m_loadingFilterWidgetsStack->setVisible(false);
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

	if (!QFileInfo(filename).exists()) {

		m_currentFilename = "";
		m_loadingFilterWidgetsStack->setVisible(false);
	}
	else {

		m_currentFilename = filename.toStdString();
		if (m_loadingFilterWidgetMap.count(m_currentFilename) != 0) {

			m_loadingFilterWidgetsStack->setCurrentWidget(m_loadingFilterWidgetMap.at(m_currentFilename));
			m_loadingFilterWidgetsStack->setVisible(true);
		}
		else {

			m_loadingFilterWidgetsStack->setVisible(false);
		}
	}
}
