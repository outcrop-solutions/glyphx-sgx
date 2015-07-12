#include "minmaxglyph3dwidget.h"
#include <QtWidgets/QVBoxLayout>

namespace SynGlyphXANTz {

	MinMaxGlyph3DWidget::MinMaxGlyph3DWidget(QWidget *parent)
		: QWidget(parent)
		//m_model(nullptr),
		//m_selectionModel(nullptr)
	{
		m_maxGlyph3DWidget = new ANTzSingleGlyphTreeWidget(MinMaxGlyphTreeModel::GlyphType::Max, this);
		m_maxGlyph3DWidget->SetEditingMode(ANTzSingleGlyphTreeWidget::EditingMode::None);

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(m_maxGlyph3DWidget);

		setLayout(layout);
	}

	MinMaxGlyph3DWidget::~MinMaxGlyph3DWidget()
	{

	}

	void MinMaxGlyph3DWidget::SetModel(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel) {

		m_maxGlyph3DWidget->SetModel(model, selectionModel);
	}

	void MinMaxGlyph3DWidget::EnableAnimation(bool enable) {

		m_maxGlyph3DWidget->EnableAnimation(enable);
	}

	void MinMaxGlyph3DWidget::SetBaseImage(const QString& baseImageFilename) {

		m_maxGlyph3DWidget->SetBaseImage(baseImageFilename);
	}

	const QString& MinMaxGlyph3DWidget::GetBaseImageFilename() const {

		return m_maxGlyph3DWidget->GetBaseImageFilename();
	}

} //namespace SynGlyphXANTz