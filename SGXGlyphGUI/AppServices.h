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

class QUndoStack;
class QItemSelectionModel;
class TreeSelection;
namespace SynGlyphX {
	class MainWindow;
	class CommandService;

	enum class TransactionType {
		ChangeTree
	};

	class SGXGLYPHGUI_EXPORT AppServices {
	public:
		AppServices(MainWindow* mw);
		virtual void BeginTransaction(const char* name, TransactionType t) = 0;
		virtual void EndTransaction() = 0 ;
		void SetModified(bool m = true);
		void ShowWarningDialog(const QString& msg);
		void ClearUndoStack();
		QUndoStack* GetUndoStack();
		virtual QItemSelectionModel* GetTreeViewSelectionModel() { return nullptr; }
		//calling object is responsible for deleting this, dont want to polute global object inteface with std::shared_ptr
		virtual TreeSelection* CreateTreeSelection() { return nullptr; } 
		virtual void ApplyTreeSelection(const TreeSelection& selection) {}
		virtual ~AppServices() {}
	protected:
		MainWindow* m_window;
	};
}