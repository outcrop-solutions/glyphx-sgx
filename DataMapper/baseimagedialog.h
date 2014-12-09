#ifndef BASEIMAGEDIALOG_H
#define BASEIMAGEDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>
#include <boost/bimap.hpp>
#include "mapoptionswidget.h"
#include "baseimage.h"
#include "browselineedit.h"
#include "xyzwidget.h"
#include "doublesizewidget.h"

class BaseImageDialog : public QDialog
{
	Q_OBJECT

public:
	BaseImageDialog(bool enablePositionAndOrientation, QWidget *parent = 0);
	~BaseImageDialog();

	virtual void accept();

	void SetBaseImage(const SynGlyphX::BaseImage& baseImage);
	SynGlyphX::BaseImage GetBaseImage() const;

private:
	static const boost::bimap<int, SynGlyphX::Vector3> s_orientationPresets;
	static const std::vector<QString> s_orientationNames;

	QStackedWidget* m_baseImageOptionsStackedWidget;
	QComboBox* m_baseImageComboBox;
	MapOptionsWidget* m_downloadedMapOptionsWidget;
	SynGlyphX::BrowseLineEdit* m_userDefinedImageLineEdit;
	XYZWidget* m_positionWidget;
	QButtonGroup* m_orientationGroup;
	SynGlyphX::DoubleSizeWidget* m_worldSizeWidget;
};

#endif // BASEIMAGEDIALOG_H
