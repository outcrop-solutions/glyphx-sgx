#include "linkedwidgetsmanager.h"

LinkedWidgetsManager::LinkedWidgetsManager(SynGlyphX::SceneViewer* viewer, QObject *parent)
	: QObject(parent),
	m_viewer(viewer)
{

}

LinkedWidgetsManager::~LinkedWidgetsManager()
{

}

void LinkedWidgetsManager::SetFilterView(bool set) {

	if (set) {

		m_viewer->setFilteredResultsDisplayMode(SynGlyphX::FilteredResultsDisplayMode::HideUnfiltered);
	}
	else {

		m_viewer->setFilteredResultsDisplayMode(SynGlyphX::FilteredResultsDisplayMode::TranslucentUnfiltered);
	}
	Q_FOREACH(QCheckBox* checkbox, m_filterViewCheckboxes) {

		if (sender() != checkbox) {

			SetFilterViewOnCheckbox(checkbox, set);
		}
	}
}

bool LinkedWidgetsManager::GetFilterView() const {

	return m_viewer ? (m_viewer->filteredResultsDisplayMode() == SynGlyphX::FilteredResultsDisplayMode::HideUnfiltered) : false;
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
