#include "kmltocsvdialog.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>

KMLToCSVDialog::KMLToCSVDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    QGridLayout* gridLayout = new QGridLayout(this);

    QLabel* kmlLabel = new QLabel(tr("KML/KMZ file:"), this);
    gridLayout->addWidget(kmlLabel, 0, 0);
    m_inputKML = new QLineEdit(this);
    gridLayout->addWidget(m_inputKML, 0, 1);
    QPushButton* kmlFileButton = new QPushButton("...", this);
    gridLayout->addWidget(kmlFileButton, 0, 2);

    QLabel* glyphLabel = new QLabel(tr("Glyph file:"), this);
    gridLayout->addWidget(glyphLabel, 1, 0);
    m_inputGlyph = new QLineEdit(this);
    gridLayout->addWidget(m_inputGlyph, 1, 1);
    QPushButton* glyphFileButton = new QPushButton("...", this);
    gridLayout->addWidget(glyphFileButton, 1, 2);

    gridLayout->setColumnStretch(1, 1);

    verticalLayout->addLayout(gridLayout);

    QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Close, this);

    verticalLayout->addWidget(dialogButtonBox);

    setLayout(verticalLayout);
}

KMLToCSVDialog::~KMLToCSVDialog()
{

}
