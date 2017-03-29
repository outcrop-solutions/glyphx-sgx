#include "helpdialog.h"
#include "application.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QStandardPaths>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>

namespace SynGlyphX {

	QWebEngineView* createHelpDialog( QString path, int width, int height, QWidget* parent )
	{
		QWebEngineView* dlg = new QWebEngineView( parent );
		dlg->setMinimumSize( width, height );
		dlg->load(QUrl(path));
		return dlg;
	}

	HelpDialog::HelpDialog(QString userDirectory, QWidget* parent)
		: QDialog(parent)
	{
		setWindowTitle(tr("Guide"));

		QString path("https://s3.amazonaws.com/glyphit/userguide/index.htm");
		if (QFileInfo(QCoreApplication::applicationFilePath()).fileName().contains("GlyphEd")) {
			path = "https://s3.amazonaws.com/glyphit/userguide/index.htm";
		}

		QVBoxLayout* mainLayout = new QVBoxLayout(this);

		m_stackedLayout = new QStackedLayout();
		m_stackedLayout->setContentsMargins(0, 0, 0, 0);
		m_stackedLayout->addWidget(CreateHelpFrame(path));
		if (!userDirectory.isNull() && QDir(userDirectory + "/guide").exists()){
			m_stackedLayout->addWidget(CreateHelpFrame(userDirectory + "/guide/index.htm"));
		}
		m_stackedLayout->setCurrentIndex(0);

		mainLayout->addLayout(m_stackedLayout);

		QDialogButtonBox* buttonBox = new QDialogButtonBox(this);

		if (!userDirectory.isNull() && QDir(userDirectory + "/guide").exists()){
			QObject::connect(toggleButton, &QPushButton::clicked, this, &HelpDialog::ChangeIndex);
		}
		QPushButton* exitButton = buttonBox->addButton(QDialogButtonBox::StandardButton::Close);
		QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

		mainLayout->addWidget(buttonBox);

		setLayout(mainLayout);
	}

	QFrame* HelpDialog::CreateHelpFrame(QString path)
	{
		QFrame* helpWidget = new QFrame(this);
		helpWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
		helpWidget->setLineWidth(2);
		helpWidget->setMidLineWidth(3);
		helpWidget->setStyleSheet("background-color: white;");

		w_guide = helpWidget->frameWidth() / 2;
		h_guide = helpWidget->frameRect().height();

		QHBoxLayout* helpLayout = new QHBoxLayout(this);
		helpLayout->setContentsMargins(0, 0, 0, 0);
		helpLayout->addStretch();
		helpLayout->addWidget(SynGlyphX::createHelpDialog(path, w_guide, h_guide, helpWidget));
		helpLayout->addStretch();
		helpWidget->setLayout(helpLayout);

		return helpWidget;
	}

	void HelpDialog::ChangeIndex()
	{
		int index = std::abs(m_stackedLayout->currentIndex() - 1);
		m_stackedLayout->setCurrentIndex(index);
		toggleButton->setText(index == 0 ? "View Visualization Guide" : "View User Guide");
	}
}
