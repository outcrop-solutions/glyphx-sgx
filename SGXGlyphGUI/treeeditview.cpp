#include "treeeditview.h"
#include <QtGui/QClipboard>
#include "glyphbuilderapplication.h"
#include "linklessgraphmimedata.h"
#include <QtCore/QMimeData>
#include "datamappingglyphgraph.h"

namespace SynGlyphX {

	TreeEditView::TreeEditView(QWidget *parent)
		: TreeView(parent)
	{
		CreateEditActions();

		QObject::connect(SynGlyphX::GlyphBuilderApplication::clipboard(), &QClipboard::dataChanged, this, &TreeEditView::OnClipboardDataChanged);
	}

	TreeEditView::~TreeEditView()
	{

	}

	const SynGlyphX::SharedActionList& TreeEditView::GetEditActions() const {

		return m_editActions;
	}

	void TreeEditView::setModel(QAbstractItemModel* model) {

		TreeView::setModel(model);

		QObject::connect(model, &QAbstractItemModel::rowsRemoved, this, &TreeEditView::OnRowsInsertedOrRemoved);
		QObject::connect(model, &QAbstractItemModel::rowsInserted, this, &TreeEditView::OnRowsInsertedOrRemoved);
	}

	void TreeEditView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

		SynGlyphX::TreeView::selectionChanged(selected, deselected);
	}

	void TreeEditView::CreateEditActions() {

		m_cutAction = m_editActions.AddAction(tr("Cut"), QKeySequence::Cut);
		QObject::connect(m_cutAction, &QAction::triggered, this, [&, this](){ CopyToClipboard(true, true); });

		m_copyWithChildrenAction = m_editActions.AddAction(tr("Copy With Children"), QKeySequence::Copy);
		QObject::connect(m_copyWithChildrenAction, &QAction::triggered, this, [&, this](){ CopyToClipboard(true, false); });

		m_copyAction = m_editActions.AddAction(tr("Copy (Object Only)"));
		QObject::connect(m_copyAction, &QAction::triggered, this, [&, this](){ CopyToClipboard(false, false); });

		m_pasteAsChildAction = m_editActions.AddAction(tr("Paste As Child"), QKeySequence::Paste);
		QObject::connect(m_pasteAsChildAction, &QAction::triggered, this, [&, this](){ PasteFromClipboard(true); });

		m_pasteAction = m_editActions.AddAction(tr("Paste (Overwrite)"));
		QObject::connect(m_pasteAction, &QAction::triggered, this, [&, this](){ PasteFromClipboard(false); });

		m_editActions.AddSeparator();

		m_deleteAction = m_editActions.AddAction(tr("Delete"), QKeySequence::Delete);
		QObject::connect(m_deleteAction, &QAction::triggered, this, &TreeEditView::DeleteSelectedAndSelectNewIndex);

		m_deleteChildrenAction = m_editActions.AddAction(tr("Delete Children"));
		QObject::connect(m_deleteChildrenAction, &QAction::triggered, this, &TreeEditView::DeleteChildrenFromSelected);

		m_editActions.EnableActions(false);
	}

	void TreeEditView::OnClipboardDataChanged() {

		const QModelIndexList& selected = selectionModel()->selectedIndexes();
		bool enablePasteActions = (selected.count() == 1) && DoesClipboardHaveGlyph();

		m_pasteAction->setEnabled(enablePasteActions);
		m_pasteAsChildAction->setEnabled(enablePasteActions);
	}

	bool TreeEditView::DoesClipboardHaveGlyph() const {

		QClipboard* globalClipboard = SynGlyphX::GlyphBuilderApplication::clipboard();
		const QMimeData* mimeData = globalClipboard->mimeData();

		if (mimeData != nullptr) {

			return mimeData->hasFormat(SynGlyphX::LinklessGraphMimeData::s_format);
		}

		return false;
	}

	void TreeEditView::CopyToClipboard(bool includeChildren, bool removeFromTree) {

		const QModelIndexList& selected = selectionModel()->selectedIndexes();
		if (!selected.isEmpty()) {

			const QModelIndex& index = selected.last();
			if (!index.isValid()) {

				return;
			}

			QClipboard* globalClipboard = SynGlyphX::GlyphBuilderApplication::clipboard();
			QMimeData* mimeData = new QMimeData();

			SynGlyphX::LinklessGraphMimeData::ConvertToMimeData(GetGraphForCopyToClipboard(index, includeChildren), mimeData);

			if (removeFromTree) {

				DeleteSelectedAndSelectNewIndex();
			}

			globalClipboard->setMimeData(mimeData);
		}
	}

	void TreeEditView::PasteFromClipboard(bool addAsChild) {

		const QModelIndexList& selected = selectionModel()->selectedIndexes();
		if (!selected.isEmpty()) {

			const QModelIndex& index = selected.last();
			QClipboard* globalClipboard = SynGlyphX::GlyphBuilderApplication::clipboard();
			const QMimeData* mimeData = globalClipboard->mimeData();
			if ((mimeData != nullptr) && (mimeData->hasFormat(SynGlyphX::LinklessGraphMimeData::s_format))) {

				SynGlyphX::DataMappingGlyphGraph subgraph = SynGlyphX::LinklessGraphMimeData::ConvertToLinklessGraph(mimeData);

				if (DoInputBindingsNeedToBeClearedBeforePaste()) {

					subgraph.ClearAllInputBindings();
				}

				if (addAsChild) {

					AddGlyphsAsChildren(index, subgraph);
				}
				else {

					OverwriteGlyph(index, subgraph);
				}
			}
		}
	}

	void TreeEditView::OnRowsInsertedOrRemoved() {

		EnableActions(selectionModel()->selection());
	}

	bool TreeEditView::DoInputBindingsNeedToBeClearedBeforePaste() {

		return false;
	}

	void TreeEditView::DeleteSelectedAndSelectNewIndex() {

		DepthSortedModelIndexes sortedIndexList = GetSelectedIndexListForDeletion();
		if (!sortedIndexList.empty()) {

			auto lastInList = sortedIndexList.rbegin();
			QModelIndex newSelectedIndex = lastInList->second.parent();
			int rootIndexCount = model()->rowCount();
			if (!newSelectedIndex.isValid() && rootIndexCount > 1) {

				//This only works to select a new root index when a single item is deleted.  That is fine for now but may need to be fixed in the future.
				int row = lastInList->second.row();
				if (row == 0) {

					newSelectedIndex = model()->index(1, 0);
				}
				else {

					newSelectedIndex = model()->index(row - 1, 0);
				}
			}

			if (newSelectedIndex.isValid()) {

				selectionModel()->select(newSelectedIndex, QItemSelectionModel::ClearAndSelect);
			}
			
			for(const auto& index : sortedIndexList) {

				if (CanIndexBeDeleted(index.second)) {

					model()->removeRow(index.second.row(), index.second.parent());
				}
			}
		}
	}

	void TreeEditView::DeleteChildrenFromSelected() {

		DepthSortedModelIndexes sortedIndexList = GetSelectedIndexListForDeletion();
		if (!sortedIndexList.empty()) {

			QAbstractItemModel* treeModel = model();
			for (const auto& index : sortedIndexList) {

				treeModel->removeRows(0, treeModel->rowCount(index.second), index.second);
			}
		}
	}

	TreeEditView::DepthSortedModelIndexes TreeEditView::GetSelectedIndexListForDeletion() const {

		QModelIndexList selectedItems = selectionModel()->selectedIndexes();
		DepthSortedModelIndexes persistentIndexes;
		for (QModelIndex& index : selectedItems) {

			unsigned int depth = 0;
			QModelIndex ancestor = index.parent();
			while (ancestor.isValid()) {

				ancestor = ancestor.parent();
				++depth;
			}
			persistentIndexes.insert(std::pair<unsigned int, QPersistentModelIndex>(depth, QPersistentModelIndex(index)));
		}

		return persistentIndexes;
	}

	bool TreeEditView::CanIndexBeDeleted(const QModelIndex& index) const {

		return true;
	}

} //namespace SynGlyphX