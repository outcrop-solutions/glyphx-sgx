#include "legendsdisplaywidget.h"
#include <QtWidgets/QVBoxLayout>
#include "imagelabel.h"

LegendsDisplayWidget::LegendsDisplayWidget(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	m_legendTabs = new QTabWidget(this);
	mainLayout->addWidget(m_legendTabs);

	setLayout(mainLayout);
}

LegendsDisplayWidget::~LegendsDisplayWidget()
{

}

void LegendsDisplayWidget::SetLegends(const std::vector<SynGlyphX::Legend>& legends) {

	ClearLegends();

	for (const auto& legend : legends) {

		SynGlyphX::ImageLabel* legendLabel = new SynGlyphX::ImageLabel(m_legendTabs);

		QPixmap pixmap(QString::fromStdWString(legend.GetFilename()));
		legendLabel->SetPixmap(pixmap);
		
		m_legendTabs->addTab(legendLabel, QString::fromStdWString(legend.GetTitle()));
	}
}

void LegendsDisplayWidget::ClearLegends() {

	m_legendTabs->clear();
}