#include "visualglyphpropertieswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include "labelline.h"
#include "colorconverter.h"
#include "glyph.h"
#include "coloralphawidget.h"
#include "xyzwidget.h"
#include "groupboxsinglewidget.h"
#include "glyphstructurewidget.h"

namespace SynGlyphX {

	VisualGlyphPropertiesWidget::VisualGlyphPropertiesWidget(bool addLockToScaleWidget, ChildOptions childOptions, QWidget *parent)
		: QWidget(parent),
		m_childrenSpinBox(NULL)
	{
		CreateWidgets(addLockToScaleWidget, childOptions);

		QPalette newPalette = palette();
		newPalette.setColor(QPalette::Disabled, QPalette::Window, newPalette.color(QPalette::Active, QPalette::Window));
		newPalette.setColor(QPalette::Disabled, QPalette::WindowText, newPalette.color(QPalette::Active, QPalette::WindowText));
		//newPalette.setColor(QPalette::Disabled, QPalette::Base, newPalette.color(QPalette::Active, QPalette::Base));
		newPalette.setColor(QPalette::Disabled, QPalette::Text, newPalette.color(QPalette::Active, QPalette::Text));
		//newPalette.setColor(QPalette::Disabled, QPalette::Button, newPalette.color(QPalette::Active, QPalette::Button));
		//newPalette.setColor(QPalette::Disabled, QPalette::ButtonText, newPalette.color(QPalette::Active, QPalette::ButtonText));
		setPalette(newPalette);
	}

	VisualGlyphPropertiesWidget::~VisualGlyphPropertiesWidget()
	{

	}

	QWidget* VisualGlyphPropertiesWidget::CreateChildrenWidget(ChildOptions childOptions) {

		QWidget* childWidget = new QWidget(this);
		QHBoxLayout* layout = new QHBoxLayout(childWidget);

		QLabel* label = new QLabel(tr("Number of Children:"), childWidget);
		layout->addWidget(label);

		m_childrenSpinBox = new QSpinBox(childWidget);

		if (childOptions & EnabledSpinBox) {
			m_childrenSpinBox->setMinimum(1);
			m_childrenSpinBox->setValue(1);
		}
		else {
			m_childrenSpinBox->setMinimum(0);
			m_childrenSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
			m_childrenSpinBox->setEnabled(false);
		}

		layout->addWidget(m_childrenSpinBox, 1);

		if (childOptions & AddChildrenButton) {
			QPushButton* addChildrenButton = new QPushButton(tr("Add Children"), childWidget);
			QObject::connect(addChildrenButton, SIGNAL(clicked()), this, SIGNAL(AddChildrenButtonClicked()));
			layout->addWidget(addChildrenButton);
		}

		childWidget->setLayout(layout);

		return childWidget;
	}

