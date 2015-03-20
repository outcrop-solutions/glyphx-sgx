#ifndef SOURCEDATASELECTIONWIDGET_H
#define SOURCEDATASELECTIONWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include "sourcedatawidget.h"
#include "singlewidgetdialog.h"

class SourceDataSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	SourceDataSelectionWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent);
	~SourceDataSelectionWidget();

private slots:
	void OnSourceWidgetWindowHidden();
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	QItemSelectionModel* m_selectionModel;
	QPushButton* m_sourceWidgetButton;
	QScopedPointer<SourceDataWidget> m_sourceDataWindow;
};

#endif // SOURCEDATASELECTIONWIDGET_H
