#include "glyphstructurewidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>

namespace SynGlyphX {

	GlyphStructureWidget::GlyphStructureWidget(QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);

		m_geometryShapeComboBox = new QComboBox(this);

		for (auto shape : SynGlyphX::GlyphStructuralProperties::s_shapeNames.left) {

			m_geometryShapeComboBox->addItem(QString::fromStdWString(shape.second));
		}

		m_topologyComboBox = new QComboBox(this);
		for (auto virtualTopology : SynGlyphX::GlyphStructuralProperties::s_virtualTopologyNames.left) {

			m_topologyComboBox->addItem(QString::fromStdWString(virtualTopology.second));
		}

		QHBoxLayout* shapeAndTopologyLayout = new QHBoxLayout(this);
		shapeAndTopologyLayout->setContentsMargins(0, 0, 0, 0);
		shapeAndTopologyLayout->addWidget(new QLabel(tr("Shape:")));
		shapeAndTopologyLayout->addWidget(m_geometryShapeComboBox, 1);
		shapeAndTopologyLayout->addWidget(new QLabel(tr("Topology:")));
		shapeAndTopologyLayout->addWidget(m_topologyComboBox, 1);

		QHBoxLayout* surfaceAndRatioLayout = new QHBoxLayout(this);
		shapeAndTopologyLayout->setContentsMargins(0, 0, 0, 0);

		QStringList surfaceNames;
		for (auto surface : SynGlyphX::GlyphStructuralProperties::s_surfaceNames.left) {

			surfaceNames.push_back(QString::fromStdWString(surface.second));
		}
		m_surfaceRadioButtonGroup = new SynGlyphX::RadioButtonGroupWidget(surfaceNames, SynGlyphX::RadioButtonGroupWidget::Horizontal, this);
		QGroupBox* surfaceGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Surface"), m_surfaceRadioButtonGroup, this);

		surfaceAndRatioLayout->addWidget(surfaceGroupBox);

		m_ratioSpinBox = new QDoubleSpinBox(this);
		m_ratioSpinBox->setSingleStep(0.05);
		m_ratioSpinBox->setDecimals(2);
		m_ratioGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Torus Ratio"), m_ratioSpinBox, this);

		surfaceAndRatioLayout->addWidget(m_ratioGroupBox);
		surfaceAndRatioLayout->addStretch(1);

		layout->addLayout(shapeAndTopologyLayout);
		layout->addLayout(surfaceAndRatioLayout);

		setLayout(layout);

		QObject::connect(m_geometryShapeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ emit GlyphPropertyUpdated(UpdateGeometry); });
		QObject::connect(m_surfaceRadioButtonGroup, &SynGlyphX::RadioButtonGroupWidget::ButtonClicked, this, [this]{ emit GlyphPropertyUpdated(UpdateSurface); });
		QObject::connect(m_topologyComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ emit GlyphPropertyUpdated(UpdateTopology); });
		QObject::connect(m_ratioSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]{ emit GlyphPropertyUpdated(UpdateTorusRatio); });


		QObject::connect(m_geometryShapeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &GlyphStructureWidget::OnShapeComboBoxChanged);
		m_ratioGroupBox->setVisible(false);
	}

	GlyphStructureWidget::~GlyphStructureWidget()
	{

	}

	GlyphStructuralProperties GlyphStructureWidget::GetGlyphStructure() const {

		GlyphStructuralProperties structure;
		structure.SetGeometryShape(SynGlyphX::GlyphStructuralProperties::s_shapeNames.right.at(m_geometryShapeComboBox->currentText().toStdWString()));
		structure.SetGeometrySurface(SynGlyphX::GlyphStructuralProperties::s_surfaceNames.right.at(m_surfaceRadioButtonGroup->GetCheckedButtonLabel().toStdWString()));
		structure.SetVirtualTopology(SynGlyphX::GlyphStructuralProperties::s_virtualTopologyNames.right.at(m_topologyComboBox->currentText().toStdWString()));
		structure.SetTorusRatio(m_ratioSpinBox->value());

		return structure;
	}

	void GlyphStructureWidget::SetWidgetFromGlyphStructure(const GlyphStructuralProperties& structure) {

		m_geometryShapeComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::GlyphStructuralProperties::s_shapeNames.left.at(structure.GetGeometryShape())));
		m_surfaceRadioButtonGroup->SetCheckedButton(structure.GetGeometrySurface());
		m_topologyComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::GlyphStructuralProperties::s_virtualTopologyNames.left.at(structure.GetVirtualTopology())));
		m_ratioSpinBox->setValue(structure.GetTorusRatio());
	}

	void GlyphStructureWidget::OnShapeComboBoxChanged(int index) {

		SynGlyphX::GlyphStructuralProperties::Shape shape = static_cast<SynGlyphX::GlyphStructuralProperties::Shape>(index);
		m_ratioGroupBox->setVisible(shape == SynGlyphX::GlyphStructuralProperties::Torus);
	}

} //namespace SynGlyphX