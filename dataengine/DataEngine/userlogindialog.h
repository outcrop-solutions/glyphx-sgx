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
#ifndef USERLOGINDIALOG_H
#define USERLOGINDIALOG_H

#include "sgxgui_global.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include "passwordlineedit.h"
#include "dataengineconnection.h"

class QLabel;

namespace DataEngine {

	class DATAENGINE UserLoginDialog : public QFrame
	{
		Q_OBJECT

	public:
		UserLoginDialog(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent);
		~UserLoginDialog();

		bool Login();

		void Clear();

	signals:
		void LoginActivated();

	private:
		QPushButton* loginButton;
		QLineEdit* m_usernameLineEdit;
		QLineEdit* m_passwordLineEdit;
		QCheckBox *stayLoggedInCheckBox;

		std::shared_ptr<DataEngine::DataEngineConnection> m_dataEngineConnection;
	};
}

#endif // USERLOGINDIALOG_H
