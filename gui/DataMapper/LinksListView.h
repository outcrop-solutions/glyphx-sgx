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
#ifndef LINKSLISTVIEW_H
#define LINKSLISTVIEW_H

#include "DataMappingListView.h"
#include "sharedactionlist.h"

namespace SynGlyphX {
	class DataTransformModel;
}

class LinksListView : public DataMappingListView
{
	Q_OBJECT

public:
	LinksListView(SynGlyphX::DataTransformModel* dataTransformModel, QWidget *parent);
	~LinksListView();

	const SynGlyphX::SharedActionList& GetSharedActions();

protected:
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
	void ShowEditDialog(const QModelIndex &index) override;

signals:
	void editLink(int row);

private slots:
	void RemoveLink();
	void EditLink();

private:
	void EnableActions();

	SynGlyphX::DataTransformModel* m_dataTransformModel;

	SynGlyphX::SharedActionList m_sharedActions;
	QAction* m_removeLinkAction;
	QAction* m_propertiesAction;
	
};

#endif // LINKSLISTVIEW_H
