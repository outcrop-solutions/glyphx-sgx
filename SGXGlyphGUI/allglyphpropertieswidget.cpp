#include "allglyphpropertieswidget.h"
#include "glyph.h"
#include "textglyphpropertieswidget.h"
namespace SynGlyphX {

	AllGlyphPropertiesWidget::AllGlyphPropertiesWidget(bool addLockToScaleWidget, VisualGlyphPropertiesWidget::ChildOptions childOptions, QWidget *parent)
		: QTabWidget(parent)
	{
		m_visualPropertiesWidget = new VisualGlyphPropertiesWidget(addLockToScaleWidget, childOptions, this);
		addTab(m_visualPropertiesWidget, tr("Visual"));

		m_textPropertiesWidget = new TextGlyphPropertiesWidget(this);
		addTab(m_textPropertiesWidget, tr("Text"));
	}

	AllGlyphPropertiesWidget::~AllGlyphPropertiesWidget()
	{

	}

	void AllGlyphPropertiesWidget::SetReadOnly(bool readOnly) {

		m_visualPropertiesWidget->setEnabled(!readOnly);
		m_textPropertiesWidget->SetReadOnly(readOnly);
	}

	void AllGlyphPropertiesWidget::SetWidgetFromGlyph(const Glyph& glyph, unsigned int numberOfChildren, bool isNotRootNode) {

		m_visualPropertiesWidget->SetWidgetFromGlyph(glyph, isNotRootNode);
		m_visualPropertiesWidget->SetNumberOfChildren(numberOfChildren);
		m_textPropertiesWidget->SetWidget(QString::fromStdWString(glyph.GetTag()), QString::fromStdWString(glyph.GetURL()), QString::fromStdWString(glyph.GetDescription()));
	}

} //namespace SynGlyphX