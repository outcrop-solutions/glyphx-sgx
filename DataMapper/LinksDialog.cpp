#include "LinksDialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
//#include <QtCore/QFile>
//#include <QtGui/QImage>
//#include <boost/assign/list_of.hpp>
//#include <boost/bimap/list_of.hpp>
//#include "groupboxsinglewidget.h"
//#include "downloadedmapproperties.h"
//#include "userdefinedbaseimageproperties.h"
//#include "colorconverter.h"

//const LinksDialog::PresetMap LinksDialog::s_presets = 
//	{ PositionOrientation(SynGlyphX::Vector3({ { 0.0, 0.0, 0.0 } }), SynGlyphX::Vector3({ { 0.0, 0.0, 0.0 } })),
//	PositionOrientation(SynGlyphX::Vector3({ { 0.0, 0.0, 180.0 } }), SynGlyphX::Vector3({ { 180.0, 0.0, 0.0 } })),
//	PositionOrientation(SynGlyphX::Vector3({ { -180.0, 0.0, 90.0 } }), SynGlyphX::Vector3({ { 90.0, 90.0, 180.0 } })),
//	PositionOrientation(SynGlyphX::Vector3({ { 180.0, 0.0, 90.0 } }), SynGlyphX::Vector3({ { 90.0, 270.0, 180.0 } })),
//	PositionOrientation(SynGlyphX::Vector3({ { 0.0, 90.0, 90.0 } }), SynGlyphX::Vector3({ { 90.0, 180.0, 180.0 } })) };
//
//const std::vector<std::string> LinksDialog::s_presetNames = { "Up", "Down", "Left", "Right", "Front" };

LinksDialog::LinksDialog(QWidget *parent)
	: QDialog(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	mainLayout->addStretch(1);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &LinksDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &LinksDialog::reject);

	setLayout(mainLayout);
}

LinksDialog::~LinksDialog()
{

}

void LinksDialog::accept() {

	QDialog::accept();
}

