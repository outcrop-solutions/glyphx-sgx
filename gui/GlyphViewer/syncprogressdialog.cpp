#include "syncprogressdialog.h"
#include <QtWidgets/QProgressBar>
#include "SharedVisualizationsWidget.h"
#include "application.h"

namespace SynGlyphX {

	SyncProgressDialog::SyncProgressDialog(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, SharedVisualizationsWidget* allViewsFilteringWidget, QWidget *parent) :
		QDialog(parent),
        m_allViewsFilteringWidget(allViewsFilteringWidget),
		m_dataEngineConnection(dataEngineConnection)
	{

		setMinimumSize(Application::DynamicQSize(400, 150));

		layout = new QVBoxLayout(this);

		setWindowFlags(Qt::FramelessWindowHint);

		syncLabel = new QLabel(this);
		syncLabel->setAlignment(Qt::AlignCenter);
		syncLabel->setWordWrap(true);
		syncLabel->setTextFormat(Qt::RichText);
		syncLabel->setStyleSheet("QLabel{font-size: 12.5pt; font-weight: bold;}");
		layout->addWidget(syncLabel);

		setStyleSheet("QDialog{background-color: #eff2f7; border: 2px solid gray;}");

		progress = new QProgressBar(this);
		progress->setRange(0, 100);
		progress->setAlignment(Qt::AlignCenter);
		progress->setValue(0);
		progress->setStyleSheet("QProgressBar{font-size: 12.5pt; font-weight: bold;}");
		layout->addWidget(progress);

		bool needToSync = m_dataEngineConnection->UserAccessControls()->FileSyncSetup(m_dataEngineConnection->GetGlyphEdPath());
		if (!needToSync){
			syncLabel->setText(tr("Loading visualizations and establishing data connection..."));
		}
		else{
			syncLabel->setText(tr("Syncing updated visualizations and data..."));
		}

		timer = new QTimer();
		timer->start(250);

		connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeOut()));
		connect(progress, SIGNAL(progress->valueChanged(int)), this, SLOT(progress->setValue(int)));

		setLayout(layout);

		QPoint dialogCenter = mapToGlobal(rect().center());
		QPoint parentWindowCenter = parent->window()->mapToGlobal(
			parent->window()->rect().center());
		move(parentWindowCenter - dialogCenter);

		m_dataEngineConnection->UserAccessControls()->StartSyncingFiles();

	}

	void SyncProgressDialog::handleTimeOut()
	{
		if (m_dataEngineConnection->UserAccessControls()->IsDoneSyncing())
		{
			timer->stop();

			progress->setValue(0);
			syncLabel->setText(tr("Loading visualizations and establishing data connection..."));
			m_allViewsFilteringWidget->Setup(m_dataEngineConnection, progress);
			progress->setValue(100);

			QTimer::singleShot(250, this, SLOT(close()));

		}
		else
		{
			progress->setValue(m_dataEngineConnection->UserAccessControls()->GetSyncProgress());
		}
	}

}
