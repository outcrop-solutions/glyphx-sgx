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

#ifndef MINMAXGLYPHTREEMODEL_H
#define MINMAXGLYPHTREEMODEL_H

#include "antzsingleglyphgui_global.h"
#include <QtCore/QAbstractItemModel>
#include "datamappingglyphgraph.h"
#include "propertyupdate.h"

namespace SynGlyphXANTz {

	class ANTZSINGLEGLYPHGUI_EXPORT MinMaxGlyphTreeModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		const int LabelRole = Qt::UserRole;

		enum GlyphType {
			Min,
			Max
		};

		MinMaxGlyphTreeModel(QObject *parent);
		~MinMaxGlyphTreeModel();

		void SetMinMaxGlyphTree(SynGlyphX::DataMappingGlyphGraph::SharedPtr minMaxGlyphTree);
		SynGlyphX::DataMappingGlyphGraph::ConstSharedPtr GetMinMaxGlyphTree() const;

		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
		virtual QModelIndex	parent(const QModelIndex& index) const;
		virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual Qt::ItemFlags flags(const QModelIndex& index) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		virtual Qt::DropActions supportedDropActions() const;
		virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

		void AppendChild(const QModelIndex& parent, const SynGlyphX::DataMappingGlyph& glyph, unsigned int numberOfChildren = 1);

		void UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyph& glyph, SynGlyphX::PropertyUpdates updates = SynGlyphX::UpdateAll);
		void UpdateGlyphs(const QModelIndexList& indexList, const SynGlyphX::DataMappingGlyph& glyph, SynGlyphX::PropertyUpdates updates = SynGlyphX::UpdateAll);

		void UpdateGlyph(const QModelIndex& index, GlyphType type, const SynGlyphX::Glyph& glyph, SynGlyphX::PropertyUpdates updates = SynGlyphX::UpdateAll);
		void UpdateGlyphs(const QModelIndexList& indexList, GlyphType type, const SynGlyphX::Glyph& glyph, SynGlyphX::PropertyUpdates updates = SynGlyphX::UpdateAll);

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator GetMinMaxGlyph(const QModelIndex& index) const;

		virtual QStringList mimeTypes() const;
		virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
		virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

		bool LoadFromFile(const QString& filename);
		void SaveToTemplateFile(const QString& filename) const;
		bool SaveToCSV(const QString& filename, bool writeMaxGlyph = true);

		bool IsClipboardEmpty() const;
		SynGlyphX::DataMappingGlyphGraph::GlyphIterator GetClipboardGlyph() const;
		void CopyToClipboard(const QModelIndex& index, bool removeFromTree = false);

		void RepaceModelWithDefaultGlyphTree();
		void ResetRootMinMaxPositionXY();

		SynGlyphX::DataMappingGlyphGraph::Label GetLabel(const QModelIndex& index) const;

		QModelIndex GetIndexFromLabel(SynGlyphX::DataMappingGlyphGraph::Label label) const;

		static bool GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right);

	private:
		QVariant GetDisplayData(const QModelIndex& index) const;

		void UpdateGlyphMin(const QModelIndex& index, const SynGlyphX::Glyph& glyph, SynGlyphX::PropertyUpdates updates);
		void UpdateGlyphMax(const QModelIndex& index, const SynGlyphX::Glyph& glyph, SynGlyphX::PropertyUpdates updates);

		bool IsValidCSVFile(const QString& filename) const;
		bool IsRootGlyph(const SynGlyphX::DataMappingGlyphGraph::GlyphIterator& glyph) const;
		unsigned int GetBranchLevel(const QModelIndex& index) const;

		static SynGlyphX::DataMappingGlyphGraph::GlyphIterator GetIteratorFromIndex(const QModelIndex& index);

		SynGlyphX::DataMappingGlyphGraph::SharedPtr m_minMaxGlyphTree;
		SynGlyphX::DataMappingGlyphGraph::GlyphIterator m_clipboardGlyph;

		std::unordered_map<SynGlyphX::DataMappingGlyphGraph::Label, QPersistentModelIndex> m_labelToIndexMap;
	};

} //namespace SynGlyphXANTz

#endif // MINMAXGLYPHTREEMODEL_H
