#ifndef BASEOBJECTLISTVIEW_H
#define BASEOBJECTLISTVIEW_H

#include <QtWidgets/QListView>
#include "sharedactionlist.h"
#include "datatransformmodel.h"

class BaseObjectListView : public QListView
{
	Q_OBJECT

public:
	BaseObjectListView(DataTransformModel* dataTransformModel, QWidget *parent);
	~BaseObjectListView();

	const SynGlyphX::SharedActionList& GetSharedActions();

protected:
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private slots:
	void RemoveBaseObject();
	void ShowBaseObjectProperties();

private:
	void EnableActions();

	DataTransformModel* m_dataTransformModel;

	SynGlyphX::SharedActionList m_sharedActions;
	QAction* m_removeBaseObjectAction;
	QAction* m_propertiesAction;
	
};

#endif // BASEOBJECTLISTVIEW_H
