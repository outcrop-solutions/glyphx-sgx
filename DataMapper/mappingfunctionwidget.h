#ifndef MAPPINGFUNCTIONWIDGET_H
#define MAPPINGFUNCTIONWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include "datamappingfunction.h"
#include "minmaxglyphmodel.h"

class MappingFunctionWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString function READ GetFunction WRITE SetFunction USER true)

public:
	MappingFunctionWidget(MinMaxGlyphModel* model, int row, double min, double max, QWidget *parent);
	~MappingFunctionWidget();

	QString GetFunction() const;

signals:
	void SupportedInputChanged(SynGlyphX::MappingFunctionData::Input supportedInput);
	void FunctionChanged();

public slots:
	void SetFunction(const QString& function);

private slots:
	void OnFunctionComboBoxChanged();
	void OnEditPropertiesClicked();

private:
	QComboBox* m_functionComboBox;
	QPushButton* m_editPropertiesButton;
	MinMaxGlyphModel* m_model;
	int m_row;
	double m_dialogOutputMin;
	double m_dialogOutputMax;
};

#endif // MAPPINGFUNCTIONWIDGET_H
