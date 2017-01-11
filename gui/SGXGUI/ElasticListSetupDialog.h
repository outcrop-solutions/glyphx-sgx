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
#ifndef ELASTICLISTSETUPDIALOG_H
#define ELASTICLISTSETUPDIALOG_H

#include "sgxgui_global.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/QTabWidget>
#include "ElasticListSetupTab.h"

namespace SynGlyphX {

	class SGXGUI_EXPORT ElasticListSetupDialog : public QDialog
	{
		Q_OBJECT

	public:
		ElasticListSetupDialog(QWidget *parent = 0);
		~ElasticListSetupDialog();

		void SetFormattedNames(std::map<std::wstring, std::wstring> formattedNames) { m_formattedNames = formattedNames; }
		void PopulateTabs(std::map<std::wstring, std::vector<std::wstring>> elastic, std::map<std::wstring, std::vector<std::wstring>> available);
		std::map<std::wstring, std::vector<std::wstring>> SaveElasticListSelections();

	private:
		QTabWidget* m_tableTabs;
		std::map<std::wstring, std::wstring> m_formattedNames;
		std::map<std::wstring, ElasticListSetupTab*> m_elasticTabs;

	};

} //namespace SynGlyphX

#endif // ELASTICLISTSETUPDIALOG_H
