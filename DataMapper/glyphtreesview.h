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
#ifndef GLYPHTREESVIEW_H
#define GLYPHTREESVIEW_H

#include "treeeditview.h"
#include "sharedactionlist.h"

namespace SynGlyphX {
	class DataTransformModel;
}

class GlyphTreesViewMemento;

class GlyphTreesView : public SynGlyphX::TreeEditView
{
	friend class GlyphTreesViewMemento;
	Q_OBJECT

public:
	GlyphTreesView(SynGlyphX::DataTransformModel* sourceModel, QWidget *parent = 0);
	~GlyphTreesView();

	//Memento lifetime is managed by undo command (deleted in command destructor), consider changing to shared_ptr
	GlyphTreesViewMemento* CreateMemento() const;
	void ReinstateMemento(GlyphTreesViewMemento* m);

	const SynGlyphX::SharedActionList& GetGlyphActions();

	const QAction* const GetClearSelectedInputBindingsAction() const;

	void SelectLastGlyphTreeRoot();

	QSize sizeHint() const override;

signals:
	void SelectionChangedSourceModel(const QModelIndexList& selectedIndexes);
	void UpdateStatusBar(const QString& message, int timeout);

protected:
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	virtual void EnableActions(const QItemSelection& selection);
	virtual SynGlyphX::DataMappingGlyphGraph GetGraphForCopyToClipboard(const QModelIndex& index, bool includeChildren);
	virtual void OverwriteGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& graph);
	virtual void AddGlyphsAsChildren(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& graph);
	
private slots:
	void AddChildren();
	void ExportGlyphToFile();
	void ChangeOptions();

private:
	SynGlyphX::DataTransformModel* m_sourceModel;

	SynGlyphX::SharedActionList m_glyphActions;
	QAction* m_addChildrenAction;
	QAction* m_clearSelectedInputBindingsAction;
	QAction* m_exportGlyphToFileAction;

	QAction* m_glyphOptionsAction;
};

#endif // GLYPHTREESVIEW_H
