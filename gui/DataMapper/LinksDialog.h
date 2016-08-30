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
#ifndef LINKSDIALOG_H
#define LINKSDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStackedWidget>
#include <QtCore/QSignalMapper>
#include "Link.h"

class QComboBox;
class QSpinBox;
class QCheckBox;
class GlyphTreesView;
class LinkLineEdit;
class DataMapping3DWidget;

namespace SynGlyphX {
	class DataTransformModel;
	class ColorButton;
}

class LinksDialog : public QDialog
{
	Q_OBJECT
		friend class FunctionDialog;
public:

	LinksDialog(SynGlyphX::DataTransformModel* dataTransformModel, QWidget* parent = 0);
	~LinksDialog();

	void accept() override;
	const SynGlyphX::Link& GetLink();
	void SetLink(const SynGlyphX::Link& link);

	//Set -1 for new link
	void SetEditRow(int row);
	int GetEditRow() { return m_row;  }
	void Clear();

private slots:
	void OnFunctionProperties();

private:
	SynGlyphX::LinkNode GetNode(GlyphTreesView* treeView, LinkLineEdit* lineEdit);
	void SelectGlyph(const QModelIndex &parent, GlyphTreesView* treeView, const SynGlyphX::LinkNode& node);
	void SetNode(const SynGlyphX::LinkNode& node, GlyphTreesView* treeView, LinkLineEdit* lineEdit);
	bool Validate();
	QLineEdit*	  m_nameLineEdit;
	LinkLineEdit* m_fromLineEdit;
	LinkLineEdit* m_toLineEdit;

	SynGlyphX::DataTransformModel* m_dataTransformModel;

	GlyphTreesView* m_fromGlyphTree;
	GlyphTreesView* m_toGlyphTree;

	DataMapping3DWidget* m_fromGlyph3DView;
	DataMapping3DWidget* m_toGlyph3DView;

	SynGlyphX::ColorButton* m_colorButton;
	QCheckBox*	m_inheritColorCheckBox;
	QSpinBox*	m_transparensySpinBox;

	QComboBox* m_functionComboBox;
	QComboBox* m_shapeComboBox;
	QPushButton* m_functionPushButton;
	SynGlyphX::Link* m_link;
	int m_row;
};

#endif // LINKSDIALOG_H
