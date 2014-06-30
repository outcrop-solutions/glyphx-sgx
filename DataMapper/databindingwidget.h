#ifndef DATABINDINGWIDGET_H
#define DATABINDINGWIDGET_H

#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGridLayout>
#include "bindinglineedit.h"
#include "richtexteditor.h"

class DataBindingWidget : public QSplitter
{
	Q_OBJECT

public:
	DataBindingWidget(QWidget *parent = 0);
	~DataBindingWidget();

public slots:
	void SetGlyphModelIndex(const QModelIndex& index);

private:
	void CreateTagAndDescriptionWidget();
	void CreatePropertiesTable();
	void CreatePropertyWidgets(QGridLayout* layout);
	void CreateGridLine(QGridLayout* layout, QFrame::Shape shape, int index, int thickness = 1);

	BindingLineEdit* m_tagLineEdit;
	SynGlyphX::RichTextEditor* m_descriptionEdit;
};

#endif // DATABINDINGWIDGET_H
