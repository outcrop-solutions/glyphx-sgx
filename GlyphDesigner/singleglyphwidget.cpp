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
    QHBoxLayout* layout = new QHBoxLayout(this);

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
    for (int i = 0; i < 10; ++i) {
        m_geometryShapeComboBox->addItem(QString::fromStdWString(SynGlyphX::GlyphProperties::ShapeToString(static_cast<SynGlyphX::Geometry::Shape>(i))));
    }

    m_geometrySurfaceComboBox = new QComboBox(this);
	m_geometrySurfaceComboBox->addItem(QString::fromStdWString(SynGlyphX::GlyphProperties::SurfaceToString(SynGlyphX::Geometry::Wireframe)));
	m_geometrySurfaceComboBox->addItem(QString::fromStdWString(SynGlyphX::GlyphProperties::SurfaceToString(SynGlyphX::Geometry::Solid)));

    m_topologyComboBox = new QComboBox(this);
    for (int i = 0; i < 8; ++i) {
		m_topologyComboBox->addItem(QString::fromStdWString(SynGlyphX::GlyphProperties::TopologyTypeToString(static_cast<SynGlyphX::Topology::Type>(i))));
    }

    QHBoxLayout* colorAndRatioLayout = new QHBoxLayout(this);
    colorAndRatioLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* colorGroupBox = new QGroupBox(tr("Color"), this);
    QHBoxLayout* colorBoxLayout = new QHBoxLayout(colorGroupBox);
    colorBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_colorWidget = new ColorAlphaWidget(colorGroupBox);
    m_colorWidget->setContentsMargins(0, 0, 0, 0);
    colorBoxLayout->addWidget(m_colorWidget);
    colorGroupBox->setLayout(colorBoxLayout);

    colorAndRatioLayout->addWidget(colorGroupBox);

    m_ratioGroupBox = new QGroupBox(tr("Torus Ratio"), this);
    QHBoxLayout* ratioBoxLayout = new QHBoxLayout(m_ratioGroupBox);
    m_ratioSpinBox = new QDoubleSpinBox(m_ratioGroupBox);
    m_ratioSpinBox->setSingleStep(0.05);
    m_ratioSpinBox->setDecimals(2);
    ratioBoxLayout->addWidget(m_ratioSpinBox);
    m_ratioGroupBox->setLayout(ratioBoxLayout);

    colorAndRatioLayout->addWidget(m_ratioGroupBox);

    QGroupBox* translateGroupBox = new QGroupBox(tr("Position"), this);
    QHBoxLayout* translateBoxLayout = new QHBoxLayout(translateGroupBox);
    translateBoxLayout->setContentsMargins(0, 0, 0, 0);
    translateGroupBox->setLayout(translateBoxLayout);
    m_translateWidget = new XYZWidget(false, translateGroupBox);
    m_translateWidget->SetRange(-10000.0, 10000.0);
    m_translateWidget->SetDecimal(4);
    m_translateWidget->setContentsMargins(0, 0, 0, 0);
    translateBoxLayout->addWidget(m_translateWidget);

    //Degree symbol gets displayed incorrectly unless we use QChar
    QChar degree = QChar('�');
    QString degreeSuffix = degree;

    QGroupBox* rotateGroupBox = new QGroupBox(tr("Rotation"), this);
    QHBoxLayout* rotateBoxLayout = new QHBoxLayout(this);
    rotateBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_rotateWidget = new XYZWidget(false, this);
    m_rotateWidget->SetRange(-180.0, 180.0);
    m_rotateWidget->SetWrapping(true);
    m_rotateWidget->SetDecimal(5);
    m_rotateWidget->SetSuffix(degreeSuffix);
    m_rotateWidget->setContentsMargins(0, 0, 0, 0);
    rotateBoxLayout->addWidget(m_rotateWidget);
    rotateGroupBox->setLayout(rotateBoxLayout);

    QGroupBox* scaleGroupBox = new QGroupBox(tr("Scale"), this);
    QHBoxLayout* scaleBoxLayout = new QHBoxLayout(this);
    scaleBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_scaleWidget = new XYZWidget(true, this);
    m_scaleWidget->SetRange(0.000001, 1000.0);
    m_scaleWidget->SetDecimal(5);
    m_scaleWidget->setContentsMargins(0, 0, 0, 0);
    m_scaleWidget->SetSpinBoxesLocked(true);
    scaleBoxLayout->addWidget(m_scaleWidget);
    scaleGroupBox->setLayout(scaleBoxLayout);

    
    form->addRow(tr("Shape:"), m_geometryShapeComboBox);
    form->addRow(tr("Surface:"), m_geometrySurfaceComboBox);
    form->addRow(tr("Topology:"), m_topologyComboBox);
    form->addRow(colorAndRatioLayout);
    form->addRow(translateGroupBox);
    form->addRow(rotateGroupBox);
    form->addRow(scaleGroupBox);

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

    QObject::connect(m_geometryShapeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SingleGlyphWidget::OnShapeComboBoxChanged);
    m_ratioGroupBox->setVisible(false);
}

void SingleGlyphWidget::SetWidgetFromGlyph(boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph) {

    m_geometryShapeComboBox->setCurrentIndex(glyph->GetShape());
    m_geometrySurfaceComboBox->setCurrentIndex(glyph->GetSurface());
    //Because null is not in the list of topologies, shift by one
    m_topologyComboBox->setCurrentIndex(glyph->GetTopology());

    m_colorWidget->SetColor(glyph->GetColor());

    m_translateWidget->Set(glyph->GetTranslate());
    m_rotateWidget->Set(glyph->GetRotate());
    m_scaleWidget->Set(glyph->GetScale());

    m_ratioSpinBox->setValue(glyph->GetRatio());

    if (m_childrenSpinBox != NULL) {
        m_childrenSpinBox->setValue(glyph.get()->GetNumberOfChildren());
    }
}

void SingleGlyphWidget::SetGlyphFromWidget(boost::shared_ptr<SynGlyphX::GlyphProperties> glyph) {

    glyph->SetGeometry(static_cast<SynGlyphX::Geometry::Shape>(m_geometryShapeComboBox->currentIndex()), static_cast<SynGlyphX::Geometry::Surface>(m_geometrySurfaceComboBox->currentIndex()));
    //Because null is not in the list of topologies, shift by one
    glyph->SetTopology(static_cast<SynGlyphX::Topology::Type>(m_topologyComboBox->currentIndex()));

    const QColor& color = m_colorWidget->GetColor();
    glyph->SetColor(color.red(), color.green(), color.blue(), color.alpha());

    glyph->SetTranslate(m_translateWidget->GetX(), m_translateWidget->GetY(), m_translateWidget->GetZ());
    glyph->SetRotate(m_rotateWidget->GetX(), m_rotateWidget->GetY(), m_rotateWidget->GetZ());
    glyph->SetScale(m_scaleWidget->GetX(), m_scaleWidget->GetY(), m_scaleWidget->GetZ());

    glyph->SetRatio(m_ratioSpinBox->value());
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

void SingleGlyphWidget::OnShapeComboBoxChanged(int index) {

    SynGlyphX::Geometry::Shape shape = static_cast<SynGlyphX::Geometry::Shape>(index);

    m_ratioGroupBox->setVisible(shape == SynGlyphX::Geometry::Torus);
}