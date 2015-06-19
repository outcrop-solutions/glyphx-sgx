#include "glyphdefaultswidget.h"
#include <QtWidgets/QFormLayout>
#include "notemptyvalidator.h"

namespace SynGlyphX {

	GlyphDefaultsWidget::GlyphDefaultsWidget(QWidget *parent)
		: QTabWidget(parent)
	{
		CreateTagTab();
	}

	GlyphDefaultsWidget::~GlyphDefaultsWidget()
	{

	}

	void GlyphDefaultsWidget::SetDefaults(const SynGlyphX::DataMappingDefaults& defaults) {

		m_tagFieldDefaultComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::DataMappingDefaults::s_tagFieldStrings.left.at(defaults.GetTagField())));
		m_tagValueDefaultLineEdit->setText(QString::fromStdWString(defaults.GetDefaultTagValue()));
	}

	SynGlyphX::DataMappingDefaults GlyphDefaultsWidget::GetDefaults() const {

		SynGlyphX::DataMappingDefaults defaults;

		defaults.SetTagField(SynGlyphX::DataMappingDefaults::s_tagFieldStrings.right.at(m_tagFieldDefaultComboBox->currentText().toStdWString()));
		defaults.SetDefaultTagValue(m_tagValueDefaultLineEdit->text().toStdWString());

		return defaults;
	}

	void GlyphDefaultsWidget::CreateTagTab() {

		QWidget* tab = new QWidget(this);
		QFormLayout* layout = new QFormLayout(tab);

		m_tagFieldDefaultComboBox = new QComboBox(tab);
		boost::bimap<SynGlyphX::DataMappingGlyph::MappableField, std::wstring>::const_iterator iT = SynGlyphX::DataMappingDefaults::s_tagFieldStrings.begin();
		for (; iT != SynGlyphX::DataMappingDefaults::s_tagFieldStrings.end(); ++iT) {

			if (iT->left != SynGlyphX::DataMappingGlyph::MappableField::Tag) {

				m_tagFieldDefaultComboBox->addItem(QString::fromStdWString(iT->get_right()));
			}
		}
		layout->addRow(tr("Make Default Input Field Same As Input Field Of"), m_tagFieldDefaultComboBox);

		m_tagValueDefaultLineEdit = new QLineEdit(tab);
		SynGlyphX::NotEmptyValidator* validator = new SynGlyphX::NotEmptyValidator(true, m_tagValueDefaultLineEdit);
		m_tagValueDefaultLineEdit->setValidator(validator);
		layout->addRow(tr("Default Value"), m_tagValueDefaultLineEdit);

		tab->setLayout(layout);
		addTab(tab, tr("Tag"));
	}

} //namespace SynGlyphX