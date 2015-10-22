#include "richtexteditor.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace SynGlyphX {

	RichTextEditor::RichTextEditor(const QString& title, QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);

		QHBoxLayout* headerLayout = new QHBoxLayout(this);

		if (!title.isEmpty()) {

			QLabel* label = new QLabel(title, this);
			headerLayout->addWidget(label);
		}
		headerLayout->addStretch(1);
		
		layout->addLayout(headerLayout);

		m_textEdit = new QTextEdit(this);
		layout->addWidget(m_textEdit);

		setLayout(layout);
	}

	RichTextEditor::~RichTextEditor()
	{

	}

} //namespace SynGlyphX
