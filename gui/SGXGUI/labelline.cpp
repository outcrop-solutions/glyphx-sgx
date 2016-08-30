#include "labelline.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>

namespace SynGlyphX {

	LabelLine::LabelLine(const QString& labelText, QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);

		QLabel* label = new QLabel(labelText, this);
		QFont font = label->font();
		font.setBold(true);
		label->setFont(font);
		layout->addWidget(label);

		QFrame* line = new QFrame(this);
		line->setFrameShape(QFrame::HLine);
		line->setFrameShadow(QFrame::Plain);
		layout->addWidget(line);

		setLayout(layout);
	}

	LabelLine::~LabelLine()
	{

	}

} //namespace SynGlyphX