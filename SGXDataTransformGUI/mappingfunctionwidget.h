///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///
#pragma once
#ifndef MAPPINGFUNCTIONWIDGET_H
#define MAPPINGFUNCTIONWIDGET_H

#include "sgxdatatransformgui_global.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include "datamappingfunction.h"
#include "glyphrolestablemodel.h"

class SGXDATATRANSFORMGUI_EXPORT MappingFunctionWidget : public QWidget
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
	void OnEditPropertiesClicked();

private:
	void OnFunctionComboBoxChanged(bool emitInputChange);
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
