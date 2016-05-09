#include "linkedwidgetsmanager.h"

LinkedWidgetsManager::LinkedWidgetsManager(Glyph3DView* glyph3DView, QObject *parent)
	: QObject(parent),
	m_glyph3DView(glyph3DView)
{

}

LinkedWidgetsManager::~LinkedWidgetsManager()
{

}

void LinkedWidgetsManager::SetFilterView(bool set) {

	if (set) {

		m_glyph3DView->SetFilteredResultsDisplayMode(SynGlyphXANTz::ANTzForestWidget::HideUnfiltered);
	}
	else {

		m_glyph3DView->SetFilteredResultsDisplayMode(SynGlyphXANTz::ANTzForestWidget::None);
	}
	Q_FOREACH(QCheckBox* checkbox, m_filterViewCheckboxes) {

		if (sender() != checkbox) {

			SetFilterViewOnCheckbox(checkbox, set);
		}
	}
}

bool LinkedWidgetsManager::GetFilterView() const {

	return (m_glyph3DView->GetFilteredResultsDisplayMode() == SynGlyphXANTz::ANTzForestWidget::HideUnfiltered);
}

void LinkedWidgetsManager::AddFilterViewCheckbox(QCheckBox *checkbox) {

	m_filterViewCheckboxes.append(checkbox);
	QObject::connect(checkbox, &QCheckBox::toggled, this, &LinkedWidgetsManager::SetFilterView);
	SetFilterViewOnCheckbox(checkbox, GetFilterView());
}

void LinkedWidgetsManager::SetFilterViewOnCheckbox(QCheckBox* checkbox, bool set) {

	bool areSignalsBlocked = checkbox->signalsBlocked();
	checkbox->blockSignals(true);
	checkbox->setChecked(set);
	checkbox->blockSignals(areSignalsBlocked);
}