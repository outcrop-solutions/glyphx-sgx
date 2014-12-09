#include "sizewidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

namespace SynGlyphX {

	SizeWidget::SizeWidget(bool showLockRatioCheckBox, QWidget *parent)
        : QWidget(parent),
		m_lockRatioCheckBox(nullptr),
		m_widthSpinBox(nullptr),
		m_heightSpinBox(nullptr),
		m_ratio(0.0)
    {
        QHBoxLayout* layout = new QHBoxLayout(this);
		//setContentsMargins(0, 0, 0, 0);
		//layout->setContentsMargins(0, 0, 0, 0);
		setLayout(layout);

		if (showLockRatioCheckBox) {

			m_lockRatioCheckBox = new QCheckBox(tr("Lock Ratio"), this);
			layout->addWidget(m_lockRatioCheckBox);
			QObject::connect(m_lockRatioCheckBox, &QCheckBox::clicked, this, &SizeWidget::OnLockRatioChanged);
		}
    }

    SizeWidget::~SizeWidget()
    {

    }

	void SizeWidget::AddSpinBoxes(QAbstractSpinBox* widthSpinBox, QAbstractSpinBox* heightSpinBox) {

		m_widthSpinBox = widthSpinBox;
		m_heightSpinBox = heightSpinBox;

		QLayout* currentLayout = layout();

		QLabel* widthLabel = new QLabel(tr("Width:"), this);
		QLabel* heightLabel = new QLabel(tr("Height:"), this);

		currentLayout->addWidget(widthLabel);
		currentLayout->addWidget(m_widthSpinBox);
		currentLayout->addWidget(heightLabel);
		currentLayout->addWidget(m_heightSpinBox);

		currentLayout->invalidate();
	}

	void SizeWidget::OnLockRatioChanged() {

		if (m_lockRatioCheckBox->isChecked()) {

			m_widthSpinBoxConnection = ConnectLockRatioToWidthSpinBox();
			m_heightSpinBoxConnection = ConnectLockRatioToHeightSpinBox();
			m_ratio = GetRatio();
		}
		else {

			QObject::disconnect(m_widthSpinBoxConnection);
			QObject::disconnect(m_heightSpinBoxConnection);
		}
	}

} //namespace SynGlyphX