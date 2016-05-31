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
#ifndef SYNGLYPHX_ELASTICLISTMODEL_H
#define SYNGLYPHX_ELASTICLISTMODEL_H

#include "sgxgui_global.h"
#include <QtCore/QAbstractTableModel>

namespace SynGlyphX {

	class SGXGUI_EXPORT ElasticListModel : public QAbstractTableModel
	{
		Q_OBJECT

	public:
		typedef std::pair<QVariant, unsigned long> DataWithCount;
		typedef QList<DataWithCount> Data;

		static const int RawDataRole = Qt::UserRole;

		ElasticListModel(QObject *parent);
		~ElasticListModel();

		void ResetData(const Data& data);

		virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

		static QString ConvertDoubleToRawString(double value);
		static QString ConvertQVariantToRawString(const QVariant& value);

	private:
		float total = 0.0;
		bool noTotal = true;
		QStringList m_rawData;
		QStringList m_formattedData;
		QList<qulonglong> m_counts;
		QStringList m_percentage;
	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_ELASTICLISTMODEL_H
