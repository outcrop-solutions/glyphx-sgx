#include "singleglyphwidget.h"
#include <QtWidgets/QFormLayout>

SingleGlyphWidget::SingleGlyphWidget(ChildOptions childOptions, QWidget *parent)
    : QWidget(parent),
    m_childrenSpinBox(NULL)
{
    CreateWidgets(childOptions);
}

SingleGlyphWidget::~SingleGlyphWidget()
{

}

QWidget* SingleGlyphWidget::CreateChildrenWidget(ChildOptions childOptions) {

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

void SingleGlyphWidget::CreateWidgets(ChildOptions childOptions) {
    
    QFormLayout* form = new QFormLayout(this);

    m_geometryShapeComboBox = new QComboBox(this);

	for (auto shape : SynGlyphX::GlyphStructuralProperties::s_shapeNames.left) {

		m_geometryShapeComboBox->addItem(QString::fromStdWString(shape.second));
	}

    m_geometrySurfaceComboBox = new QComboBox(this);
	for (auto surface : SynGlyphX::GlyphStructuralProperties::s_surfaceNames.left) {

		m_geometrySurfaceComboBox->addItem(QString::fromStdWString(surface.second));
	}

    m_topologyComboBox = new QComboBox(this);
    for (auto virtualTopology : SynGlyphX::GlyphStructuralProperties::s_virtualTopologyNames.left) {

		m_topologyComboBox->addItem(QString::fromStdWString(virtualTopology.second));
    }

    QHBoxLayout* colorAndRatioLayout = new QHBoxLayout(this);
    colorAndRatioLayout->setContentsMargins(0, 0, 0, 0);

	m_colorWidget = new SynGlyphX::ColorAlphaWidget(this);
	//m_colorWidget->setContentsMargins(0, 0, 0, 0);
	SynGlyphX::GroupBoxSingleWidget* colorGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Color"), m_colorWidget, this);

    colorAndRatioLayout->addWidget(colorGroupBox);

	colorAndRatioLayout->addStretch(1);

	/*m_ratioSpinBox = new QDoubleSpinBox(this);
	m_ratioSpinBox->setSingleStep(0.05);
	m_ratioSpinBox->setDecimals(2);
	m_ratioGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Torus Ratio"), m_ratioSpinBox, this); 

    colorAndRatioLayout->addWidget(m_ratioGroupBox);*/

	m_translateWidget = new XYZWidget(false, this);
	m_translateWidget->SetRange(-10000.0, 10000.0);
	m_translateWidget->SetDecimal(4);
	m_translateWidget->setContentsMargins(0, 0, 0, 0);

	SynGlyphX::GroupBoxSingleWidget* translateGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Position"), m_translateWidget, this);

    //Degree symbol gets displayed incorrectly unless we use QChar
    QChar degree = QChar('°');
    QString degreeSuffix = degree;

	m_rotateWidget = new XYZWidget(false, this);
	m_rotateWidget->SetRange(-180.0, 180.0);
	m_rotateWidget->SetWrapping(true);
	m_rotateWidget->SetDecimal(5);
	m_rotateWidget->SetSuffix(degreeSuffix);
	m_rotateWidget->setContentsMargins(0, 0, 0, 0);

	SynGlyphX::GroupBoxSingleWidget* rotateGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Rotation"), m_rotateWidget, this);

	m_scaleWidget = new XYZWidget(true, this);
	m_scaleWidget->SetRange(0.000001, 1000.0);
	m_scaleWidget->SetDecimal(5);
	m_scaleWidget->setContentsMargins(0, 0, 0, 0);
	m_scaleWidget->SetSpinBoxesLocked(true);

	SynGlyphX::GroupBoxSingleWidget* scaleGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Scale"), m_scaleWidget, this);

	m_rotateRateWidget = new XYZWidget(false, this);
	m_rotateRateWidget->SetRange(-100.0, 100.0);
	m_rotateRateWidget->SetDecimal(3);
	m_rotateRateWidget->setContentsMargins(0, 0, 0, 0);

	SynGlyphX::GroupBoxSingleWidget* rotateRateGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Animation - Rotation"), m_rotateRateWidget, this);
    
    form->addRow(tr("Shape:"), m_geometryShapeComboBox);
    form->addRow(tr("Surface:"), m_geometrySurfaceComboBox);
    form->addRow(tr("Topology:"), m_topologyComboBox);
    form->addRow(colorAndRatioLayout);
    form->addRow(translateGroupBox);
    form->addRow(rotateGroupBox);
    form->addRow(scaleGroupBox);
	form->addRow(rotateRateGroupBox);

    if ((childOptions & ShowOnBottom) || (childOptions & ShowOnTop)) {

        QWidget* childrenWidget = CreateChildrenWidget(childOptions);
        if (childOptions & ShowOnBottom) {
            form->addRow(childrenWidget);
        }
        else {
            form->insertRow(0, childrenWidget);
        }
    }

    setLayout(form);

    //QObject::connect(m_geometryShapeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SingleGlyphWidget::OnShapeComboBoxChanged);
    //m_ratioGroupBox->setVisible(false);
}

void SingleGlyphWidget::SetWidgetFromGlyph(const SynGlyphX::Glyph& glyph, bool isNotRootNode) {

	m_geometryShapeComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::GlyphStructuralProperties::s_shapeNames.left.at(glyph.GetStructure().GetGeometryShape())));
	m_geometrySurfaceComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::GlyphStructuralProperties::s_surfaceNames.left.at(glyph.GetStructure().GetGeometrySurface())));
	m_topologyComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::GlyphStructuralProperties::s_virtualTopologyNames.left.at(glyph.GetStructure().GetVirtualTopology())));

    m_colorWidget->SetColor(glyph.GetColor(), glyph.GetTransparency());

	m_translateWidget->setEnabled(isNotRootNode);
    m_translateWidget->Set(glyph.GetPosition());
    m_rotateWidget->Set(glyph.GetRotation());
    m_scaleWidget->Set(glyph.GetScale());
	m_rotateRateWidget->Set(glyph.GetRotationRate());

    //m_ratioSpinBox->setValue(glyph->GetRatio());
}

