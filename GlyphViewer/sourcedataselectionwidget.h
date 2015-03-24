#ifndef SOURCEDATASELECTIONWIDGET_H
#define SOURCEDATASELECTIONWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QStackedLayout>
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
	void OnModelReset();

private:
	QItemSelectionModel* m_selectionModel;
	QPushButton* m_sourceWidgetButton;
	QScopedPointer<SourceDataWidget> m_sourceDataWindow;
	QComboBox* m_tableComboBox;
	QStackedLayout* m_elasticListsLayout;

	SynGlyphX::SourceDataCache::SharedPtr m_sourceDataCache;
};

#endif // SOURCEDATASELECTIONWIDGET_H
