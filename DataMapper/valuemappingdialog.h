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

#ifndef VALUEMAPPINGDIALOG_H
#define VALUEMAPPINGDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include "colorbutton.h"
#include "rangewidget.h"
#include "valuemappingfunction.h"
#include <set>

class ValueMappingDialog : public QDialog
{
	Q_OBJECT

public:
	enum class InputType {
		Numeric,
		Text,
		Range
	};

	enum class OutputType {
		Numeric,
		Color
	};
	ValueMappingDialog(InputType input, OutputType output, QWidget *parent);
	virtual ~ValueMappingDialog();

	//void SetDialogFromMapping(SynGlyphX::ValueMappingData<OutputType, InputType, KeyType>::ConstSharedPtr mapping);
	//SynGlyphX::ValueMappingData<OutputType, InputType, KeyType>::SharedPtr GetMappingFromDialog() const;

protected slots:
	void OnAddKeyValue();
	void OnRemoveKeyValue();
	void OnTableSelectionChanged();
	void OnClearAllKeyValues();

protected:
	void AddRow();

	QTableWidget* m_table;

	QDoubleSpinBox* m_defaultDoubleWidget;
	SynGlyphX::ColorButton* m_defaultColorWidget;

	SynGlyphX::RangeWidget* m_inputRangeWidget;
	QDoubleSpinBox* m_inputDoubleWidget;
	QLineEdit* m_inputTextWidget;
	QDoubleSpinBox* m_outputDoubleWidget;
	SynGlyphX::ColorButton* m_outputColorWidget;

	QPushButton* m_removeEntryButton;

	std::set<double> m_doubleInputValues;
	std::set<std::wstring> m_textInputValues;
	std::set<SynGlyphX::Range> m_rangeInputValues;

private:
	InputType m_input;
	OutputType m_output;
};

class Numeric2NumericMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Numeric2NumericMappingDialog(QWidget *parent);
	virtual ~Numeric2NumericMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Numeric2NumericMappingData::ConstSharedPtr mapping);
	SynGlyphX::Numeric2NumericMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	
};

class Numeric2ColorMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Numeric2ColorMappingDialog(QWidget *parent);
	virtual ~Numeric2ColorMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Numeric2ColorMappingData::ConstSharedPtr mapping);
	SynGlyphX::Numeric2ColorMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	
};

class Text2NumericMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Text2NumericMappingDialog(QWidget *parent);
	virtual ~Text2NumericMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Text2NumericMappingData::ConstSharedPtr mapping);
	SynGlyphX::Text2NumericMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	
};

class Text2ColorMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Text2ColorMappingDialog(QWidget *parent);
	virtual ~Text2ColorMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Text2ColorMappingData::ConstSharedPtr mapping);
	SynGlyphX::Text2ColorMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	
};

class Range2NumericMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Range2NumericMappingDialog(QWidget *parent);
	virtual ~Range2NumericMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Range2NumericMappingData::ConstSharedPtr mapping);
	SynGlyphX::Range2NumericMappingData::SharedPtr GetMappingFromDialog() const;

protected:

};

class Range2ColorMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Range2ColorMappingDialog(QWidget *parent);
	virtual ~Range2ColorMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Range2ColorMappingData::ConstSharedPtr mapping);
	SynGlyphX::Range2ColorMappingData::SharedPtr GetMappingFromDialog() const;

protected:

};

#endif // VALUEMAPPINGDIALOG_H
