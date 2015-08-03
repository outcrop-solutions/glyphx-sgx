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

#ifndef ANTZSINGLEGLYPHTREEWIDGET_H
#define ANTZSINGLEGLYPHTREEWIDGET_H

#include "antzsingleglyphgui_global.h"
#include "antzwidget.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include "data/nptypes.h"
#include "glyphgraph.h"
#include "datamappingglyphgraph.h"
#include "minmaxglyphtreemodel.h"
#include <QtCore/QItemSelectionModel>
#include "defaultbaseimageproperties.h"

namespace SynGlyphXANTz {

	class ANTZSINGLEGLYPHGUI_EXPORT ANTzSingleGlyphTreeWidget : public ANTzWidget
	{
		Q_OBJECT

	public:
		enum EditingMode {
			None = 0,
			Move,
			Rotate,
			Size
		};

		ANTzSingleGlyphTreeWidget(MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent = 0);
		~ANTzSingleGlyphTreeWidget();

		void SetAllowMultiSelection(bool allowMultiSelection);
		void SetModel(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel);

		void ShowGlyph(bool show);

		void SetBaseImage(SynGlyphX::DefaultBaseImageProperties::Type baseImage);
		SynGlyphX::DefaultBaseImageProperties::Type GetBaseImage() const;

	public slots:
		void SetEditingMode(EditingMode mode);
		void EnableAnimation(bool enable);

	protected:
		virtual void initializeGL();
		virtual void BeforeDrawScene();
		virtual void AfterDrawScene();
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		virtual void mouseMoveEvent(QMouseEvent* event);

		bool IsRootNodeSelected() const;

		virtual void DeleteNode(pNPnode node);

	private slots:
		void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);
		void OnModelReset();
		void OnModelRowsInserted(const QModelIndex& parent, int first, int last);
		void OnModelRowsMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow);
		void OnModelRowsRemoved(const QModelIndex& parent, int first, int last);
		void UpdateData(const QModelIndex& topLeft, const QModelIndex& bottomRight);

	private:
		void EnableBasedOnModelRowCount();
		QModelIndex IndexFromANTzID(int id) const;
		pNPnode GetGlyphFromModelIndex(const QModelIndex& index) const;
		pNPnode GetRootGlyph() const;

		int GetChildIndexFromParentNoLinks(pNPnode node) const;

		void DeleteRootGlyphNode();

		void CreateNewSubTree(pNPnode parent, const SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph);
		pNPnode CreateNodeFromTemplate(pNPnode parent, const SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph);
		void RebuildTree();

		void UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::DataMappingGlyph& minMaxGlyph);
		void UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::Glyph& glyphTemplate);

		void ConnectDataChangedSignal();
		void DisconnectDataChangedSignal();

		void ResetAnimationValuesInTree(pNPnode node, const SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator& minMaxGlyph);
		void UpdateAnimationValuesFromGlyph(pNPnode node, const SynGlyphX::Glyph& glyph);
		void TurnOffAnimationInNodeTree(pNPnode node);

		void RemoveNodeFromIDMap(pNPnode node);

		pNPnode m_rootGlyph;
		unsigned int m_baseImageTextureID;

		SynGlyphX::Glyph m_clipboardGlyph;

		MinMaxGlyphTreeModel::GlyphType m_glyphTreeType;

		QPoint m_lastMousePosition;
		QMetaObject::Connection m_dataChangedConnection;
		QMetaObject::Connection m_selectionConnection;

		EditingMode m_editingMode;
		bool m_selectionEdited;
		bool m_allowMultiSelection;

		MinMaxGlyphTreeModel* m_model;
		QItemSelectionModel* m_selectionModel;

		bool m_animationEnabled;

		SynGlyphX::DefaultBaseImageProperties::Type m_baseImage;

		boost::bimap<SynGlyphX::DataMappingGlyphGraph::Label, int> m_labelToANTzNodeMap;
	};

} //namespace SynGlyphXANTz

#endif // ANTZSINGLEGLYPHTREEWIDGET_H
