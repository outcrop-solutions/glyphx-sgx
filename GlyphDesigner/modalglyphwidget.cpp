#include "modalglyphwidget.h"

ModalGlyphWidget::ModalGlyphWidget(GlyphTreeModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
    : SingleGlyphWidget(SingleGlyphWidget::ShowOnBottom | SingleGlyphWidget::AddChildrenButton, parent),
    m_model(model),
    m_selectionModel(selectionModel)
{
    ConnectWidgetSignals();

    QObject::connect(m_selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(SelectionChanged(const QItemSelection&, const QItemSelection&)));

    QObject::connect(m_model, SIGNAL(NodeUpdated(const QModelIndex&)), this, SLOT(OnNodeUpdated(const QModelIndex&)));

    setEnabled(false);
}

ModalGlyphWidget::~ModalGlyphWidget()
{

}

void ModalGlyphWidget::ConnectWidgetSignals() {

    m_propertyConnections.push_back(QObject::connect(m_geometryShapeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ OnWidgetUpdated(GlyphTreeModel::UpdateGeometry); }));
    m_propertyConnections.push_back(QObject::connect(m_geometrySurfaceComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ OnWidgetUpdated(GlyphTreeModel::UpdateSurface); }));
    m_propertyConnections.push_back(QObject::connect(m_topologyComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this]{ OnWidgetUpdated(GlyphTreeModel::UpdateTopology); }));

    m_propertyConnections.push_back(QObject::connect(m_ratioSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this]{ OnWidgetUpdated(GlyphTreeModel::UpdateScale); }));

    m_propertyConnections.push_back(QObject::connect(m_colorWidget, &ColorAlphaWidget::ColorChanged, this, [this]{ OnWidgetUpdated(GlyphTreeModel::UpdateColor); }));

    m_propertyConnections.push_back(QObject::connect(m_translateWidget, &XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(GlyphTreeModel::UpdatePosition); }));
    m_propertyConnections.push_back(QObject::connect(m_rotateWidget, &XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(GlyphTreeModel::UpdateRotation); }));
    m_propertyConnections.push_back(QObject::connect(m_scaleWidget, &XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(GlyphTreeModel::UpdateScale); }));
}

void ModalGlyphWidget::DisconnectWidgetSignals() {

    for (int i = 0; i < m_propertyConnections.size(); ++i) {
        QObject::disconnect(m_propertyConnections[i]);
    }
    m_propertyConnections.clear();
}

void ModalGlyphWidget::OnWidgetUpdated(GlyphTreeModel::PropertyUpdates updates) {

    const QModelIndexList& selected = m_selectionModel->selectedIndexes();
    if (!selected.isEmpty()) {

        boost::shared_ptr<SynGlyphX::GlyphProperties> glyph(new SynGlyphX::GlyphProperties());
        SetGlyphFromWidget(glyph);

        m_model->UpdateNodes(selected, glyph, updates);
    }
}

void ModalGlyphWidget::SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

    const QModelIndexList& selectedList = m_selectionModel->selectedIndexes();
    if (selectedList.isEmpty()) {
        setEnabled(false);
    }
    else {
        DisconnectWidgetSignals();
        UpdateWidget(static_cast<pNPnode>(selectedList.back().internalPointer()));
        ConnectWidgetSignals();
        setEnabled(true);
    }
}

void ModalGlyphWidget::UpdateWidget(pNPnode node) {

    boost::shared_ptr<SynGlyphX::Glyph> glyph(new SynGlyphX::Glyph(node));
    SetWidgetFromGlyph(glyph);
}

void ModalGlyphWidget::OnNodeUpdated(const QModelIndex& index) {

    const QModelIndexList& selectedList = m_selectionModel->selectedIndexes();
    if (selectedList.back() == index) {

        DisconnectWidgetSignals();
        UpdateWidget(static_cast<pNPnode>(index.internalPointer()));
        ConnectWidgetSignals();
    }
}