	void VisualGlyphPropertiesWidget::CreateWidgets(bool addLockToScaleWidget, ChildOptions childOptions) {

		QVBoxLayout* layout = new QVBoxLayout(this);

		m_glyphStructureWidget = new GlyphStructureWidget(this);

		QHBoxLayout* colorLayout = new QHBoxLayout(this);
		colorLayout->setContentsMargins(0, 0, 0, 0);

		m_colorWidget = new ColorAlphaWidget(this);
		//m_colorWidget->setContentsMargins(0, 0, 0, 0);
		GroupBoxSingleWidget* colorGroupBox = new GroupBoxSingleWidget(tr("Color"), m_colorWidget, this);

		colorLayout->addWidget(colorGroupBox);

		colorLayout->addStretch(1);

		m_translateWidget = new XYZWidget(false, this);
		m_translateWidget->SetRange(-10000.0, 10000.0);
		m_translateWidget->SetDecimal(4);
		m_translateWidget->setContentsMargins(0, 0, 0, 0);

		GroupBoxSingleWidget* translateGroupBox = new GroupBoxSingleWidget(tr("Position"), m_translateWidget, this);

		//Degree symbol gets displayed incorrectly unless we use QChar
		QString degreeSuffix = QString::fromStdWString(L"\u00b0");

		m_rotateWidget = new XYZWidget(false, this);
		m_rotateWidget->SetRange(-180.0, 180.0);
		m_rotateWidget->SetWrapping(true);
		m_rotateWidget->SetDecimal(5);
		m_rotateWidget->SetSuffix(degreeSuffix);
		m_rotateWidget->setContentsMargins(0, 0, 0, 0);

		GroupBoxSingleWidget* rotateGroupBox = new GroupBoxSingleWidget(tr("Rotation"), m_rotateWidget, this);

		m_scaleWidget = new XYZWidget(addLockToScaleWidget, this);
		m_scaleWidget->SetRange(0.000001, 1000.0);
		m_scaleWidget->SetDecimal(5);
		m_scaleWidget->setContentsMargins(0, 0, 0, 0);
		m_scaleWidget->SetSpinBoxesLocked(true);

		GroupBoxSingleWidget* scaleGroupBox = new GroupBoxSingleWidget(tr("Scale"), m_scaleWidget, this);

		m_rotateRateWidget = new XYZWidget(false, this);
		m_rotateRateWidget->SetRange(-100.0, 100.0);
		m_rotateRateWidget->SetDecimal(3);
		m_rotateRateWidget->setContentsMargins(0, 0, 0, 0);

		GroupBoxSingleWidget* rotateRateGroupBox = new GroupBoxSingleWidget(tr("Animation - Rotation"), m_rotateRateWidget, this);

		LabelLine* structureLine = new LabelLine(tr("Structural Properties"), this);
		LabelLine* mappableLine = new LabelLine(tr("Mappable Properties"), this);

		layout->addWidget(structureLine);
		layout->addWidget(m_glyphStructureWidget);
		layout->addWidget(mappableLine);
		layout->addLayout(colorLayout);
		layout->addWidget(translateGroupBox);
		layout->addWidget(rotateGroupBox);
		layout->addWidget(scaleGroupBox);
		layout->addWidget(rotateRateGroupBox);

		if ((childOptions & ShowOnBottom) || (childOptions & ShowOnTop)) {

			QWidget* childrenWidget = CreateChildrenWidget(childOptions);
			if (childOptions & ShowOnBottom) {

				LabelLine* childrenLine = new LabelLine(tr("Children"), this);
				layout->addWidget(childrenLine);
				layout->addWidget(childrenWidget);
			}
			else {

				layout->insertWidget(0, childrenWidget);
			}
		}

		layout->addStretch(1);
		setLayout(layout);
	}

	void VisualGlyphPropertiesWidget::SetWidgetFromGlyph(const Glyph& glyph, bool isNotRootNode) {

		m_glyphStructureWidget->SetWidgetFromGlyphGeometryAndTopology(glyph.GetStructure(), glyph.GetVirtualTopology());

		m_colorWidget->SetColor(ColorConverter::GlyphColor2QColor(glyph.GetColor(), glyph.GetTransparency()));

		m_translateWidget->SetEnabled(isNotRootNode, isNotRootNode, true, isNotRootNode);
		m_translateWidget->Set(glyph.GetPosition());
		m_rotateWidget->Set(glyph.GetRotation());
		m_scaleWidget->Set(glyph.GetScale());
		m_rotateRateWidget->Set(glyph.GetRotationRate());
	}

	void VisualGlyphPropertiesWidget::SetGlyphFromWidget(Glyph& glyph) {

		glyph.GetStructure() = m_glyphStructureWidget->GetGlyphGeometry();
		glyph.GetVirtualTopology() = m_glyphStructureWidget->GetVirtualTopology();

		QColor color = m_colorWidget->GetColor();
		glyph.GetColor() = ColorConverter::QColor2GlyphColor(color);
		glyph.GetTransparency() = color.alpha();

		glyph.GetPosition() = { { m_translateWidget->GetX(), m_translateWidget->GetY(), m_translateWidget->GetZ() } };
		glyph.GetRotation() = { { m_rotateWidget->GetX(), m_rotateWidget->GetY(), m_rotateWidget->GetZ() } };
		glyph.GetScale() = { { m_scaleWidget->GetX(), m_scaleWidget->GetY(), m_scaleWidget->GetZ() } };

		glyph.GetRotationRate() = { { m_rotateRateWidget->GetX(), m_rotateRateWidget->GetY(), m_rotateRateWidget->GetZ() } };
	}

	void VisualGlyphPropertiesWidget::SetNumberOfChildren(unsigned int numChildren) {

		if (m_childrenSpinBox != NULL) {
			m_childrenSpinBox->setValue(numChildren);
		}
	}

	unsigned int VisualGlyphPropertiesWidget::GetNumberOfChildren() const {

		if (m_childrenSpinBox == NULL) {
			return 0;
		}
		else {
			return static_cast<unsigned int>(m_childrenSpinBox->value());
		}
	}

} //namespace SynGlyphX
