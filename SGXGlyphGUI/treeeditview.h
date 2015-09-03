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

#ifndef SYNGLYPHX_TREEEDITVIEW_H
#define SYNGLYPHX_TREEEDITVIEW_H

#include "sgxglyphgui_global.h"
#include "treeview.h"
#include "sharedactionlist.h"
#include "datamappingglyphgraph.h"

namespace SynGlyphX {

	class SGXGLYPHGUI_EXPORT TreeEditView : public TreeView
	{
		Q_OBJECT

	public:
		TreeEditView(QWidget *parent);
		~TreeEditView();

		const SynGlyphX::SharedActionList& GetEditActions() const;

		virtual void setModel(QAbstractItemModel* model);

	protected slots:
		virtual void DeleteSelected() = 0;
		virtual void DeleteChildrenFromSelected() = 0;
		void OnClipboardDataChanged();
		void OnRowsInsertedOrRemoved();

	protected:
		virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
		virtual DataMappingGlyphGraph::LinklessGraph GetGraphForCopyToClipboard(const QModelIndex& index) = 0;
		virtual DataMappingGlyph GetGlyphForCopyToClipboard(const QModelIndex& index) = 0;
		virtual void OverwriteGlyph(const QModelIndex& index, const DataMappingGlyphGraph::LinklessGraph& graph) = 0;
		virtual void AddGlyphsAsChildren(const QModelIndex& index, const DataMappingGlyphGraph::LinklessGraph& graph) = 0;
		virtual void EnableActions(const QItemSelection& selected) = 0;
		bool DoesClipboardHaveGlyph() const;
		void CopyToClipboard(bool includeChildren, bool removeFromTree);
		void PasteFromClipboard(bool addAsChild);

		void CreateEditActions();

		SynGlyphX::SharedActionList m_editActions;

		//Edit menu actions
		QAction* m_cutAction;
		QAction* m_copyAction;
		QAction* m_copyWithChildrenAction;
		QAction* m_pasteAction;
		QAction* m_pasteAsChildAction;
		QAction* m_deleteAction;
		QAction* m_deleteChildrenAction;

	private:

	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_TREEEDITVIEW_H
