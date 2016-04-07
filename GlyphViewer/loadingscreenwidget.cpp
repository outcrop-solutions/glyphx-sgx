#include "loadingscreenwidget.h"
#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>

QString LoadingScreenWidget::s_glyphEdDir;

LoadingScreenWidget::LoadingScreenWidget(QWidget *parent)
	: QFrame(parent)
{
	setFrameShape(QFrame::Shape::Box);
	setFrameShadow(QFrame::Shadow::Sunken);
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* vizAndFilterLayout = new QHBoxLayout(this);

	QStringList visualizationNames;
	visualizationNames << "Global Admissions View" << "Admissions Officer View" << "High School View";
	m_viewListWidget = new QListWidget(this);
	m_viewListWidget->setSortingEnabled(false);
	m_viewListWidget->addItems(visualizationNames);
	QObject::connect(m_viewListWidget, &QListWidget::itemSelectionChanged, this, &LoadingScreenWidget::OnNewViewSelected);

	SynGlyphX::GroupBoxSingleWidget* visualizationGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("View(s)"), m_viewListWidget, this);
	vizAndFilterLayout->addWidget(visualizationGroupBox);

	m_filterWidgets.push_back(new QListWidget(this));

	m_filterGroupBoxWidgets.push_back(new SynGlyphX::GroupBoxSingleWidget(tr("Admissions Officer"), m_filterWidgets[0], this));
	vizAndFilterLayout->addWidget(m_filterGroupBoxWidgets[0]);

	m_filterWidgets.push_back(new QListWidget(this));

	m_filterGroupBoxWidgets.push_back(new SynGlyphX::GroupBoxSingleWidget(tr("Year(s)"), m_filterWidgets[1], this));
	vizAndFilterLayout->addWidget(m_filterGroupBoxWidgets[1]);

	vizAndFilterLayout->addStretch(1);

	mainLayout->addLayout(vizAndFilterLayout, 1);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);

	QPushButton* loadButton = new QPushButton(tr("Load Visualization"), this);
	buttonLayout->addWidget(loadButton, 1);
	QObject::connect(loadButton, &QPushButton::clicked, this, &LoadingScreenWidget::OnLoadVisualization);

	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);

	m_viewListWidget->selectionModel()->select(m_viewListWidget->model()->index(0, 0), QItemSelectionModel::SelectionFlag::ClearAndSelect);
}

LoadingScreenWidget::~LoadingScreenWidget()
{

}

bool LoadingScreenWidget::DoesGlyphEdDirExist() {

	if (s_glyphEdDir.isEmpty()) {

		s_glyphEdDir = QDir::toNativeSeparators(QDir::cleanPath(SynGlyphX::GlyphBuilderApplication::applicationDirPath()) + QDir::separator() + "GlyphEd");
	}

	return QDir(s_glyphEdDir).exists();
}

void LoadingScreenWidget::OnLoadVisualization() {

	if (m_filterWidgets[0]->selectionModel()->selectedIndexes().empty() || m_filterWidgets[1]->selectionModel()->selectedIndexes().empty()) {

		QMessageBox::information(this, tr("Did not load visualization"), tr("Visualization can not be loaded until at least one value has been selected from each filter."));
	}
	else {


	}
}

void LoadingScreenWidget::OnNewViewSelected() {

	m_filterWidgets[0]->selectionModel()->clearSelection();
	m_filterWidgets[1]->selectionModel()->clearSelection();

	unsigned int row = m_viewListWidget->selectionModel()->selectedIndexes().front().row();
	if (row == 2) {

		m_filterGroupBoxWidgets[0]->setTitle(tr("High School"));
		m_filterGroupBoxWidgets[1]->setTitle(tr("Years(s)"));

		m_filterWidgets[0]->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		m_filterWidgets[1]->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	}
	else if (row == 1) {

		m_filterGroupBoxWidgets[0]->setTitle(tr("Admissions Officer"));
		m_filterGroupBoxWidgets[1]->setTitle(tr("Decision Status"));
		m_filterWidgets[0]->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		m_filterWidgets[1]->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	}
	else {

		m_filterGroupBoxWidgets[0]->setTitle(tr("Admissions Officer"));
		m_filterGroupBoxWidgets[1]->setTitle(tr("Years(s)"));
		m_filterWidgets[0]->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		m_filterWidgets[1]->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	}
}