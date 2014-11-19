#ifndef MINMAXGLYPH3DWIDGET_H
#define MINMAXGLYPH3DWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCore/QItemSelectionModel>
#include "antzsingleglyphtreewidget.h"
#include "minmaxglyphtreemodel.h"

class MinMaxGlyph3DWidget : public QWidget
{
	Q_OBJECT

public:
	MinMaxGlyph3DWidget(MinMaxGlyphTreeModel* model, QWidget *parent);
	~MinMaxGlyph3DWidget();

private:
	MinMaxGlyphTreeModel* m_model;
	QItemSelectionModel* m_selectionModel;

	ANTzSingleGlyphTreeWidget* m_maxGlyph3DWidget;
};

#endif // MINMAXGLYPH3DWIDGET_H