void SingleGlyphWidget::SetGlyphFromWidget(SynGlyphX::Glyph& glyph) {

	glyph.GetStructure().SetGeometryShape(SynGlyphX::GlyphStructuralProperties::s_shapeNames.right.at(m_geometryShapeComboBox->currentText().toStdWString())); 
	glyph.GetStructure().SetGeometrySurface(SynGlyphX::GlyphStructuralProperties::s_surfaceNames.right.at(m_geometrySurfaceComboBox->currentText().toStdWString()));
    
	glyph.GetStructure().SetVirtualTopology(SynGlyphX::GlyphStructuralProperties::s_virtualTopologyNames.right.at(m_topologyComboBox->currentText().toStdWString()));

	QColor color = m_colorWidget->GetColor();
	glyph.GetColor() = SynGlyphX::ColorButton::ConvertQColorToColor(color);
	glyph.GetTransparency() = color.alpha();

	glyph.GetPosition() = { { m_translateWidget->GetX(), m_translateWidget->GetY(), m_translateWidget->GetZ() } };
	glyph.GetRotation() = { { m_rotateWidget->GetX(), m_rotateWidget->GetY(), m_rotateWidget->GetZ() } };
	glyph.GetScale() = { { m_scaleWidget->GetX(), m_scaleWidget->GetY(), m_scaleWidget->GetZ() } };

	glyph.GetRotationRate() = { { m_rotateRateWidget->GetX(), m_rotateRateWidget->GetY(), m_rotateRateWidget->GetZ() } };

    //glyph->SetRatio(m_ratioSpinBox->value());
}

void SingleGlyphWidget::SetNumberOfChildren(unsigned int numChildren) {

    if (m_childrenSpinBox != NULL) {
        m_childrenSpinBox->setValue(numChildren);
    }
}

unsigned int SingleGlyphWidget::GetNumberOfChildren() const {

    if (m_childrenSpinBox == NULL) {
        return 0;
    }
    else {
        return static_cast<unsigned int>(m_childrenSpinBox->value());
    }
}
/*
void SingleGlyphWidget::OnShapeComboBoxChanged(int index) {

	SynGlyphXANTz::GlyphProperties::Shape shape = static_cast<SynGlyphXANTz::GlyphProperties::Shape>(index);

	m_ratioGroupBox->setVisible(shape == SynGlyphXANTz::GlyphProperties::Torus);
}*/