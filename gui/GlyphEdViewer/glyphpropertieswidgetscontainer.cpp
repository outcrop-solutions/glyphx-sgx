#include "glyphpropertieswidgetscontainer.h"
#include "GlyphForestInfoModel.h"

GlyphPropertiesWidgetsContainer::GlyphPropertiesWidgetsContainer(SynGlyphX::GlyphForestInfoModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QObject(parent),
	m_model(model),
	m_selectionModel(selectionModel)
{
	/*m_visualPropertiesStackedWidget = new QStackedWidget(parent);
	m_visualPropertiesWidget = new SynGlyphX::VisualGlyphPropertiesWidget(false, SynGlyphX::VisualGlyphPropertiesWidget::ShowOnBottom, m_visualPropertiesStackedWidget);
	m_visualPropertiesWidget->setEnabled(false);
	m_visualPropertiesStackedWidget->addWidget(new QWidget(m_visualPropertiesStackedWidget));
	m_visualPropertiesStackedWidget->addWidget(m_visualPropertiesWidget);*/

	m_textPropertiesStackedWidget = new QStackedWidget(parent);
	m_textPropertiesWidget = new SynGlyphX::TextGlyphPropertiesWidget(m_textPropertiesStackedWidget);
	m_textPropertiesWidget->SetReadOnly(true);
	m_textPropertiesStackedWidget->addWidget(new QWidget(m_textPropertiesStackedWidget));
	m_textPropertiesStackedWidget->addWidget(m_textPropertiesWidget);

	QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &GlyphPropertiesWidgetsContainer::OnSelectionChanged);

	SelectWidgetsInStackedWidgets(0);
}

GlyphPropertiesWidgetsContainer::~GlyphPropertiesWidgetsContainer()
{

}

QStackedWidget* GlyphPropertiesWidgetsContainer::GetVisualProperitesWidget() const {

	return m_visualPropertiesStackedWidget;
}

QStackedWidget* GlyphPropertiesWidgetsContainer::GetTextProperitesWidget() const {

	return m_textPropertiesStackedWidget;
}

void GlyphPropertiesWidgetsContainer::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
	SelectWidgetsInStackedWidgets(selectedItems.empty() ? 0 : 1);
	if (!selectedItems.empty()) {

		const QModelIndex& index = selectedItems.back();
		const SynGlyphX::GlyphForestInfoModel::GlyphTextProperties& textProperties = m_model->GetGlyphTextProperties(index);
		//m_visualPropertiesWidget->SetWidgetFromGlyph(glyph, true);
		//m_visualPropertiesWidget->SetNumberOfChildren(m_model->rowCount(index));
		m_textPropertiesWidget->SetWidget(textProperties[0], textProperties[2], textProperties[1]);
	}
}

void GlyphPropertiesWidgetsContainer::SelectWidgetsInStackedWidgets(unsigned int index) {

	//m_visualPropertiesStackedWidget->setCurrentIndex(index);
	m_textPropertiesStackedWidget->setCurrentIndex(index);
}