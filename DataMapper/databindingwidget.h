#ifndef DATABINDINGWIDGET_H
#define DATABINDINGWIDGET_H

#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include "bindinglineedit.h"

class DataBindingWidget : public QSplitter
{
	Q_OBJECT

public:
	DataBindingWidget(QWidget *parent = 0);
	~DataBindingWidget();

private:
	BindingLineEdit* m_tagLineEdit;
	QTextEdit* m_descriptionEdit;
};

#endif // DATABINDINGWIDGET_H
