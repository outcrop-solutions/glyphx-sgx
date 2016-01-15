#include "mainwindow.h"
#include <QtCore/QSettings>
#include <QtWidgets/QAction>
#include <QtCore/QFileInfo>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include "application.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include "licensingdialog.h"

namespace SynGlyphX {

	const QString MainWindow::s_copyright = QString::fromStdWString(L"Copyright © 2013-2015 SynGlyphX Holdings Incorporated. All Rights Reserved.\n\nSynGlyphX, Glyph IT, Glyph KIT are either registered trademarks or trademarks of SynGlyphX Holdings Incorporated in the United States and/or other countries.  All other trademarks are the property of their respective owners.");
	const QString MainWindow::s_fileDialogSettingsGroup = "FileDialogSettings";

	MainWindow::MainWindow(unsigned int stateVersion, QWidget *parent)
        : QMainWindow(parent),
		m_stateVersion(stateVersion),
		m_needToReadSettings(true)
    {
        //Make sure Status Bar gets created for all applications
        statusBar();

        //Add an additional 
        for (int i = 0; i < MaxRecentFiles + 1; ++i) {
            QAction* recentFileAction = new QAction(this);
            recentFileAction->setVisible(false);
            if (i != 0) {
                QObject::connect(recentFileAction, &QAction::triggered, this, &MainWindow::OnRecentFileSelected);
            }
            m_recentFileActions.push_back(recentFileAction);
        }
        m_recentFileActions[0]->setSeparator(true);

		ClearCurrentFile();
    }

    MainWindow::~MainWindow()
    {

    }

    void MainWindow::ReadSettings() {

        QSettings settings;

		settings.beginGroup("GlyphTemplates");
		m_glyphTemplatesDirectory = settings.value("dir", SynGlyphX::Application::applicationDirPath() + QDir::separator() + "GlyphTemplates").toString();
		settings.endGroup();

        settings.beginGroup("Window");
        resize(settings.value("size", QSize(1100, 820)).toSize());
		QByteArray geometry = settings.value("geometry").toByteArray();
		if (geometry.isEmpty()) {

			move(100, 100);
		}
		else {

			restoreGeometry(geometry);
		}
		restoreState(settings.value("state").toByteArray(), m_stateVersion);
        settings.endGroup();

        UpdateRecentFileList();
    }

    void MainWindow::WriteSettings() {

        QSettings settings;
        settings.beginGroup("Window");
        settings.setValue("size", size());
        settings.setValue("geometry", saveGeometry());
		settings.setValue("state", saveState(m_stateVersion));
        settings.endGroup();
    }

	void MainWindow::showEvent(QShowEvent* event) {

		if (m_needToReadSettings) {

			SaveOriginalState();
			ReadSettings();
			m_needToReadSettings = false;
		}
		QMainWindow::showEvent(event);
	}

    void MainWindow::closeEvent(QCloseEvent* event) {

        WriteSettings();
        QMainWindow::closeEvent(event);
    }

    void MainWindow::OnRecentFileSelected() {

        QAction *action = qobject_cast<QAction *>(sender());
        if (action != NULL) {

			QString recentFile = action->data().toString();
			if (!LoadRecentFile(recentFile)) {

				if (QMessageBox::question(this, tr("Recent File Failed To Load"), tr("The selected recent file failed to load.  Do you wish to remove it from the recent file list?")) == QMessageBox::Yes) {

					QSettings settings;
					settings.beginGroup("RecentFiles");
					QStringList files = settings.value("recentFileList").toStringList();
					settings.endGroup();

					files.removeAll(recentFile);

					settings.beginGroup("RecentFiles");
					settings.setValue("recentFileList", files);
					settings.endGroup();

					UpdateRecentFileList();
				}
			}
        }
    }

