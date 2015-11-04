#include "modalglyphwidget.h"

ModalGlyphWidget::ModalGlyphWidget(SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent)
    : SynGlyphX::GlyphPropertiesWidget(true, SynGlyphX::GlyphPropertiesWidget::ShowOnBottom | SynGlyphX::GlyphPropertiesWidget::AddChildrenButton, parent),
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

void ModalGlyphWidget::SetModel(SynGlyphXANTz::MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel) {

	QObject::disconnect(m_glyphUpdateConnection);
	QObject::disconnect(m_selectionConnection);

	m_model = model;
	if (m_model != nullptr) {

		QObject::connect(m_model, &SynGlyphXANTz::MinMaxGlyphTreeModel::dataChanged, this, &ModalGlyphWidget::OnGlyphUpdated);
	}

	m_selectionModel = selectionModel;
	if (m_selectionModel != nullptr) {

		QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &ModalGlyphWidget::SelectionChanged);
	}
}

void ModalGlyphWidget::ConnectWidgetSignals() {

	m_propertyConnections.push_back(QObject::connect(m_glyphStructureWidget, &SynGlyphX::GlyphStructureWidget::GlyphPropertyUpdated, this, &ModalGlyphWidget::OnWidgetUpdated));

	m_propertyConnections.push_back(QObject::connect(m_colorWidget, &SynGlyphX::ColorAlphaWidget::ColorChanged, this, [this]{ OnWidgetUpdated(SynGlyphX::UpdateColor); }));

	m_propertyConnections.push_back(QObject::connect(m_translateWidget, &SynGlyphX::XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(SynGlyphX::UpdatePosition); }));
	m_propertyConnections.push_back(QObject::connect(m_rotateWidget, &SynGlyphX::XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(SynGlyphX::UpdateRotation); }));
	m_propertyConnections.push_back(QObject::connect(m_scaleWidget, &SynGlyphX::XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(SynGlyphX::UpdateScale); }));

	m_propertyConnections.push_back(QObject::connect(m_rotateRateWidget, &SynGlyphX::XYZWidget::ValuesChanged, this, [this]{ OnWidgetUpdated(SynGlyphX::UpdateRotationRate); }));
}

void ModalGlyphWidget::DisconnectWidgetSignals() {

    for (int i = 0; i < m_propertyConnections.size(); ++i) {
        QObject::disconnect(m_propertyConnections[i]);
    }
    m_propertyConnections.clear();
}

void ModalGlyphWidget::OnWidgetUpdated(SynGlyphX::PropertyUpdates updates) {

    const QModelIndexList& selected = m_selectionModel->selectedIndexes();
    if (!selected.isEmpty()) {

		SynGlyphX::Glyph glyph;
        SetGlyphFromWidget(glyph);

        m_model->UpdateGlyphs(selected, m_glyphTreeType, glyph, updates);
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

	SynGlyphX::Glyph glyph;
	if (m_glyphTreeType == SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType::Max) {

		glyph = m_model->GetMinMaxGlyph(index)->second.GetMaxGlyph();
	}
	else {

		glyph = m_model->GetMinMaxGlyph(index)->second.GetMinGlyph();
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