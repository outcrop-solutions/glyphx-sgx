#include "passwordlineedit.h"
#include <QtWidgets/QHBoxLayout>

namespace SynGlyphX {

	PasswordLineEdit::PasswordLineEdit(QWidget *parent)
		: QWidget(parent)
	{
		QHBoxLayout* mainLayout = new QHBoxLayout(this);

		m_lineEdit = new QLineEdit(this);
		m_lineEdit->setEchoMode(QLineEdit::EchoMode::Password);
		mainLayout->addWidget(m_lineEdit, 1);

		m_showHidePasswordButton = new QPushButton(tr("Show"), this);
		mainLayout->addWidget(m_showHidePasswordButton);
		QObject::connect(m_showHidePasswordButton, &QPushButton::clicked, this, &PasswordLineEdit::OnShowHidePasswordButtonClicked);

		setLayout(mainLayout);
	}

	PasswordLineEdit::~PasswordLineEdit()
	{

	}

	void PasswordLineEdit::SetReadOnly(bool isReadOnly) {

		m_lineEdit->setReadOnly(isReadOnly);
	}

	void PasswordLineEdit::SetPassword(const QString& password) {

		m_lineEdit->setText(password);
	}

	QString PasswordLineEdit::GetPassword() const {

		return m_lineEdit->text();
	}

	void PasswordLineEdit::OnShowHidePasswordButtonClicked() {

		if (m_lineEdit->echoMode() == QLineEdit::EchoMode::Password) {

			m_showHidePasswordButton->setText(tr("Hide"));
			m_lineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
		}
		else {

			m_showHidePasswordButton->setText(tr("Show"));
			m_lineEdit->setEchoMode(QLineEdit::EchoMode::Password);
		}
	}

} //namespace SynGlyphX