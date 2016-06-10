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

#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include "glyphvieweroptions.h"
#include "browselineedit.h"

#ifdef USE_ZSPACE
#include "zspaceoptionswidget.h"
#endif

class OptionsWidget : public QTabWidget
{
	Q_OBJECT

public:
	OptionsWidget(const GlyphViewerOptions& options, bool enableCacheOptions, QWidget *parent);
	~OptionsWidget();

	GlyphViewerOptions GetOptions() const;

private slots:
	void ClearCache();
	void SetToDefaultCacheDirectory();

private:
	void CreateCacheTab(bool enableCacheOptions);
	void Create3DTab();
	void CreateFilteringTab();
	void CreateUITab();

	void SetCacheValues(const GlyphViewerOptions& options);
	void Set3DValues(const GlyphViewerOptions& options);
	void SetFilteringValues(const GlyphViewerOptions& options);
	void SetUIValues(const GlyphViewerOptions& options);

	SynGlyphX::BrowseLineEdit* m_cacheDirectoryWidget;
	
	QCheckBox* m_hideSelectedGlyphsCheckbox;

#ifdef USE_ZSPACE
	SynGlyphX::ZSpaceOptionsWidget* m_zSpaceOptionsWidget;
#endif

	QCheckBox* m_showDownloadedImageErrorMessages;
	
	QCheckBox* m_showHUDAxisInfoObjectCheckBox;
	QComboBox* m_axisObjectLocationComboBox;
};

#endif // OPTIONSWIDGET_H
