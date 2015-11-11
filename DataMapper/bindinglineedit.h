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

#ifndef BINDINGLINEEDIT_H
#define BINDINGLINEEDIT_H

#include <QtWidgets/QLineEdit>
#include "inputbinding.h"
#include "glyphrolestablemodel.h"
#include "datamappingfunction.h"
#include "inputfieldmimedata.h"

class BindingLineEdit : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(SynGlyphX::InputField value READ GetInputField WRITE SetInputField USER true)

public:
	BindingLineEdit(const GlyphRolesTableModel* model, QWidget *parent = 0, SynGlyphX::MappingFunctionData::Input acceptedInputTypes = SynGlyphX::MappingFunctionData::Input::All);
	~BindingLineEdit();

	const SynGlyphX::InputField& GetInputField() const;
	bool OnlyAcceptsNumericField() const;

public slots:
	void SetInputField(const SynGlyphX::InputField& inputfield);
	void Clear();
	void SetAcceptedInputTypes(SynGlyphX::MappingFunctionData::Input acceptedInputTypes);

signals:
	void ValueChanged(SynGlyphX::InputField);
	void ValueChangedByUser(SynGlyphX::InputField);

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
	virtual void contextMenuEvent(QContextMenuEvent* event);

private:
	const GlyphRolesTableModel* m_model;
	SynGlyphX::InputField m_inputField;
	SynGlyphX::MappingFunctionData::Input m_acceptedInputTypes;
	QAction* m_clearAction;
	QLineEdit* m_lineEdit;
};

#endif // BINDINGLINEEDIT_H
