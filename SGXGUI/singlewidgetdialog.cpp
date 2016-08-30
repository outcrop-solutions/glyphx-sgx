#include "singlewidgetdialog.h"
#include <QtWidgets/QVBoxLayout>

namespace SynGlyphX {

	SingleWidgetDialog::SingleWidgetDialog(QDialogButtonBox::StandardButtons buttons, QWidget* widget, QWidget *parent)
		: QDialog(parent),
		m_widget(widget)
	{
		m_widget->setParent(this);

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->addWidget(widget);

		QDialogButtonBox* buttonBox = new QDialogButtonBox(buttons, this);
		layout->addWidget(buttonBox);

		setLayout(layout);

		QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	}

	SingleWidgetDialog::~SingleWidgetDialog()
	{

	}

} //namespace SynGlyphX