    void MainWindow::UpdateRecentFileList() {

        QSettings settings;
        settings.beginGroup("RecentFiles");
        QStringList files = settings.value("recentFileList").toStringList();
        settings.endGroup();

        int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFiles));

        for (int i = 1; i < numRecentFiles + 1; ++i) {
            QString text = tr("&%1 %2").arg(i).arg(QFileInfo(files[i - 1]).fileName());
            m_recentFileActions[i]->setText(text);
            m_recentFileActions[i]->setData(files[i - 1]);
            m_recentFileActions[i]->setVisible(true);
        }
        for (int j = numRecentFiles + 1; j < MaxRecentFiles + 1; ++j)
            m_recentFileActions[j]->setVisible(false);

        //If there are any files in the recent file list make the separator visible
        m_recentFileActions[0]->setVisible(numRecentFiles > 0);
    }

	void MainWindow::UpdateFilenameWindowTitle(const QString& title) {

        setWindowTitle(title + "[*] - " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
    }

	void MainWindow::ClearCurrentFile() {

		m_currentFilename = "";
		UpdateFilenameWindowTitle("Untitled");
	}

    void MainWindow::SetCurrentFile(const QString& filename) {

        m_currentFilename = filename;

        QSettings settings;
        settings.beginGroup("RecentFiles");
        QStringList files = settings.value("recentFileList").toStringList();
        settings.endGroup();

        files.removeAll(filename);
        files.prepend(filename);
        while (files.size() > MaxRecentFiles) {
            files.removeLast();
        }

        settings.beginGroup("RecentFiles");
        settings.setValue("recentFileList", files);
        settings.endGroup();

        UpdateRecentFileList();
        UpdateFilenameWindowTitle(QFileInfo(filename).fileName());
        setWindowModified(false);
    }

    QAction* MainWindow::CreateMenuAction(QMenu* menu, const QString& title, QKeySequence shortcut) {

        QString titleWithShortcut = title;
#ifdef WIN32
        //ALT+F4 exits on windows but does not show up in the menu even though the key sequence exits the program so add the sequence to the title manually
        if (title == "Exit") {
            titleWithShortcut += "\tAlt+F4";
        } 
#endif
        QAction* action = menu->addAction(titleWithShortcut);
        action->setShortcut(shortcut);
        return action;
    }

    void MainWindow::CreateHelpMenu() {

        m_helpMenu = menuBar()->addMenu(tr("Help"));

		QAction* licensingAction = m_helpMenu->addAction(tr("Licensing"));
		QObject::connect(licensingAction, &QAction::triggered, this, &MainWindow::ShowLicensingDialog);

		m_helpMenu->addSeparator();

        m_aboutBoxAction = m_helpMenu->addAction("About " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
		QObject::connect(m_aboutBoxAction, &QAction::triggered, this, &MainWindow::ShowAboutBox);
    }

	void MainWindow::ShowLicensingDialog() {

		SynGlyphX::LicensingDialog dialog(this);
		dialog.exec();
	}

    void MainWindow::ShowAboutBox() {

        QString appName = SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
		QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion() + "\n\n" + s_copyright);
    }

    void MainWindow::SwitchBetweenFullAndNormalScreen() {

        if (isFullScreen()) {
            showNormal();
            m_fullScreenAction->setText(tr("Full Screen"));
        }
        else {
            showFullScreen();
            m_fullScreenAction->setText(tr("Return to window"));
        }
    }

	void MainWindow::CreateViewMenu() {

		m_viewMenu = menuBar()->addMenu(tr("View"));

		QAction* restoreLayoutAction = CreateMenuAction(m_viewMenu, tr("Restore Original Layout"));
		QObject::connect(restoreLayoutAction, &QAction::triggered, this, &MainWindow::RestoreOriginalLayout);

        m_fullScreenAction = CreateMenuAction(m_viewMenu, tr("Full Screen"), QKeySequence::FullScreen);
        QObject::connect(m_fullScreenAction, &QAction::triggered, this, &MainWindow::SwitchBetweenFullAndNormalScreen);

		m_viewMenu->addSeparator();
    }

	QString MainWindow::GetFileNameOpenDialog(const QString& settingKey, const QString& caption, const QString& defaultDir, const QString& filter) {

		QSettings settings;
		settings.beginGroup(s_fileDialogSettingsGroup);
		QString initialDir = settings.value(settingKey, defaultDir).toString();

		QString filename = QFileDialog::getOpenFileName(this, caption, initialDir, filter);
		if (!filename.isEmpty()) {

			QFileInfo fileInfo(filename);
			settings.setValue(settingKey, fileInfo.absolutePath());
		}

		settings.endGroup();
		return filename;
	}

	QStringList MainWindow::GetFileNamesOpenDialog(const QString& settingKey, const QString& caption, const QString& defaultDir, const QString& filter) {

		QSettings settings;
		settings.beginGroup(s_fileDialogSettingsGroup);
		QString initialDir = settings.value(settingKey, defaultDir).toString();

		QStringList filenames = QFileDialog::getOpenFileNames(this, caption, initialDir, filter);
		if (!filenames.isEmpty()) {

			QFileInfo fileInfo(filenames[0]);
			settings.setValue(settingKey, fileInfo.absolutePath());
		}

		settings.endGroup();
		return filenames;
	}

	QString MainWindow::GetFileNameSaveDialog(const QString& settingKey, const QString& caption, const QString& defaultDir, const QString& filter) {

		QSettings settings;
		settings.beginGroup(s_fileDialogSettingsGroup);
		QString initialDir = settings.value(settingKey, defaultDir).toString();

		QString filename = QFileDialog::getSaveFileName(this, caption, initialDir, filter);
		if (!filename.isEmpty()) {

			QFileInfo fileInfo(filename);
			settings.setValue(settingKey, fileInfo.absolutePath());
		}

		settings.endGroup();
		return filename;
	}

	QString MainWindow::GetExistingDirectoryDialog(const QString& settingKey, const QString& caption, const QString& defaultDir) {

		QSettings settings;
		settings.beginGroup(s_fileDialogSettingsGroup);
		QString initialDir = settings.value(settingKey, defaultDir).toString();

		QString directory = QFileDialog::getExistingDirectory(this, caption, initialDir);
		if (!directory.isEmpty()) {

			settings.setValue(settingKey, directory);
		}

		settings.endGroup();
		return directory;
	}

	QString MainWindow::GetExistingEmptyDirectory(const std::set<QDir> invalidDirectories, const QString& settingKey, const QString& caption, const QString& defaultDir, const QString& invalidDirError) {
		
		QString directoryName;
		bool isDirectoryInvalid = false;

		do {

			directoryName = QDir::toNativeSeparators(GetExistingDirectoryDialog(settingKey, caption, defaultDir));
			if (directoryName.isEmpty()) {

				break;
			}

			QDir dir(directoryName);
			bool doesDirectoryMatchAnInvalidDirectory = false;

			for (const auto& invalidDir : invalidDirectories) {

				if (invalidDir == dir) {

					doesDirectoryMatchAnInvalidDirectory = true;
					break;
				}
			}

			if (doesDirectoryMatchAnInvalidDirectory) {

				QMessageBox::warning(this, tr("Invalid Directory"), invalidDirError + tr("  Directory can not be used.  Select another directory."));
				isDirectoryInvalid = true;
			}
			else if (dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() > 0) {

				if (QMessageBox::question(this, tr("Directory isn't empty"), tr("Selected directory is not empty.  All contents of the directory will be deleted before export.  Do you wish to continue?")) == QMessageBox::No) {

					break;
				}
				else {

					isDirectoryInvalid = false;
				}
			}

		} while (isDirectoryInvalid);

		return directoryName;
	}

	void MainWindow::SaveOriginalState() {

		m_originalState = saveState();
	}

	void MainWindow::RestoreOriginalLayout() {

		if (!m_originalState.isEmpty()) {

			restoreState(m_originalState);
		}
	}

} //namespace SynGlyphX