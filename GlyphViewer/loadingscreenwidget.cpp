#include "loadingscreenwidget.h"
#include "glyphbuilderapplication.h"
#include <QtCore/QDir>
#include <QtWidgets/QHBoxLayout>
#include "radiobuttongroupwidget.h"
#include "groupboxsinglewidget.h"

QString LoadingScreenWidget::s_glyphEdDir;

LoadingScreenWidget::LoadingScreenWidget(QWidget *parent)
	: QFrame(parent)
{
	setFrameShape(QFrame::Shape::Box);
	setFrameShadow(QFrame::Shadow::Sunken);
	
	QHBoxLayout* mainLayout = new QHBoxLayout(this);

	QStringList visualizationNames;
	visualizationNames << "Global Admissions View" << "Admissions Officer View" << "High School View";
	m_viewListWidget = new QListWidget(this);
	m_viewListWidget->addItems(visualizationNames);

	SynGlyphX::GroupBoxSingleWidget* visualizationGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("View(s)"), m_viewListWidget, this);
	mainLayout->addWidget(visualizationGroupBox);

	mainLayout->addStretch(1);

	setLayout(mainLayout);
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