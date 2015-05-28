#include "linkedwidgetsmanager.h"

LinkedWidgetsManager::LinkedWidgetsManager(SynGlyphXANTz::ANTzForestWidget* antzWidget, QObject *parent)
	: QObject(parent),
	m_antzWidget(antzWidget)
{

}

LinkedWidgetsManager::~LinkedWidgetsManager()
{

}

void LinkedWidgetsManager::SetFilterView(bool set) {

	m_antzWidget->SetHideUnselectedGlyphTrees(set);
	Q_FOREACH(QCheckBox* checkbox, m_filterViewCheckboxes) {

		if (sender() != checkbox) {

			SetFilterViewOnCheckbox(checkbox, set);
		}
	}
}

bool LinkedWidgetsManager::GetFilterView() const {

	return m_antzWidget->GetHideUnselectedGlyphTrees();
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