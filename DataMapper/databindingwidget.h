#ifndef DATABINDINGWIDGET_H
#define DATABINDINGWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDataWidgetMapper>
#include "bindinglineedit.h"
#include "richtexteditor.h"
#include "datatransformmapping.h"
#include "minmaxglyphmodel.h"

class DataBindingWidget : public QTabWidget
{
	Q_OBJECT

public:
	DataBindingWidget(MinMaxGlyphModel* model, QWidget *parent = 0);
	~DataBindingWidget();

public slots:
	void CommitChanges();
	void OnBaseObjectChanged();

private slots:
	void OnModelReset();

private:
	void CreateTagAndDescriptionWidget();
	void CreatePropertiesTable();
	void CreateIntegerPropertyWidgets(QGridLayout* layout, int row);
	void CreateDoublePropertyWidgets(QGridLayout* layout, int row, bool addToPositionXYList = false);
	void CreateColorPropertyWidgets(QGridLayout* layout, int row);
	void CreateRowOfPropertyWidgets(QGridLayout* layout, QWidget* minWidget, QWidget* maxWidget, int row);
	void CreateGridLine(QGridLayout* layout, QFrame::Shape shape, int index, int thickness = 1);
	void EnablePositionXYMixMaxWidgets(bool enable);

	BindingLineEdit* m_tagLineEdit;
	SynGlyphX::RichTextEditor* m_descriptionEdit;
	QList<QDataWidgetMapper*> m_dataWidgetMappers;
	MinMaxGlyphModel* m_model;
	QList<QWidget*> m_positionXYMinMaxWidgets;
};

#endif // DATABINDINGWIDGET_H
