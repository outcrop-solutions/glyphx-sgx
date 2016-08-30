#include "glyphdefaultswidget.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QCheckBox>
#include "notemptyvalidator.h"

namespace SynGlyphX {

	GlyphDefaultsWidget::GlyphDefaultsWidget(QWidget *parent)
		: QTabWidget(parent)
	{
		CreateTagTab();
		CreateScaleTab();
	}

	GlyphDefaultsWidget::~GlyphDefaultsWidget()
	{

	}

	void GlyphDefaultsWidget::SetDefaults(const SynGlyphX::DataMappingDefaults& defaults) {

		m_tagFieldDefaultComboBox->setCurrentText(QString::fromStdWString(SynGlyphX::DataMappingDefaults::s_tagFieldStrings.left.at(defaults.GetTagField())));
		m_tagValueDefaultLineEdit->setText(QString::fromStdWString(defaults.GetDefaultTagValue()));
		m_removeScaleZeroCheckbox->setChecked(defaults.GetRemoveWhenScaleIsZero());
	}

	SynGlyphX::DataMappingDefaults GlyphDefaultsWidget::GetDefaults() const {

		SynGlyphX::DataMappingDefaults defaults;

		defaults.SetTagField(SynGlyphX::DataMappingDefaults::s_tagFieldStrings.right.at(m_tagFieldDefaultComboBox->currentText().toStdWString()));
		defaults.SetDefaultTagValue(m_tagValueDefaultLineEdit->text().toStdWString());
		defaults.SetRemoveWhenScaleIsZero(m_removeScaleZeroCheckbox->isChecked());

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

	void GlyphDefaultsWidget::CreateScaleTab() {

		QWidget* tab = new QWidget(this);
		QVBoxLayout* tabLayout = new QVBoxLayout(tab);

		tabLayout->addStretch(1);

		m_removeScaleZeroCheckbox = new QCheckBox(tr("Remove objects whose value for Scale X, Scale Y, or Scale z is 0"), tab);
		tabLayout->addWidget(m_removeScaleZeroCheckbox);

		tabLayout->addStretch(1);

		tab->setLayout(tabLayout);
		addTab(tab, tr("Scale"));
	}

} //namespace SynGlyphX