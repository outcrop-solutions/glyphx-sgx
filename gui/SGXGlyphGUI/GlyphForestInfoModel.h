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

#include "sgxglyphgui_global.h"
#include <QtCore/QAbstractItemModel>
#include <array>
#include <unordered_map>
#include "csvfilereader.h"
#include <containers/ntree.hpp>

namespace SynGlyphX {

	class SGXGLYPHGUI_EXPORT GlyphForestInfoModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		//Order is tag, description, url in the below array
		typedef std::array<QString, 3> GlyphTextProperties;

		GlyphForestInfoModel(QObject *parent);
		~GlyphForestInfoModel();

		void ClearAndReset();
		void LoadGlyphForestInfoLegacy(const QString& nodeCSVFile, const QString& tagCSVFile);

		//Functions from QAbstractItemModel that need to be implemented
		int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 1; }
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
		QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const override;
		QModelIndex	parent(const QModelIndex& index) const override;
		int	rowCount(const QModelIndex& parent = QModelIndex()) const override;
		Qt::ItemFlags flags(const QModelIndex& index) const override;
		//QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

		QModelIndex IndexFromCSVID(int id) const;

		bool OpenURLs(const QModelIndexList& indexList);

		const GlyphTextProperties& GetGlyphTextProperties(const QModelIndex& index) const;

	private:
		typedef stlplus::ntree<GlyphTextProperties> GlyphInfoTree;
		typedef GlyphInfoTree::const_iterator GlyphInfoConstIterator;
		typedef GlyphInfoTree::iterator GlyphInfoIterator;
		typedef stlplus::ntree_node<GlyphTextProperties> GlyphInfoNode;
		
		void Clear();
		unsigned int FindHeaderIndex(const CSVFileHandler::CSVValues& headers, const std::wstring& header) const;
		void ReadTagCSV(const QString& filename, std::unordered_map<unsigned long, GlyphTextProperties>& id2GlyphTextProperties);
		QString GlyphForestInfoModel::GetTag(const std::wstring& title) const;
		QString GlyphForestInfoModel::GetURL(const std::wstring& title) const;

		std::vector<GlyphInfoTree> m_glyphs;
		std::unordered_map<unsigned long, GlyphInfoIterator> m_csvID2GlyphNode;
	};

} //namespace SynGlyphX