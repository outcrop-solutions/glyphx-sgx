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

#ifndef BASEIMAGEDIALOG_H
#define BASEIMAGEDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtCore/QSignalMapper>
#include "mapoptionswidget.h"
#include "baseimage.h"
#include "browselineedit.h"
#include "xyzwidget.h"
#include "doublesizewidget.h"
#include "defaultbaseimagescombobox.h"
#include "colorbutton.h"

class BaseImageDialog : public QDialog
{
	Q_OBJECT

public:
	BaseImageDialog(bool enablePositionAndOrientation, bool showDownloadMapOptions, QWidget *parent = 0);
	~BaseImageDialog();

	virtual void accept();

	void SetBaseImage(const SynGlyphX::BaseImage& baseImage);
	SynGlyphX::BaseImage GetBaseImage() const;

private slots:
	void PresetButtonClicked(int id);
	void BaseImageTypeChanged(int type);
	void OnUseImageSizeRatioInWorldSize();

private:
	bool ValidateUserImageFilename(const QString& userImageFilename);

	typedef std::pair<SynGlyphX::Vector3, SynGlyphX::Vector3> PositionOrientation;
	typedef std::vector<PositionOrientation> PresetMap;

	static const PresetMap s_presets;
	static const std::vector<std::string> s_presetNames;

	QStackedWidget* m_baseImageOptionsStackedWidget;
	QComboBox* m_baseImageComboBox;
	MapOptionsWidget* m_downloadedMapOptionsWidget;
	SynGlyphX::BrowseLineEdit* m_userDefinedImageLineEdit;
	SynGlyphX::DefaultBaseImagesComboBox* m_defaultImagesComboBox;
	
	SynGlyphX::XYZWidget* m_positionWidget;
	SynGlyphX::XYZWidget* m_orientationWidget;
	QSignalMapper* m_presetButtonSignalMapper;
	QPushButton* m_setWorldSizeRatioToImageButton;
	SynGlyphX::DoubleSizeWidget* m_worldSizeWidget;

	SynGlyphX::IntSizeWidget* m_gridLinesCountsWidget;
	SynGlyphX::ColorButton* m_gridLinesColorButton;
};

#endif // BASEIMAGEDIALOG_H
