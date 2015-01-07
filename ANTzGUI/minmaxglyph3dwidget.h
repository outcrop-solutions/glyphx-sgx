#ifndef MINMAXGLYPH3DWIDGET_H
#define MINMAXGLYPH3DWIDGET_H

#include "antzgui_global.h"
#include <QtWidgets/QWidget>
#include <QtCore/QItemSelectionModel>
#include "antzsingleglyphtreewidget.h"
#include "minmaxglyphtreemodel.h"

namespace SynGlyphXANTz {

	class ANTZGUI_EXPORT MinMaxGlyph3DWidget : public QWidget
	{
		Q_OBJECT

	public:
		MinMaxGlyph3DWidget(QWidget *parent);
		~MinMaxGlyph3DWidget();

		virtual void SetModel(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel);

	private:
		ANTzSingleGlyphTreeWidget* m_maxGlyph3DWidget;
	};

} //namespace SynGlyphXANTz

#endif // MINMAXGLYPH3DWIDGET_H
