#include "syncprogressdialog.h"
#include <QtWidgets\QProgressBar>
#include "SharedVisualizationsWidget.h"

namespace SynGlyphX {

	SyncProgressDialog::SyncProgressDialog(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, SharedVisualizationsWidget* allViewsFilteringWidget, QWidget *parent) :
		QDialog(parent),
		value(0),
		viz_count(0),
		m_dataEngineConnection(dataEngineConnection),
		m_allViewsFilteringWidget(allViewsFilteringWidget)
	{
		timer = new QTimer();
		timer->start(250);
		setMinimumSize(400, 150);

		layout = new QVBoxLayout(this);

		setWindowFlags(windowFlags() & Qt::FramelessWindowHint);

		layout->addWidget(GetSyncLabel());
		setStyleSheet("QDialog{background-color: #eff2f7; border: 2px solid gray;}");

		progress = new QProgressBar(this);
		progress->setRange(0, 100);
		progress->setAlignment(Qt::AlignCenter);
		progress->setValue(0);
		progress->setStyleSheet("QProgressBar{font-size: 12.5pt; font-weight: bold;}");
		layout->addWidget(progress);

		file_count = m_dataEngineConnection->UserAccessControls()->FileSyncSetup(m_dataEngineConnection->GetGlyphEdPath());
		if (file_count == 0){
			syncLabel->setText(tr("Loading visualizations and establishing data connection..."));
			progress->show();
		}
		else{
			progress->hide();
		}

		connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeOut()));
		connect(progress, SIGNAL(progress->valueChanged(int)), this, SLOT(progress->setValue(int)));

		setLayout(layout);

		viz_count = m_dataEngineConnection->UserAccessControls()->VisualizationsToSync();
		m_dataEngineConnection->UserAccessControls()->StartSyncingFiles();
	}

	QLabel* SyncProgressDialog::GetSyncLabel()
	{
		syncLabel = new QLabel(this);
		syncLabel->setAlignment(Qt::AlignCenter);
		syncLabel->setWordWrap(true);
		syncLabel->setTextFormat(Qt::RichText);
		syncLabel->setStyleSheet("QLabel{font-size: 12.5pt; font-weight: bold;}");

		return syncLabel;
	}

	void SyncProgressDialog::UpdateSyncLabel()
	{
		std::string s = "Syncing " + std::to_string(value) + " out of " + std::to_string(viz_count) + " visualizations...";
		syncLabel->setText(tr(s.c_str()));
	}

	void SyncProgressDialog::handleTimeOut()
	{
		if ((value > file_count) || file_count == 0)
		{
			timer->stop();

			m_allViewsFilteringWidget->Setup(m_dataEngineConnection, progress);
			progress->setValue(100);

			QTimer::singleShot(1000, this, SLOT(close()));
		}
		else
		{
			if ((value <= viz_count) && viz_count != 0)
			{
				UpdateSyncLabel();
				value = m_dataEngineConnection->UserAccessControls()->FilesSynced();
			}
			else
			{
				std::string s = "Syncing additional files...";
				syncLabel->setText(tr(s.c_str()));
				value = m_dataEngineConnection->UserAccessControls()->FilesSynced();
			}

			if (value > file_count){
				syncLabel->setText(tr("Loading visualizations and establishing data connection..."));
				progress->show();
			}

		}
	}

}