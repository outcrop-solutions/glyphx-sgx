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

		for (auto shape : SynGlyphX::GlyphGeometryInfo::s_shapeNames.left) {

			m_geometryShapeComboBox->addItem(QString::fromStdWString(shape.second));
		}

		m_topologyComboBox = new QComboBox(this);
		for (auto virtualTopology : SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left) {

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

		m_nonmappableGeometryWidget = new NonMappableGeometryWidget(this);
		m_nonmappableGeometryWidget->setContentsMargins(0, 0, 0, 0);

		surfaceAndRatioLayout->addWidget(m_nonmappableGeometryWidget);
		surfaceAndRatioLayout->addStretch(1);

		layout->addLayout(shapeAndTopologyLayout);
		layout->addLayout(surfaceAndRatioLayout);

		setLayout(layout);

		QObject::connect(m_geometryShapeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ emit GlyphPropertyUpdated(UpdateGeometry); });
		QObject::connect(m_nonmappableGeometryWidget, &NonMappableGeometryWidget::SurfaceChanged, this, [this]{ emit GlyphPropertyUpdated(UpdateSurface); });
		QObject::connect(m_topologyComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ emit GlyphPropertyUpdated(UpdateTopology); });
		QObject::connect(m_nonmappableGeometryWidget, &NonMappableGeometryWidget::TorusRatioChanged, this, [this]{ emit GlyphPropertyUpdated(UpdateTorusRatio); });


		QObject::connect(m_geometryShapeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &GlyphStructureWidget::OnShapeComboBoxChanged);
		m_nonmappableGeometryWidget->ShowTorusRatioWidget(false);
	}

	GlyphStructureWidget::~GlyphStructureWidget()
	{

	}

	GlyphGeometry GlyphStructureWidget::GetGlyphGeometry() const {

		GlyphGeometry structure;
		structure.SetGeometryShape(SynGlyphX::GlyphGeometryInfo::s_shapeNames.right.at(m_geometryShapeComboBox->currentText().toStdWString()));
		structure.SetGeometrySurface(m_nonmappableGeometryWidget->GetSurface());
		structure.SetTorusRatio(m_nonmappableGeometryWidget->GetTorusRatio());

		return structure;
	}

	VirtualTopology GlyphStructureWidget::GetVirtualTopology() const {

		VirtualTopology virtualTopology;
		virtualTopology.SetType(SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.right.at(m_topologyComboBox->currentText().toStdWString()));

		return virtualTopology;
	}

	void GlyphStructureWidget::SetWidgetFromGlyphGeometryAndTopology(const GlyphGeometry& structure, const VirtualTopology& virtualTopology) {

		m_geometryShapeComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::GlyphGeometryInfo::s_shapeNames.left.at(structure.GetGeometryShape())));
		m_nonmappableGeometryWidget->SetWidget(structure.GetGeometrySurface(), structure.GetTorusRatio());

		m_topologyComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left.at(virtualTopology.GetType())));
	}

	void GlyphStructureWidget::OnShapeComboBoxChanged(int index) {

		SynGlyphX::GlyphGeometryInfo::Shape shape = static_cast<SynGlyphX::GlyphGeometryInfo::Shape>(index);
		m_nonmappableGeometryWidget->ShowTorusRatioWidget(shape == SynGlyphX::GlyphGeometryInfo::Torus);
	}

} //namespace SynGlyphX