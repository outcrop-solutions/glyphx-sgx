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

#ifndef ITEMFOCUSSELECTIONMODEL_H
#define ITEMFOCUSSELECTIONMODEL_H

#include "sgxglyphgui_global.h"
#include <QtCore/QItemSelectionModel>
#include <vector>

namespace SynGlyphX {

	class SGXGLYPHGUI_EXPORT ItemFocusSelectionModel : public QItemSelectionModel
	{
		Q_OBJECT

	public:
		enum FocusFlag {
			NoUpdate = 0x0,
			Clear = 0x1,
			Focus = 0x2,
			Unfocus = 0x4,
			Toggle = 0x8,
			ClearAndFocus = Clear | Focus
		};
		Q_DECLARE_FLAGS(FocusFlags, FocusFlag)

		ItemFocusSelectionModel(QAbstractItemModel* model, QObject *parent = nullptr);
		~ItemFocusSelectionModel();

		void ClearFocus();
		void SetFocus(const QModelIndex& index, FocusFlags command);
		void SetFocus(const QModelIndexList& indexes, FocusFlags command);

		const QModelIndexList& GetFocusList() const;

	signals:
		void FocusChanged(const QModelIndexList& focusList);

	private:
		QModelIndexList m_focusList;
	};

	Q_DECLARE_OPERATORS_FOR_FLAGS(ItemFocusSelectionModel::FocusFlags)

} //namespace SynGlyphX

#endif // ITEMFOCUSSELECTIONMODEL_H
