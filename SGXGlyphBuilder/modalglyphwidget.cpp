#include "modalglyphwidget.h"

ModalGlyphWidget::ModalGlyphWidget(GlyphTreeModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
    : SingleGlyphWidget(false, parent),
    m_model(model),
    m_selectionModel(selectionModel)
{
    QObject::connect(m_geometryShapeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnUpdated()));
    QObject::connect(m_geometrySurfaceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnUpdated()));
    QObject::connect(m_topologyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnUpdated()));

    QObject::connect(m_colorWidget, SIGNAL(ColorChanged(const QColor&)), this, SLOT(OnUpdated()));

    QObject::connect(m_translateWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnUpdated()));
    QObject::connect(m_rotateWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnUpdated()));
    QObject::connect(m_scaleWidget, SIGNAL(ValuesChanged(double, double, double)), this, SLOT(OnUpdated()));

    QObject::connect(m_selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(SelectionChanged(const QItemSelection&, const QItemSelection&)));

    setEnabled(false);
}

ModalGlyphWidget::~ModalGlyphWidget()
{

}

void ModalGlyphWidget::OnUpdated() {

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

        boost::shared_ptr<SynGlyphX::Glyph> glyph(new SynGlyphX::Glyph(static_cast<pNPnode>(selectedList.back().internalPointer())));
        SetWidgetFromGlyph(glyph);
        setEnabled(true);
    }
}