#include "verticalscrollarea.h"
#include <QtCore/QEvent>
#include <QtWidgets/QScrollBar>

namespace SynGlyphX {

	VerticalScrollArea::VerticalScrollArea(QWidget *parent)
		: QScrollArea(parent)
	{
		setWidgetResizable(true);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}

	VerticalScrollArea::~VerticalScrollArea()
	{

	}

	bool VerticalScrollArea::eventFilter(QObject *o, QEvent *e)
	{
		// This works because QScrollArea::setWidget installs an eventFilter on the widget
		if (o && o == widget() && e->type() == QEvent::Resize) {

			int w = widget()->minimumSizeHint().width();
			setMinimumWidth(widget()->minimumSizeHint().width() + verticalScrollBar()->width());
		}

		return QScrollArea::eventFilter(o, e);
	}


} //namespace SynGlyphX