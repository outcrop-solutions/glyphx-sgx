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

#ifndef INPUTFIELDDATA_H
#define INPUTFIELDDATA_H

#include <QtCore/QVariant>
#include <boost/shared_ptr.hpp>

namespace SynGlyphX {

	class InputFieldData
	{
	public:
		typedef boost::shared_ptr<InputFieldData> SharedPtr;
		typedef boost::shared_ptr<const InputFieldData> ConstSharedPtr;

		InputFieldData(const QVariantList& data, double min = 0.0, double max = 0.0);
		~InputFieldData();

		const QVariantList& GetData() const;
		double GetMin() const;
		double GetMax() const;
		double GetMaxMinDifference() const;

	private:
		double m_min;
		double m_max;
		QVariantList m_data;
		double m_maxMinDifference;
	};

} //namespace SynGlyphX

#endif // INPUTFIELDDATA_H
