#ifndef SOURCEDATAWIDGET_H
#define SOURCEDATAWIDGET_H

#include <QtWidgets/QTabWidget>
#include "glyphforestmodel.h"
#include "sourcedatacache.h"
#include <QtCore/QItemSelectionModel>

class SourceDataWidget : public QTabWidget
{
	Q_OBJECT

public:
	SourceDataWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent = nullptr);
	~SourceDataWidget();

signals:
	void WindowHidden();

protected:
	virtual void closeEvent(QCloseEvent* event);

private slots:
	void OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	void UpdateTables();
	unsigned long GetRootRow(const QModelIndex& index) const;

	GlyphForestModel* m_model;
	QItemSelectionModel* m_selectionModel;
	SynGlyphX::SourceDataCache::SharedPtr m_sourceDataCache;
};

#endif // SOURCEDATAWIDGET_H
