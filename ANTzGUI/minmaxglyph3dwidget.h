#ifndef MINMAXGLYPH3DWIDGET_H
#define MINMAXGLYPH3DWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCore/QItemSelectionModel>
#include "antzwidget.h"
#include "glyphtreemodel.h"
#include "minmaxglyphtreemodel.h"

class MinMaxGlyph3DWidget : public QWidget
{
	Q_OBJECT

public:
	MinMaxGlyph3DWidget(MinMaxGlyphTreeModel* model, QWidget *parent);
	~MinMaxGlyph3DWidget();

private slots:
	void OnModelReset();
	void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);
	void UpdateSelectionFromMaxView(const QItemSelection& selected, const QItemSelection& deselected);

private:
	void ConnectMaxViewSelection();

	MinMaxGlyphTreeModel* m_model;
	QItemSelectionModel* m_selectionModel;
	QMetaObject::Connection m_maxViewSelectConnection;

	ANTzWidget* m_maxGlyph3DWidget;
	GlyphTreeModel* m_maxGlyphModel;
	QItemSelectionModel* m_maxGlyphSelectionModel;
};

#endif // MINMAXGLYPH3DWIDGET_H
