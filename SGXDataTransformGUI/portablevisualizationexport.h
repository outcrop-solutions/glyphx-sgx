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

#ifndef PORTABLEVISUALIZATIONEXPORT_H
#define PORTABLEVISUALIZATIONEXPORT_H

#include "datatransformmapping.h"
#include <QtCore/QString>

namespace SynGlyphX {

	class PortableVisualizationExport
	{
	public:
		enum Platform {
			Windows,
			WindowsZSpace,
			Mac,
			Linux,
			Web
		};

		PortableVisualizationExport();
		~PortableVisualizationExport();

		static void SetupSourceDirectories();
		static const QString& GetSourceDirectory(Platform platform);

	private:
		static std::unordered_map<Platform, QString> s_sourceDirectories;
	};

} //namespace SynGlyphX

#endif // PORTABLEVISUALIZATIONEXPORT_H
