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
#ifndef SYNGLYPHX_ABSTRACTTREEMODEL_H
#define SYNGLYPHX_ABSTRACTTREEMODEL_H

#include "sgxgui_global.h"
#include <QtCore/QAbstractItemModel>

namespace SynGlyphX {

	class SGXGUI_EXPORT AbstractTreeModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		AbstractTreeModel(QObject *parent);
		~AbstractTreeModel();

		//At some point this should have a default implementation, but should still be overrideable
		virtual unsigned int GetMaxDepth() const = 0;

	private:

	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_ABSTRACTTREEMODEL_H
