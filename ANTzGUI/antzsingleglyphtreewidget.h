#ifndef ANTZSINGLEGLYPHTREEWIDGET_H
#define ANTZSINGLEGLYPHTREEWIDGET_H

#include "antzgui_global.h"
#include "antzwidget.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include "data/nptypes.h"
#include "glyphgraph.h"
#include "datamappingglyphgraph.h"
#include "minmaxglyphtreemodel.h"
#include <QtCore/QItemSelectionModel>

namespace SynGlyphXANTz {

	class ANTZGUI_EXPORT ANTzSingleGlyphTreeWidget : public ANTzWidget
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

		void AppendChild(const QModelIndex& parent, boost::shared_ptr<const SynGlyphX::Glyph> glyph, unsigned int numberOfChildren = 1);

		void ShowGlyph(bool show);

		public slots:
		void SetEditingMode(EditingMode mode);

	protected:
		virtual void initializeGL();
		virtual void BeforeDrawScene();
		virtual void AfterDrawScene();
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		virtual void mouseMoveEvent(QMouseEvent* event);

		bool IsRootNodeSelected() const;

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

		int GetChildIndexFromParent(pNPnode node) const;

		void DeleteRootGlyphNode();

		void CreateNewSubTree(pNPnode parent, const SynGlyphX::DataMappingGlyphGraph::const_iterator& minMaxGlyph);
		pNPnode CreateNodeFromTemplate(pNPnode parent, const SynGlyphX::DataMappingGlyphGraph::const_iterator& minMaxGlyph);
		void RebuildTree();

		void UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::DataMappingGlyphGraph::const_iterator& minMaxGlyph);
		void UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::Glyph& glyphTemplate);

		void ConnectDataChangedSignal();
		void DisconnectDataChangedSignal();

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
	};

} //namespace SynGlyphXANTz

#endif // ANTZSINGLEGLYPHTREEWIDGET_H
