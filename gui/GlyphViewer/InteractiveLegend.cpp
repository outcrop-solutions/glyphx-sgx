
#include "InteractiveLegend.h"
#include <QBoxLayout>
#include <QPushButton>
#include <SceneViewer/sceneviewer.h>
#include <SceneViewer/glyphnode.h>
#include <SceneViewer/glyphrenderer.h>
#include <algorithm>
#include "Glyph3DSceneExport.h"

InteractiveLegend::InteractiveLegend(QWidget *parent, SynGlyphX::DataTransformMapping::ConstSharedPtr _mapping, SynGlyphX::SceneViewer* _primary_viewer) : QWidget(parent), mapping(_mapping), primary_viewer(_primary_viewer)
{
	viewer = new SynGlyphX::SceneViewer(this, SynGlyphX::ViewerMode::SingleGlyph_AllowSelection);
	viewer->setMinimumSize(256, 256);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(viewer);

	setLayout(mainLayout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	QPushButton* toggle = new QPushButton("Show/Hide Element", this);
	QObject::connect(toggle, &QPushButton::clicked, this, [this]() {
		// gather selection
		std::unordered_set<uint32_t> selected;
		viewer->getScene().enumSelected([this, &selected](const SynGlyphX::Glyph3DNode& glyph) {
			selected.insert(glyph.getLabel());
		});

		// if any element in the selection is visible, hide all selected elements; otherwise, show them all
		bool hide = false;
		for (auto l : selected)
			if (hidden_elements.find(l) == hidden_elements.end())
				hide = true;

		if (hide)
		{
			hidden_elements.insert(selected.begin(), selected.end());
			primary_viewer->getScene().enumGlyphs([this, &selected](const SynGlyphX::Glyph3DNode& glyph) {
				if (selected.find(glyph.getLabel()) != selected.end())
					glyph.setHiddenByLegend(true);
				return true;
			});
		}
		else
		{
			for (auto l : selected)
				hidden_elements.erase(l);
			primary_viewer->getScene().enumGlyphs([this, &selected](const SynGlyphX::Glyph3DNode& glyph) {
				if (selected.find(glyph.getLabel()) != selected.end())
					glyph.setHiddenByLegend(false);
				return true;
			});
		}

		primary_viewer->getScene().flagChanged();
		update_viewer();
	});
	toggle->setEnabled(false);
	buttonsLayout->addWidget(toggle);

	QPushButton* branch = new QPushButton("Show/Hide Branch", this);
	QObject::connect(branch, &QPushButton::clicked, this, [this]() {
		// gather selection
		std::unordered_set<uint32_t> selected;
		viewer->getScene().enumSelected([this, &selected](const SynGlyphX::Glyph3DNode& glyph) {
			selected.insert(glyph.getLabel());
			glyph.enumNodes([this, &selected](const SynGlyphX::Glyph3DNode& child) {
				selected.insert(child.getLabel());
				return true;
			});
		});

		// if any element in the selection is visible, hide all selected elements; otherwise, show them all
		bool hide = false;
		for (auto l : selected)
			if (hidden_elements.find(l) == hidden_elements.end())
				hide = true;

		if (hide)
		{
			hidden_elements.insert(selected.begin(), selected.end());
			primary_viewer->getScene().enumGlyphs([this, &selected](const SynGlyphX::Glyph3DNode& glyph) {
				if (selected.find(glyph.getLabel()) != selected.end())
					glyph.setHiddenByLegend(true);
				return true;
			});
		}
		else
		{
			for (auto l : selected)
				hidden_elements.erase(l);
			primary_viewer->getScene().enumGlyphs([this, &selected](const SynGlyphX::Glyph3DNode& glyph) {
				if (selected.find(glyph.getLabel()) != selected.end())
					glyph.setHiddenByLegend(false);
				return true;
			});
		}

		primary_viewer->getScene().flagChanged();
		update_viewer();
	});
	buttonsLayout->addWidget(branch);

	QPushButton* show_all = new QPushButton("Show All", this);
	QObject::connect(show_all, &QPushButton::clicked, this, [this]() {
		hidden_elements.clear();
		primary_viewer->getScene().enumGlyphs([this](const SynGlyphX::Glyph3DNode& glyph) {
			glyph.setHiddenByLegend(false);
			return true;
		});
		primary_viewer->getScene().flagChanged();
		update_viewer();
	});
	buttonsLayout->addWidget(show_all);

	mainLayout->addLayout(buttonsLayout);

	viewer->setOnSelectionChanged([toggle, branch, this](bool) {
		toggle->setEnabled(!viewer->getScene().selectionEmpty());
		branch->setEnabled(!viewer->getScene().selectionEmpty());
		viewer->resetSelectionAnimation();
	});
}

void InteractiveLegend::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	viewer->makeCurrent();
	viewer->clearScene();

	char buf[255];
	sprintf(buf, "sizeof(Glyph3DNode) = %i\n", sizeof(SynGlyphX::Glyph3DNode));
	OutputDebugStringA(buf);
	
	SynGlyphX::GlyphScene& scene = viewer->getScene();
	auto glyph = mapping->GetGlyphGraphs().begin()->second;
	scene.beginAdding(glyph->size());
	SynGlyphX::Glyph3DSceneExport::ExportMaxGlyphTo3DScene(*glyph, scene);
	scene.finishAdding();

	viewer->setResetPulseAnimOnSelectionChange(false);
	viewer->setHiddenElementMode(SynGlyphX::GlyphRenderer::HiddenElementMode::GreyedOut);
	viewer->resetCamera();
}

void InteractiveLegend::update_viewer()
{
	viewer->getScene().enumGlyphs([this](const SynGlyphX::Glyph3DNode& glyph) {
		glyph.setHiddenByLegend(hidden_elements.find(glyph.getLabel()) != hidden_elements.end());
		return true;
	});
	viewer->getScene().flagChanged();
	viewer->getScene().flagSelectionChanged();
}
