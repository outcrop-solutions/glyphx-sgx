#ifndef BASEIMAGEDIALOG_H
#define BASEIMAGEDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QComboBox>
#include "mapoptionswidget.h"
#include "baseimage.h"
#include "browselineedit.h"

class BaseImageDialog : public QDialog
{
	Q_OBJECT

public:
	BaseImageDialog(QWidget *parent = 0);
	~BaseImageDialog();

	virtual void accept();

	void SetBaseImage(const SynGlyphX::BaseImage& baseImage);
	SynGlyphX::BaseImage GetBaseImage() const;

private:
	QStackedWidget* m_baseImageOptionsStackedWidget;
	QComboBox* m_baseImageComboBox;
	MapOptionsWidget* m_downloadedMapOptionsWidget;
	SynGlyphX::BrowseLineEdit* m_userDefinedImageLineEdit;
};

#endif // BASEIMAGEDIALOG_H
