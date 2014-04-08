#include "singleglyphwidget.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <limits>
#include "typedefs.h"

SingleGlyphWidget::SingleGlyphWidget(bool showNumberOfChildrenWidget, QWidget *parent)
    : QWidget(parent)
{
    CreateWidgets();

    m_childrenSpinBox->setEnabled(showNumberOfChildrenWidget);
}

SingleGlyphWidget::~SingleGlyphWidget()
{

}

void SingleGlyphWidget::CreateWidgets() {
    
    QFormLayout* form = new QFormLayout(this);
    setLayout(form);

    m_childrenSpinBox = new QSpinBox(this);
    m_childrenSpinBox->setMinimum(1);
    m_childrenSpinBox->setValue(3);

    m_geometryShapeComboBox = new QComboBox(this);
    for (int i = 0; i < 10; ++i) {
        m_geometryShapeComboBox->addItem(ShapeToString((SynGlyphX::Geometry::Shape)i));
    }

    m_geometrySurfaceComboBox = new QComboBox(this);
    m_geometrySurfaceComboBox->addItem(SurfaceToString(SynGlyphX::Geometry::Wireframe));
    m_geometrySurfaceComboBox->addItem(SurfaceToString(SynGlyphX::Geometry::Solid));

    m_topologyComboBox = new QComboBox(this);
    for (int i = 1; i < 8; ++i) {
        m_topologyComboBox->addItem(TopologyTypeToString((SynGlyphX::Topology::Type)i));
    }

    QGroupBox* colorGroupBox = new QGroupBox(tr("Color"), this);
    QHBoxLayout* colorBoxLayout = new QHBoxLayout(this);
    m_colorWidget = new ColorAlphaWidget(this);
    colorBoxLayout->addWidget(m_colorWidget);
    colorGroupBox->setLayout(colorBoxLayout);

    QGroupBox* translateGroupBox = new QGroupBox(tr("Translate"), this);
    QHBoxLayout* translateBoxLayout = new QHBoxLayout(this);
    m_translateWidget = new XYZWidget(this);
    m_translateWidget->SetRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
    translateBoxLayout->addWidget(m_translateWidget);
    translateGroupBox->setLayout(translateBoxLayout);

    QGroupBox* rotateGroupBox = new QGroupBox(tr("Rotate"), this);
    QHBoxLayout* rotateBoxLayout = new QHBoxLayout(this);
    m_rotateWidget = new XYZWidget(this);
    m_rotateWidget->SetRange(0.0, 360.0);
    m_rotateWidget->SetWrapping(true);
    rotateBoxLayout->addWidget(m_rotateWidget);
    rotateGroupBox->setLayout(rotateBoxLayout);

    QGroupBox* scaleGroupBox = new QGroupBox(tr("Scale"), this);
    QHBoxLayout* scaleBoxLayout = new QHBoxLayout(this);
    m_scaleWidget = new XYZWidget(this);
    m_scaleWidget->SetRange(0.0, std::numeric_limits<double>::max());
    scaleBoxLayout->addWidget(m_scaleWidget);
    scaleGroupBox->setLayout(scaleBoxLayout);

    form->addRow(tr("Number of Children"), m_childrenSpinBox);
    form->addRow(tr("Shape"), m_geometryShapeComboBox);
    form->addRow(tr("Surface"), m_geometrySurfaceComboBox);
    form->addRow(tr("Topology"), m_topologyComboBox);
    form->addRow(colorGroupBox);
    form->addRow(translateGroupBox);
    form->addRow(rotateGroupBox);
    form->addRow(scaleGroupBox);
}

QString SingleGlyphWidget::ShapeToString(SynGlyphX::Geometry::Shape shape) {
    if (shape == SynGlyphX::Geometry::Cube) {
        return "Cube";
    }
    else if (shape == SynGlyphX::Geometry::Sphere) {
        return "Sphere";
    }
    else if (shape == SynGlyphX::Geometry::Cone) {
        return "Cone";
    }
    else if (shape == SynGlyphX::Geometry::Torus) {
        return "Torus";
    }
    else if (shape == SynGlyphX::Geometry::Dodecahedron) {
        return "Dodecahedron";
    }
    else if (shape == SynGlyphX::Geometry::Octahedron) {
        return "Octahedron";
    }
    else if (shape == SynGlyphX::Geometry::Tetrahedron) {
        return "Tetrahedron";
    }
    else if (shape == SynGlyphX::Geometry::Icosahedron) {
        return "Icosahedron";
    }
    else if (shape == SynGlyphX::Geometry::Pin) {
        return "Pin";
    }
    else if (shape == SynGlyphX::Geometry::Cylinder) {
        return "Cylinder";
    }

    return "";
}

QString SingleGlyphWidget::SurfaceToString(SynGlyphX::Geometry::Surface surface) {
    if (surface == SynGlyphX::Geometry::Wireframe) {
        return "Wireframe";
    }
    else if (surface == SynGlyphX::Geometry::Solid) {
        return "Solid";
    }

    return "";
}

QString SingleGlyphWidget::TopologyTypeToString(SynGlyphX::Topology::Type topo) {
    if (topo == SynGlyphX::Topology::Cube) {
        return "Cube";
    }
    else if (topo == SynGlyphX::Topology::Sphere) {
        return "Sphere";
    }
    else if (topo == SynGlyphX::Topology::Torus) {
        return "Torus";
    }
    else if (topo == SynGlyphX::Topology::Cylinder) {
        return "Cylinder";
    }
    else if (topo == SynGlyphX::Topology::Pin) {
        return "Pin";
    }
    else if (topo == SynGlyphX::Topology::Rod) {
        return "Rod";
    }
    else if (topo == SynGlyphX::Topology::Point) {
        return "Point";
    }

    return "";
}