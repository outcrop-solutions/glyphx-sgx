#include "newglyphtreewizard.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QInputDialog>

namespace SynGlyphX {

	NewGlyphTreeWizard::NewGlyphTreeWizard(unsigned int numberOfBranches, QWidget *parent)
		: QWizard(parent)
	{
		if (numberOfBranches < 1) {

			throw std::invalid_argument("Create New Tree Wizard can not create a tree of zero branches");
		}

		setOptions(QWizard::IndependentPages | QWizard::IgnoreSubTitles | QWizard::NoBackButtonOnStartPage);
		setWindowTitle(tr("Create New Glyph Tree"));

		for (int i = 0; i < numberOfBranches; ++i) {

			QWizardPage* page = new QWizardPage(this);
			QVBoxLayout* layout = new QVBoxLayout(this);
			page->setLayout(layout);
			SynGlyphX::GlyphPropertiesWidget::ChildOptions childOptions = SynGlyphX::GlyphPropertiesWidget::Invisible;
			if (i != numberOfBranches - 1) {

				childOptions = SynGlyphX::GlyphPropertiesWidget::ShowOnBottom | SynGlyphX::GlyphPropertiesWidget::EnabledSpinBox;
			}

			SynGlyphX::GlyphPropertiesWidget* glyphWidget = new SynGlyphX::GlyphPropertiesWidget(childOptions, page);
			m_glyphWidgets.push_back(glyphWidget);
			if (i == 0) {

				page->setTitle("Glyphs for root level");
				glyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::s_defaultRootGlyph, false);
			}
			else {

				page->setTitle(QString::number(i).prepend("Glyphs for branch level "));
				glyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::s_defaultGlyph, true);
			}
			layout->addWidget(glyphWidget);

			addPage(page);
		}
	}

	NewGlyphTreeWizard::~NewGlyphTreeWizard()
	{

	}

	void NewGlyphTreeWizard::accept() {

		SynGlyphX::Glyph rootGlyph;
		m_glyphWidgets[0]->SetGlyphFromWidget(rootGlyph);
		rootGlyph.GetPosition()[0] = 180.0;
		rootGlyph.GetPosition()[1] = 90.0;
		SynGlyphX::GlyphGraph newMaxGlyphTree(rootGlyph);

		std::vector<SynGlyphX::Glyph> templates;
		std::vector<unsigned int> instanceCounts;
		for (int i = 1; i < m_glyphWidgets.size(); ++i) {

			SynGlyphX::Glyph glyph;
			m_glyphWidgets[i]->SetGlyphFromWidget(glyph);

			templates.push_back(glyph);
			instanceCounts.push_back(m_glyphWidgets[i - 1]->GetNumberOfChildren());
		}

		newMaxGlyphTree.AllocateChildSubtree(templates, instanceCounts, newMaxGlyphTree.GetRoot());

		m_dataMappingGlyphGraph = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(newMaxGlyphTree);
		QWizard::accept();
	}

	DataMappingGlyphGraph::SharedPtr NewGlyphTreeWizard::GetNewDataMappingGlyphGraph() const {

		return m_dataMappingGlyphGraph;
	}

	DataMappingGlyphGraph::SharedPtr NewGlyphTreeWizard::RunNewGlyphTreeWizard(QWidget *parent) {

		DataMappingGlyphGraph::SharedPtr dataMappingGlyphGraph;
		bool createTree = false;
		unsigned int numberOfBranches = QInputDialog::getInt(parent, "Create Glyph Tree", "Number Of Branches In Glyph Tree (Including Root)", 6, 1, 214783647, 1, &createTree);
		if (createTree) {

			NewGlyphTreeWizard wizard(numberOfBranches, parent);
			if (wizard.exec() == QDialog::Accepted) {
			
				dataMappingGlyphGraph = wizard.GetNewDataMappingGlyphGraph();
			}
		}

		return dataMappingGlyphGraph;
	}

} //namespace SynGlyphX