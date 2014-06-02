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

    QObject::connect(m_geometryShapeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnWidgetUpdated()));
    QObject::connect(m_geometrySurfaceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnWidgetUpdated()));
    QObject::connect(m_topologyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnWidgetUpdated()));

    QObject::connect(m_colorWidget, SIGNAL(ColorChanged(const QColor&)), this, SLOT(OnWidgetUpdated()));

    QObject::connect(m_translateWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnWidgetUpdated()));
    QObject::connect(m_rotateWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnWidgetUpdated()));
    QObject::connect(m_scaleWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnWidgetUpdated()));
}

void ModalGlyphWidget::DisconnectWidgetSignals() {

    QObject::disconnect(m_geometryShapeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnWidgetUpdated()));
    QObject::disconnect(m_geometrySurfaceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnWidgetUpdated()));
    QObject::disconnect(m_topologyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnWidgetUpdated()));

    QObject::disconnect(m_colorWidget, SIGNAL(ColorChanged(const QColor&)), this, SLOT(OnWidgetUpdated()));

    QObject::disconnect(m_translateWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnWidgetUpdated()));
    QObject::disconnect(m_rotateWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnWidgetUpdated()));
    QObject::disconnect(m_scaleWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnWidgetUpdated()));
}

void ModalGlyphWidget::OnWidgetUpdated() {

    const QModelIndexList& selected = m_selectionModel->selectedIndexes();
    if (!selected.isEmpty()) {

        boost::shared_ptr<SynGlyphX::Glyph> glyph(new SynGlyphX::Glyph());
        SetGlyphFromWidget(glyph);

        m_model->UpdateNode(selected.back(), glyph);
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

    if (m_selectionModel->isSelected(index)) {
        DisconnectWidgetSignals();
        UpdateWidget(static_cast<pNPnode>(index.internalPointer()));
        ConnectWidgetSignals();
    }
}