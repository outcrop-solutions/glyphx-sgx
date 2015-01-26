#ifndef MAPPINGFUNCTIONWIDGET_H
#define MAPPINGFUNCTIONWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include "datamappingfunction.h"

class MappingFunctionWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString value READ GetFunction WRITE SetFunction USER true)

public:
	MappingFunctionWidget(QWidget *parent);
	~MappingFunctionWidget();

	QString GetFunction() const;

signals:
	void SupportedInputChanged(SynGlyphX::MappingFunctionData::Input supportedInput);

public slots:
	void SetFunction(const QString& function);

private slots:
	void OnFunctionComboBoxChanged(int index);
	void OnEditPropertiesClicked();

private:
	QComboBox* m_functionComboBox;
	QPushButton* m_editPropertiesButton;
};

#endif // MAPPINGFUNCTIONWIDGET_H
