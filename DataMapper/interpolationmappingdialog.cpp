#include "interpolationmappingdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>

InterpolationMappingDialog::InterpolationMappingDialog(QWidget *parent)
	: QDialog(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &InterpolationMappingDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &InterpolationMappingDialog::reject);

	setLayout(mainLayout);

	setWindowTitle(tr("Edit Interpolation Mapping Properties"));
}

InterpolationMappingDialog::~InterpolationMappingDialog()
{

}

void InterpolationMappingDialog::SetDialogFromMapping(SynGlyphX::InterpolationMappingData::ConstSharedPtr mapping) {


}

SynGlyphX::InterpolationMappingData::SharedPtr InterpolationMappingDialog::GetMappingFromDialog() const {


}