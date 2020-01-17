#include "LegendsWidget.h"
#include <QtWidgets/QVBoxLayout>
#include "ResizeableImageLabel.h"

LegendsWidget::LegendsWidget(QWidget *parent)
	: QDialog(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	m_legendTabs = new QTabWidget(this);
	mainLayout->addWidget(m_legendTabs);

	setLayout(mainLayout);
}

LegendsWidget::~LegendsWidget()
{

}

void LegendsWidget::SetLegends(QString cache_location, QStringList legends) {

	ClearLegends();

	for (QString legend : legends) {

		SynGlyphX::ResizeableImageLabel* legendLabel = new SynGlyphX::ResizeableImageLabel(true, m_legendTabs);

		QPixmap pixmap(cache_location + "/" + legend);
		legendLabel->SetPixmap(pixmap);

		m_legendTabs->addTab(legendLabel, legend.split(".")[0]);
	}
}

void LegendsWidget::ClearLegends() {

	m_legendTabs->clear();
}

bool LegendsWidget::HasLegends() const {

	return (m_legendTabs->count() != 0);
}
