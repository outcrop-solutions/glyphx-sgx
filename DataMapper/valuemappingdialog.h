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
#include "valuemappingrangewidget.h"
#include "valuemappingfunction.h"
#include <set>
#include "glyphenumcombobox.h"

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
		Color,
		GeometryShape,
		VirtualTopology
	};

	ValueMappingDialog(InputType input, OutputType output, QWidget *parent);
	virtual ~ValueMappingDialog();

	void SetOutputSpinBoxRange(double min, double max);

	virtual void accept();

protected slots:
	void OnAddKeyValue();
	void OnRemoveKeyValue();
	void OnTableSelectionChanged();
	void OnClearAllKeyValues();
	void OnLoadFromFile();
	void OnSaveToFile();

protected:
	virtual bool CreateMappingData() = 0;
	virtual void LoadFile(const std::string& filename) = 0;
	virtual void SaveFile(const std::string& filename) = 0;

	void OnNumberOfRowsInTableChanged();
	void AddRow();
	SynGlyphX::ValueMappingRange GetRangeFromWidget(int row, int column = 0);
	std::wstring GetTextFromWidget(int row, int column = 0);
	double GetDoubleFromWidget(int row, int column = 0);
	SynGlyphX::GlyphColor GetColorFromWidget(int row, int column = 1);
	SynGlyphX::GlyphGeometryInfo::Shape GetShapeFromWidget(int row, int column = 1);
	SynGlyphX::VirtualTopologyInfo::Type GetVirtualTopologyFromWidget(int row, int column = 1);
	QDoubleSpinBox* CreateDoubleSpinBox(double min = -100000.0, double max = 100000.0);

	QTableWidget* m_table;

	QDoubleSpinBox* m_defaultDoubleWidget;
	SynGlyphX::ColorButton* m_defaultColorWidget;
	SynGlyphX::GlyphShapeComboBox* m_defaultShapeWidget;
	SynGlyphX::VirtualTopologyComboBox* m_defaultVirtualTopologyWidget;

	SynGlyphX::ValueMappingRangeWidget* m_inputRangeWidget;
	QDoubleSpinBox* m_inputDoubleWidget;
	QLineEdit* m_inputTextWidget;
	QDoubleSpinBox* m_outputDoubleWidget;
	SynGlyphX::ColorButton* m_outputColorWidget;
	SynGlyphX::GlyphShapeComboBox* m_outputShapeWidget;
	SynGlyphX::VirtualTopologyComboBox* m_outputVirtualTopologyWidget;

	QPushButton* m_removeEntryButton;
	QPushButton* m_saveToFileButton;

	double m_outputSpinBoxMin;
	double m_outputSpinBoxMax;

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
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Numeric2NumericMappingData::SharedPtr m_mappingData;
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
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Numeric2ColorMappingData::SharedPtr m_mappingData;
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
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Text2NumericMappingData::SharedPtr m_mappingData;
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
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Text2ColorMappingData::SharedPtr m_mappingData;
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
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Range2NumericMappingData::SharedPtr m_mappingData;
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
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Range2ColorMappingData::SharedPtr m_mappingData;
};

//---------------------------------------------------------------------

class Numeric2ShapeMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Numeric2ShapeMappingDialog(QWidget *parent);
	virtual ~Numeric2ShapeMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Numeric2ShapeMappingData::ConstSharedPtr mapping);
	SynGlyphX::Numeric2ShapeMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Numeric2ShapeMappingData::SharedPtr m_mappingData;
};

class Numeric2VirtualTopologyMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Numeric2VirtualTopologyMappingDialog(QWidget *parent);
	virtual ~Numeric2VirtualTopologyMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Numeric2VirtualTopologyMappingData::ConstSharedPtr mapping);
	SynGlyphX::Numeric2VirtualTopologyMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Numeric2VirtualTopologyMappingData::SharedPtr m_mappingData;
};

class Text2ShapeMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Text2ShapeMappingDialog(QWidget *parent);
	virtual ~Text2ShapeMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Text2ShapeMappingData::ConstSharedPtr mapping);
	SynGlyphX::Text2ShapeMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Text2ShapeMappingData::SharedPtr m_mappingData;
};

class Text2VirtualTopologyMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Text2VirtualTopologyMappingDialog(QWidget *parent);
	virtual ~Text2VirtualTopologyMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Text2VirtualTopologyMappingData::ConstSharedPtr mapping);
	SynGlyphX::Text2VirtualTopologyMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Text2VirtualTopologyMappingData::SharedPtr m_mappingData;
};

class Range2ShapeMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Range2ShapeMappingDialog(QWidget *parent);
	virtual ~Range2ShapeMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Range2ShapeMappingData::ConstSharedPtr mapping);
	SynGlyphX::Range2ShapeMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Range2ShapeMappingData::SharedPtr m_mappingData;
};

class Range2VirtualTopologyMappingDialog : public ValueMappingDialog
{
	Q_OBJECT

public:
	Range2VirtualTopologyMappingDialog(QWidget *parent);
	virtual ~Range2VirtualTopologyMappingDialog();

	void SetDialogFromMapping(SynGlyphX::Range2VirtualTopologyMappingData::ConstSharedPtr mapping);
	SynGlyphX::Range2VirtualTopologyMappingData::SharedPtr GetMappingFromDialog() const;

protected:
	virtual bool CreateMappingData();
	virtual void LoadFile(const std::string& filename);
	virtual void SaveFile(const std::string& filename);

	SynGlyphX::Range2VirtualTopologyMappingData::SharedPtr m_mappingData;
};

#endif // VALUEMAPPINGDIALOG_H
