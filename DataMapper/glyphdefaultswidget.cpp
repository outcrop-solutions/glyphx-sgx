#include "glyphdefaultswidget.h"
#include <QtWidgets/QFormLayout>
#include "notemptyvalidator.h"

GlyphDefaultsWidget::GlyphDefaultsWidget(const SynGlyphX::DataMappingDefaults& defaults, QWidget *parent)
	: QTabWidget(parent)
{
	CreateTagTab(defaults);
}

GlyphDefaultsWidget::~GlyphDefaultsWidget()
{

}

SynGlyphX::DataMappingDefaults GlyphDefaultsWidget::GetDefaults() const {

	SynGlyphX::DataMappingDefaults defaults;
	
	defaults.SetTagField(SynGlyphX::DataMappingDefaults::s_tagFieldStrings.right.at(m_tagFieldDefaultComboBox->currentText().toStdWString()));
	defaults.SetDefaultTagValue(m_tagValueDefaultLineEdit->text().toStdWString());
	
	return defaults;
}

void GlyphDefaultsWidget::CreateTagTab(const SynGlyphX::DataMappingDefaults& defaults) {

	QWidget* tab = new QWidget(this);
	QFormLayout* layout = new QFormLayout(tab);

	m_tagFieldDefaultComboBox = new QComboBox(tab);
	boost::bimap<SynGlyphX::DataMappingDefaults::TagFieldPropertyDefault, std::wstring>::right_const_iterator iT = SynGlyphX::DataMappingDefaults::s_tagFieldStrings.right.begin();
	for (; iT != SynGlyphX::DataMappingDefaults::s_tagFieldStrings.right.end(); ++iT) {

		m_tagFieldDefaultComboBox->addItem(tr("Same As ") + QString::fromStdWString(iT->first));
	}
	layout->addRow(tr("Default Input Field"), m_tagFieldDefaultComboBox);

	m_tagValueDefaultLineEdit = new QLineEdit(QString::fromStdWString(defaults.GetDefaultTagValue()), tab);
	SynGlyphX::NotEmptyValidator* validator = new SynGlyphX::NotEmptyValidator(true, m_tagValueDefaultLineEdit);
	m_tagValueDefaultLineEdit->setValidator(validator);
	layout->addRow(tr("Default Value"), m_tagValueDefaultLineEdit);

	tab->setLayout(layout);
	addTab(tab, tr("Tag"));
}