#include "sizewidget.h"
#include <QtWidgets/QLayout>

namespace SynGlyphX {

	SizeWidget::SizeWidget(bool showLockRatioCheckBox, QWidget *parent)
		: HorizontalFormWidget(parent),
		m_lockRatioCheckBox(nullptr),
		m_widthSpinBox(nullptr),
		m_heightSpinBox(nullptr),
		m_ratio(0.0)
    {
		if (showLockRatioCheckBox) {

			m_lockRatioCheckBox = new QCheckBox(tr("Lock Ratio"), this);
			layout()->addWidget(m_lockRatioCheckBox);
			QObject::connect(m_lockRatioCheckBox, &QCheckBox::clicked, this, &SizeWidget::OnLockRatioChanged);
		}
    }

    SizeWidget::~SizeWidget()
    {

    }

	void SizeWidget::AddSpinBoxes(QAbstractSpinBox* widthSpinBox, QAbstractSpinBox* heightSpinBox, const QString& widthLabel, const QString& heightLabel) {

		m_widthSpinBox = widthSpinBox;
		m_heightSpinBox = heightSpinBox;

		AddWidget(widthLabel, m_widthSpinBox);
		AddWidget(heightLabel, m_heightSpinBox);
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