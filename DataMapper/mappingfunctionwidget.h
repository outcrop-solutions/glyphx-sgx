#ifndef MAPPINGFUNCTIONWIDGET_H
#define MAPPINGFUNCTIONWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include "datamappingfunction.h"
#include "glyphrolestablemodel.h"

class MappingFunctionWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString function READ GetFunction WRITE SetFunction USER true)

public:
	enum KeyType {
		Numeric,
		Color,
		GeometryShape,
		VirtualTopology
	};

	MappingFunctionWidget(KeyType keyType, GlyphRolesTableModel* model, int row, QWidget *parent);
	~MappingFunctionWidget();

	QString GetFunction() const;

	void SetDialogOutputMinMax(double min, double max);

signals:
	void SupportedInputChanged(SynGlyphX::MappingFunctionData::Input supportedInput);
	void FunctionChanged();

public slots:
	void SetFunction(const QString& function);

private slots:
	void OnFunctionComboBoxChangedByUser();
	void OnFunctionComboBoxChanged();
	void OnEditPropertiesClicked();

private:
	static QStringList CreateNumericColorFunctionList();
	static QStringList CreateEnumerationFunctionList();

	QComboBox* m_functionComboBox;
	QPushButton* m_editPropertiesButton;
	GlyphRolesTableModel* m_model;
	int m_row;
	double m_dialogOutputMin;
	double m_dialogOutputMax;
	KeyType m_keyType;

	static const QStringList s_numericColorFunctions;
	static const QStringList s_enumerationFunctions;
};

#endif // MAPPINGFUNCTIONWIDGET_H
