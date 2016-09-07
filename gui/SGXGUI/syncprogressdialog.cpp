#include "syncprogressdialog.h"

namespace SynGlyphX {

	SyncProgressDialog::SyncProgressDialog(int files2sync, DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent) :
		QDialog(parent),
		value(0),
		viz_count(0),
		file_count(files2sync),
		m_dataEngineConnection(dataEngineConnection)
	{
		timer = new QTimer();
		timer->start(250);
		setMinimumSize(400, 150);

		layout = new QVBoxLayout(this);

		setWindowFlags(windowFlags() & Qt::FramelessWindowHint);

		layout->addWidget(GetSyncLabel());

		connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeOut()));

		setLayout(layout);

		viz_count = m_dataEngineConnection->UserAccessControls()->VisualizationsToSync();
		m_dataEngineConnection->UserAccessControls()->StartSyncingFiles();
	}

	QLabel* SyncProgressDialog::GetSyncLabel()
	{
		std::string s = "Syncing " + std::to_string(value) + " out of " + std::to_string(viz_count) + " visualizations...";
		syncLabel = new QLabel(tr(s.c_str()), this);
		syncLabel->setAlignment(Qt::AlignCenter);
		syncLabel->setWordWrap(true);
		syncLabel->setTextFormat(Qt::RichText);
		syncLabel->setStyleSheet("QLabel{font-size: 12.5pt; font-weight: bold; background-color: #eff2f7; border: 2px solid gray;}");

		return syncLabel;
	}

	void SyncProgressDialog::UpdateSyncLabel()
	{
		std::string s = "Syncing " + std::to_string(value) + " out of " + std::to_string(viz_count) + " visualizations...";
		syncLabel->setText(tr(s.c_str()));
	}

	void SyncProgressDialog::handleTimeOut()
	{
		if (value > file_count)
		{
			syncLabel->setText(tr("Syncing Complete!"));
			QTimer::singleShot(1000, this, SLOT(close()));
		}
		else
		{
			if (value <= viz_count)
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

		}
	}

}