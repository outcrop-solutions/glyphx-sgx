#include "radiobuttongroupwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>
#include <stdexcept>

namespace SynGlyphX {

	RadioButtonGroupWidget::RadioButtonGroupWidget(const QStringList& buttonNames, Qt::Orientation alignment, QWidget *parent)
		: QWidget(parent)
	{
		if (buttonNames.empty()) {

			throw std::invalid_argument("Can't create RadioButtonGroupWidget with zero radio buttons");
		}

		QBoxLayout* layout;
		if (alignment == Qt::Horizontal) {

			layout = new QHBoxLayout(this);
		}
		else {

			layout = new QVBoxLayout(this);
		}

		m_buttonGroup = new QButtonGroup(this);
		for (int i = 0; i < buttonNames.size(); ++i) {

			QRadioButton* radioButton = new QRadioButton(buttonNames[i], this);
			layout->addWidget(radioButton);
			m_buttonGroup->addButton(radioButton, i);
		}
		m_buttonGroup->button(0)->setChecked(true);

		setLayout(layout);

		QObject::connect(m_buttonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &RadioButtonGroupWidget::ButtonClicked);
	}

	RadioButtonGroupWidget::~RadioButtonGroupWidget()
	{

	}

	void RadioButtonGroupWidget::UncheckAllButtons() {

		Q_FOREACH(QAbstractButton* radioButton, m_buttonGroup->buttons()) {

			radioButton->setChecked(false);
		}
	}

	void RadioButtonGroupWidget::SetCheckedButton(int id) {

		QAbstractButton* button = m_buttonGroup->button(id);
		if (button == nullptr) {

			throw std::invalid_argument("RadioButtonGroupWidget does not have a button with the given id");
		}

		button->setChecked(true);
		emit ButtonClicked(id);
	}

	int RadioButtonGroupWidget::GetCheckedButton() const {

		return m_buttonGroup->checkedId();
	}

	QString RadioButtonGroupWidget::GetCheckedButtonLabel() const {

		return m_buttonGroup->checkedButton()->text();
	}

	void RadioButtonGroupWidget::SetButtonEnabled(int id, bool enabled) {

		QAbstractButton* button = m_buttonGroup->button(id);
		if (button == nullptr) {

			throw std::invalid_argument("RadioButtonGroupWidget does not have a button with the given id");
		}

		button->setEnabled(enabled);
	}

} //namespace SynGlyphX