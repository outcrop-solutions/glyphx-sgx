#include "textglyphpropertieswidget.h"
#include <QtWidgets/QVBoxLayout>
#include "labelline.h"

namespace SynGlyphX {

	TextGlyphPropertiesWidget::TextGlyphPropertiesWidget(QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);

		LabelLine* tagLine = new LabelLine(tr("Tag"), this);
		mainLayout->addWidget(tagLine);

		m_tagLineEdit = new QLineEdit(this);
		m_tagLineEdit->setReadOnly(true);
		mainLayout->addWidget(m_tagLineEdit);

		LabelLine* descriptionLine = new LabelLine(tr("Description"), this);
		mainLayout->addWidget(descriptionLine);

		m_descriptionEdit = new QTextEdit(this);
		m_descriptionEdit->setReadOnly(true);
		mainLayout->addWidget(m_descriptionEdit, 1);

		setLayout(mainLayout);
	}

	TextGlyphPropertiesWidget::~TextGlyphPropertiesWidget()
	{

	}

	void TextGlyphPropertiesWidget::SetWidgetFromGlyph(const Glyph& glyph) {

		m_tagLineEdit->setText(QString::fromStdWString(glyph.GetTag()));
		m_descriptionEdit->setHtml(QString::fromStdWString(glyph.GetDescription()));
	}

	void TextGlyphPropertiesWidget::SetReadOnly(bool readOnly) {

		m_tagLineEdit->setReadOnly(readOnly);
		m_descriptionEdit->setReadOnly(readOnly);
	}

} //namespace SynGlyphX