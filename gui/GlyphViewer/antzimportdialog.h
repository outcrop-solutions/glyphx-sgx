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

#ifndef ANTZIMPORTDIALOG_H
#define ANTZIMPORTDIALOG_H

#include <QtWidgets/QDialog>
#include "browselineedit.h"
#include <QtWidgets/QCheckBox>
#include "csvfilereader.h"

class ANTzImportDialog : public QDialog
{
	Q_OBJECT

public:
	ANTzImportDialog(QWidget *parent = 0);
	~ANTzImportDialog();

	QString GetANTzNodeFilename() const;
	QString GetANTzTagFilename() const;
	QString GetANTzChannelFilename() const;
	QString GetANTzChannelMapFilename() const;
	QString GetBaseImageFilename() const;
	QString GetOutputFilename() const;

public slots:
	virtual void accept();

private slots:
	void OnDefaultBaseImageCheckboxChanged(int newState);
	void OnNodeFileChanged(const QString& newText);

private:
	bool ValidateANTzCSVFile(const QString& filename, const SynGlyphX::CSVFileReader::CSVValues& headers, const QString& fieldName, bool filenameCanBeEmpty = false);
	bool ValidateBaseImageFile(const QString& filename);
	bool ValidateDataTransformFile(const QString& filename);

	SynGlyphX::BrowseLineEdit* m_antzNodeFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_antzTagFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_antzChannelFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_antzChannelMapFileLineEdit;
	QCheckBox* m_defaultBaseImageCheckBox;
	SynGlyphX::BrowseLineEdit* m_baseImageFileLineEdit;
	SynGlyphX::BrowseLineEdit* m_outputFileLineEdit;
};

#endif // ANTZIMPORTDIALOG_H
