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

#ifndef LINKSDIALOG_H
#define LINKSDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtCore/QSignalMapper>
#include "mapoptionswidget.h"
#include "browselineedit.h"
#include "doublesizewidget.h"
#include "defaultbaseimagescombobox.h"
#include "colorbutton.h"
#include "Link.h"
#include "glyphtreesview.h"
#include "bindinglineedit.h"
#include "glyphrolestablemodel.h"

class LinkLineEdit : public QLineEdit
{
public:
	LinkLineEdit(DataTransformModel* dataTransformModel, QWidget *parent = 0);
	virtual ~LinkLineEdit() {}
	const SynGlyphX::InputField& GetInputField() const { return m_inputField; }
	void SetInputField(const SynGlyphX::InputField& inputField);
protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
private:
	SynGlyphX::InputField m_inputField;
	DataTransformModel* m_dataTransformModel;
};

class LinksDialog : public QDialog
{
	friend class LinkLineEdit;
	Q_OBJECT

public:
	LinksDialog(DataTransformModel* dataTransformModel, GlyphRolesTableModel* glyphRolesTableModel, QWidget* parent = 0);
	~LinksDialog();

	void accept() override;
	const SynGlyphX::Link& GetLink() const;
	void SetLink(const SynGlyphX::Link& link) {}

private slots:


private:
	LinkLineEdit* m_fromLineEdit;
	LinkLineEdit* m_toLineEdit;

	DataTransformModel* m_dataTransformModel;
	GlyphRolesTableModel* m_glyphRolesTableModel;

	GlyphTreesView* m_fromGlyphTree;
	GlyphTreesView* m_toGlyphTree;

	SynGlyphX::ColorButton* m_colorButton;
	QCheckBox*	m_inheritColorCheckBox;
	QSpinBox*	m_transparensySpinBox;

};

#endif // LINKSDIALOG_H
