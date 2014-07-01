#ifndef DATABINDINGWIDGET_H
#define DATABINDINGWIDGET_H

#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDataWidgetMapper>
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
	void CreateIntegerPropertyWidgets(QGridLayout* layout, const QString& name, int row);
	void CreateDoublePropertyWidgets(QGridLayout* layout, const QString& name, int row);
	void CreateColorPropertyWidgets(QGridLayout* layout, const QString& name, int row);
	void CreateRowOfPropertyWidgets(QGridLayout* layout, const QString& name, QWidget* minWidget, QWidget* maxWidget, int row);
	void CreateGridLine(QGridLayout* layout, QFrame::Shape shape, int index, int thickness = 1);

	BindingLineEdit* m_tagLineEdit;
	SynGlyphX::RichTextEditor* m_descriptionEdit;
	QList<QDataWidgetMapper*> m_dataWidgetMappers;
};

#endif // DATABINDINGWIDGET_H
