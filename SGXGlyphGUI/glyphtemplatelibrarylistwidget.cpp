#include "glyphtemplatelibrarylistwidget.h"
#include "glyphbuilderapplication.h"

namespace SynGlyphX {

	GlyphTemplateLibraryListWidget::GlyphTemplateLibraryListWidget(bool multiselect, QWidget *parent)
		: QListView(parent)
	{
		QString glyphTemplatesDirectory = QFileInfo(SynGlyphX::GlyphBuilderApplication::applicationDirPath() + QDir::separator() + "GlyphTemplates").canonicalFilePath();

		m_fileSystemModel = new QFileSystemModel(this);
		m_fileSystemModel->setNameFilters(QStringList("*.sgt"));

		setModel(m_fileSystemModel);
		setRootIndex(m_fileSystemModel->setRootPath(glyphTemplatesDirectory));
		setSelectionMode(multiselect ? QAbstractItemView::ExtendedSelection : QAbstractItemView::SingleSelection);
	}

	GlyphTemplateLibraryListWidget::~GlyphTemplateLibraryListWidget()
	{

	}

	QStringList GlyphTemplateLibraryListWidget::GetSelectedTemplates() const {

		const QModelIndexList& selected = selectionModel()->selectedIndexes();
		QStringList selectedTemplates;
		for (const QModelIndex& index : selected) {

			selectedTemplates << m_fileSystemModel->filePath(index);
		}
		return selectedTemplates;
	}

} //namespace SynGlyphX
