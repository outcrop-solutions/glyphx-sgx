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

#ifndef GLYPHTREEVIEW_H
#define GLYPHTREEVIEW_H

#include "treeview.h"
#include "minmaxglyphtreemodel.h"
#include "sharedactionlist.h"

class GlyphTreeView : public SynGlyphX::TreeView
{
    Q_OBJECT

public:
	GlyphTreeView(SynGlyphXANTz::MinMaxGlyphTreeModel* model, SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent = 0);
    ~GlyphTreeView();

	const SynGlyphX::SharedActionList& GetGlyphActions() const;
	const SynGlyphX::SharedActionList& GetEditActions() const;

public slots:
	void AddChildren();

protected:
    virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private slots:
	void DeleteSelected();
	void DeleteChildrenFromSelected();
	void PropertiesActivated();

private:
	void CreateContextMenuActions();
	void CreatePropertiesDialog();
	void CreateAddChildrenDialog();
	void EnableActions();

	SynGlyphXANTz::MinMaxGlyphTreeModel* m_model;
	SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType m_glyphTreeType;

	SynGlyphX::SharedActionList m_glyphActions;
	SynGlyphX::SharedActionList m_editActions;

	//Edit menu actions
	QAction* m_cutAction;
	QAction* m_copyAction;
	QAction* m_pasteAction;
	QAction* m_pasteAsChildAction;
	QAction* m_deleteAction;
	QAction* m_deleteChildrenAction;
	QAction* m_propertiesAction;

	//Glyph menu actions
	QAction* m_addChildrenAction;
};

#endif // GLYPHTREEVIEW_H
