#include "modalglyphwidget.h"

ModalGlyphWidget::ModalGlyphWidget(MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent)
    : SingleGlyphWidget(SingleGlyphWidget::ShowOnBottom | SingleGlyphWidget::AddChildrenButton, parent),
    m_model(nullptr),
	m_glyphTreeType(glyphTreeType),
    m_selectionModel(nullptr)
{
    ConnectWidgetSignals();
    setEnabled(false);
}

ModalGlyphWidget::~ModalGlyphWidget()
{

}

void ModalGlyphWidget::SetModel(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel) {

	QObject::disconnect(m_glyphUpdateConnection);
	QObject::disconnect(m_selectionConnection);

	m_model = model;
	if (m_model != nullptr) {

		QObject::connect(m_model, &MinMaxGlyphTreeModel::dataChanged, this, &ModalGlyphWidget::OnGlyphUpdated);
	}

	m_selectionModel = selectionModel;
	if (m_selectionModel != nullptr) {

		QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ModalGlyphWidget::SelectionChanged);
	}
}

void ModalGlyphWidget::ConnectWidgetSignals() {

	m_propertyConnections.push_back(QObject::connect(m_geometryShapeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ OnWidgetUpdated(MinMaxGlyphTreeModel::UpdateGeometry); }));
	m_propertyConnections.push_back(QObject::connect(m_geometrySurfaceComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ OnWidgetUpdated(MinMaxGlyphTreeModel::UpdateSurface); }));
	m_propertyConnections.push_back(QObject::connect(m_topologyComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ OnWidgetUpdated(MinMaxGlyphTreeModel::UpdateTopology); }));

	m_propertyConnections.push_back(QObject::connect(m_ratioSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]{ OnWidgetUpdated(MinMaxGlyphTreeModel::UpdateScale); }));

	m_propertyConnections.push_back(QObject::connect(m_colorWidget, &SynGlyphX::ColorAlphaWidget::ColorChanged, this, [this]{ OnWidgetUpdated(MinMaxGlyphTreeModel::UpdateColor); }));

	m_propertyConnections.push_back(QObject::connect(m_translateWidget, &XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(MinMaxGlyphTreeModel::UpdatePosition); }));
	m_propertyConnections.push_back(QObject::connect(m_rotateWidget, &XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(MinMaxGlyphTreeModel::UpdateRotation); }));
	m_propertyConnections.push_back(QObject::connect(m_scaleWidget, &XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(MinMaxGlyphTreeModel::UpdateScale); }));
}

void ModalGlyphWidget::DisconnectWidgetSignals() {

    for (int i = 0; i < m_propertyConnections.size(); ++i) {
        QObject::disconnect(m_propertyConnections[i]);
    }
    m_propertyConnections.clear();
}

void ModalGlyphWidget::OnWidgetUpdated(MinMaxGlyphTreeModel::PropertyUpdates updates) {

    const QModelIndexList& selected = m_selectionModel->selectedIndexes();
    if (!selected.isEmpty()) {

        boost::shared_ptr<SynGlyphX::GlyphProperties> glyph(new SynGlyphX::GlyphProperties());
        SetGlyphFromWidget(glyph);

        m_model->UpdateGlyph(selected.back(), m_glyphTreeType, *glyph.get(), updates);
    }
}

void ModalGlyphWidget::SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

    const QModelIndexList& selectedList = m_selectionModel->selectedIndexes();
    if (selectedList.isEmpty()) {
        setEnabled(false);
    }
    else {
        DisconnectWidgetSignals();
		const QModelIndex& index = selectedList.back();
		UpdateWidget(index);
        ConnectWidgetSignals();
        setEnabled(true);
    }
}

void ModalGlyphWidget::UpdateWidget(const QModelIndex& index) {

    boost::shared_ptr<SynGlyphX::GlyphProperties> glyph;
	if (m_glyphTreeType == MinMaxGlyphTreeModel::GlyphType::Max) {

		glyph.reset(new SynGlyphX::GlyphProperties(m_model->GetMinMaxGlyph(index)->GetMaxGlyph()));
	}
	else {

		glyph.reset(new SynGlyphX::GlyphProperties(m_model->GetMinMaxGlyph(index)->GetMinGlyph()));
	}
    SetWidgetFromGlyph(glyph, index.parent().isValid());
	SetNumberOfChildren(m_model->rowCount(index));
}

void ModalGlyphWidget::OnGlyphUpdated(const QModelIndex& index) {

    const QModelIndexList& selectedList = m_selectionModel->selectedIndexes();
    if (selectedList.back() == index) {

        DisconnectWidgetSignals();
        UpdateWidget(index);
        ConnectWidgetSignals();
    }
}