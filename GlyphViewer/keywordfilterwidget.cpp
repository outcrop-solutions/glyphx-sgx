#include "keywordfilterwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>

KeywordFilterWidget::KeywordFilterWidget(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* keywordLayout = new QHBoxLayout(this);
	keywordLayout->setContentsMargins(0, 0, 0, 0);

	QLabel* keywordLabel = new QLabel(tr("Keyword") + ":", this);
	keywordLayout->addWidget(keywordLabel);

	m_keywordLineEdit = new QLineEdit(this);
	keywordLayout->addWidget(m_keywordLineEdit, 1);
	QObject::connect(m_keywordLineEdit, &QLineEdit::textEdited, this, &KeywordFilterWidget::FilterUpdated);

	mainLayout->addLayout(keywordLayout);

	QHBoxLayout* optionsLayout = new QHBoxLayout(this);
	optionsLayout->setContentsMargins(0, 0, 0, 0);

	m_caseSenstiveCheckBox = new QCheckBox(tr("Case Sensitive"), this);
	optionsLayout->addWidget(m_caseSenstiveCheckBox);
	QObject::connect(m_caseSenstiveCheckBox, &QCheckBox::stateChanged, this, &KeywordFilterWidget::FilterUpdated);

	m_exactMatchCheckBox = new QCheckBox(tr("Exact Match"), this);
	optionsLayout->addWidget(m_exactMatchCheckBox);
	QObject::connect(m_exactMatchCheckBox, &QCheckBox::stateChanged, this, &KeywordFilterWidget::FilterUpdated);

	mainLayout->addLayout(optionsLayout);

	setLayout(mainLayout);
}

KeywordFilterWidget::~KeywordFilterWidget()
{

}

void KeywordFilterWidget::Set(const KeywordFilter& filter) {

	m_keywordLineEdit->setText(filter.GetKeyword());
	m_caseSenstiveCheckBox->setChecked(filter.GetCaseSensitive());
	m_exactMatchCheckBox->setChecked(filter.GetExactMatch());
}

KeywordFilter KeywordFilterWidget::Get() const {

	KeywordFilter filter;
	filter.SetKeyword(m_keywordLineEdit->text());
	filter.SetCaseSensitive(m_caseSenstiveCheckBox->isChecked());
	filter.SetExactMatch(m_exactMatchCheckBox->isChecked());

	return filter;
}