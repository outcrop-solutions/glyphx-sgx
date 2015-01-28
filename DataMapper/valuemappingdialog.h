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

#ifndef VALUEMAPPINGDIALOG_H
#define VALUEMAPPINGDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHBoxLayout>
#include "colorbutton.h"
#include "valuemappingfunction.h"

class ValueMappingDialog : public QDialog
{
	Q_OBJECT

public:
	/*enum Output {
		
		Numeric = 0x0,
		Color = 0x1
	};

	enum Input {

		Numeric = 0x0,
		Text = 0x1,
		Range = 0x2
	};*/

	ValueMappingDialog(QWidget *parent);
	~ValueMappingDialog();

	//void SetDialogFromMapping(SynGlyphX::ValueMappingData<OutputType, InputType, KeyType>::ConstSharedPtr mapping);
	//SynGlyphX::ValueMappingData<OutputType, InputType, KeyType>::SharedPtr GetMappingFromDialog() const;

protected:
	void SetupDialog();
	void SetupDialogLayout(QWidget* defaultWidget, QHBoxLayout* editLayout);

	QDoubleSpinBox* m_defaultDoubleWidget;
	SynGlyphX::ColorButton* m_defaultColorWidget;

	QTableWidget* m_table;
};

#endif // VALUEMAPPINGDIALOG_H
