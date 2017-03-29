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
#ifndef DATASTATSMODEL_H
#define DATASTATSMODEL_H

#include "sgxdatatransformgui_global.h"
#include <QtCore/QAbstractTableModel>
#include <QtSql/QSqlQuery>
#include <boost/uuid/uuid.hpp>
#include "dataengineconnection.h"
#include "inputtable.h"

namespace SynGlyphX {

	class SGXDATATRANSFORMGUI_EXPORT DataStatsModel : public QAbstractTableModel
	{
		Q_OBJECT

	public:
		typedef QList<QStringList> TableStats;

		DataStatsModel(const SynGlyphX::InputTable& table, const TableStats& tableStats, QObject *parent = 0);
		~DataStatsModel();

		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

		virtual Qt::ItemFlags flags(const QModelIndex & index) const;
		virtual QStringList mimeTypes() const;
		virtual QMimeData* mimeData(const QModelIndexList& indexes) const;
		bool setFormattedData(const QModelIndex & index, const QVariant &value);

	private:
		//void GenerateStats(const boost::uuids::uuid& databaseId, const QString& tableName, QString filename, DataEngine::DataEngineConnection &dec);
		//void GenerateStats(DataEngine::DataEngineConnection *dec);

		QList<QVariant::Type> m_fieldTypes;
		SynGlyphX::InputTable m_table;
		TableStats m_stats;
	};

} //namespace SynGlyphX

#endif // DATASTATSMODEL_H
