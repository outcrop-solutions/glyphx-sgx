#include "mainwindow.h"
#include <QtCore/QSettings>
#include <QtWidgets/QAction>
#include <QtCore/QFileInfo>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include "application.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include "licensingdialog.h"
#include "helpdialog.h"
#include <QtCore/QStandardPaths>
#include <QtWidgets/QUndoStack>
#include "singlewidgetdialog.h"
#include <QtWidgets/QToolButton>

namespace SynGlyphX {

	const QString MainWindow::s_copyright = QString::fromStdWString(L"Copyright © 2013-2015 SynGlyphX Holdings Incorporated. All Rights Reserved.\n\nSynGlyphX, Glyph IT, Glyph KIT are either registered trademarks or trademarks of SynGlyphX Holdings Incorporated in the United States and/or other countries.  All other trademarks are the property of their respective owners.");
	const QString MainWindow::s_fileDialogSettingsGroup = "FileDialogSettings";
	const QString MainWindow::s_noFileName = "Untitled";

	SettingsStoredFileList MainWindow::s_recentFileList("recentFileList", MainWindow::MaxRecentFiles);

	MainWindow::MainWindow(unsigned int stateVersion, QWidget *parent)
        : QMainWindow(parent),
		m_stateVersion(stateVersion),
		m_needToReadSettings(true)
    {
        //Make sure Status Bar gets created for all applications
        statusBar();

        //Add an additional 
		for (int i = 0; i < MaxRecentFileMenuEntries + 1; ++i) {
            QAction* recentFileAction = new QAction(this);
            recentFileAction->setVisible(false);
            if (i != 0) {
                QObject::connect(recentFileAction, &QAction::triggered, this, &MainWindow::OnRecentFileSelected);
            }
            m_recentFileActions.push_back(recentFileAction);
        }
        m_recentFileActions[0]->setSeparator(true);

		ClearCurrentFile();
		m_undoStack = new QUndoStack(this);

		QObject::connect(&s_recentFileList, &SettingsStoredFileList::FileListChanged, this, &MainWindow::UpdateRecentFileList);

		//userMenuBar = new QMenuBar(this);
    }

    MainWindow::~MainWindow()
    {

    }

    void MainWindow::ReadSettings() {

        QSettings settings;

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

		s_recentFileList.ReadFromSettings();
    }

    void MainWindow::WriteSettings() {

        QSettings settings;
        settings.beginGroup("Window");
        settings.setValue("size", size());
        settings.setValue("geometry", saveGeometry());
		settings.setValue("state", saveState(m_stateVersion));
        settings.endGroup();

		s_recentFileList.WriteToSettings();
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

					s_recentFileList.RemoveFile(recentFile);
				}
			}
        }
    }

    void MainWindow::UpdateRecentFileList() {

		const QStringList& files = s_recentFileList.GetFiles();

		int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFileMenuEntries));

        for (int i = 1; i < numRecentFiles + 1; ++i) {
            QString text = tr("&%1 %2").arg(i).arg(QFileInfo(files[i - 1]).fileName());
            m_recentFileActions[i]->setText(text);
            m_recentFileActions[i]->setData(files[i - 1]);
            m_recentFileActions[i]->setVisible(true);
        }
		for (int j = numRecentFiles + 1; j < MaxRecentFileMenuEntries + 1; ++j)
            m_recentFileActions[j]->setVisible(false);

        //If there are any files in the recent file list make the separator visible
        m_recentFileActions[0]->setVisible(numRecentFiles > 0);
    }

	void MainWindow::UpdateFilenameWindowTitle(const QString& title) {

		setWindowTitle(title + "[*] - " + GetApplicationDisplayName());
    }

	QString MainWindow::GetApplicationDisplayName() const {

		return SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName();
	}

	void MainWindow::ClearCurrentFile() {

		m_currentFilename = "";
		UpdateFilenameWindowTitle(s_noFileName);
	}

    void MainWindow::SetCurrentFile(const QString& filename) {

        m_currentFilename = filename;

		s_recentFileList.AddFile(filename);

        UpdateFilenameWindowTitle(QFileInfo(filename).completeBaseName());
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

		if (DoesHelpExist()) {

			m_fullHelpMenu = CreateMenuAction(m_helpMenu, tr("Guide"), QKeySequence::HelpContents);
			QObject::connect(m_fullHelpMenu, &QAction::triggered, this, &MainWindow::ShowTutorial);
		}

		QAction* licensingAction = m_helpMenu->addAction(tr("Licensing"));
		QObject::connect(licensingAction, &QAction::triggered, this, &MainWindow::ShowLicensingDialog);

		m_helpMenu->addSeparator();

		m_aboutBoxAction = m_helpMenu->addAction("About " + GetApplicationDisplayName());
		QObject::connect(m_aboutBoxAction, &QAction::triggered, this, &MainWindow::ShowAboutBox);
    }

	void MainWindow::CreateLoginMenu() {

		//QString user = "Log In " + QString(QChar(0x23F7));
		//menuBar()->setCornerWidget(userMenuBar, Qt::TopRightCorner);
		m_loginMenu = menuBar()->addMenu(tr("User"));
		m_loginMenu->menuAction()->setVisible(false);

	}

	void MainWindow::UpdateUserMenu(QString username){

		//userMenuBar->clear();
		//m_loginMenu->setVisible(true);
		QString user = username + " " + QString(QChar(0x23F7));
		//m_loginMenu = userMenuBar->addMenu(tr(user.toStdString().c_str()));
		m_loginMenu->setTitle(username);


		m_userSettingsMenu = m_loginMenu->addAction("User Settings");
		QObject::connect(m_userSettingsMenu, &QAction::triggered, this, &MainWindow::UserSettings);

		m_loginMenu->addSeparator();

		m_logoutMenu = m_loginMenu->addAction("Log Out");

		m_loginMenu->menuAction()->setVisible(true);
		//menuBar()->setCornerWidget(userMenuBar, Qt::TopRightCorner);
	}

	QAction* MainWindow::LogoutMenu(){
		return m_logoutMenu;
	}

	void MainWindow::UserSettings() {

		QDialog* s = new QDialog(this);
		s->setWindowTitle(tr("User Settings"));
		QVBoxLayout* layout = new QVBoxLayout(s);
		QWidget *placeHolder = new QWidget(s);
		placeHolder->setMinimumSize(300, 300);
		layout->addWidget(placeHolder);
		s->setLayout(layout);
		s->exec();
	}

	void MainWindow::UserLogOut(){

		//userMenuBar->clear();
		m_loginMenu->clear();
	}

	bool MainWindow::HasOpenFile(){
		if (m_currentFilename == ""){
			return false;
		}
		return true;
	}

	void MainWindow::ShowLicensingDialog() {
#ifdef USE_LICENSING
		SynGlyphX::LicensingDialog dialog(this);
		dialog.exec();
#endif
	}

	void MainWindow::ShowTutorial() {

		SingleWidgetDialog* helpDialog = new SingleWidgetDialog(QDialogButtonBox::StandardButton::Close, new SynGlyphX::HelpDialog(970, 920, this), this);
		helpDialog->setWindowTitle(tr("Help"));
		helpDialog->show();
	}


    void MainWindow::ShowAboutBox() {

		QString appName = GetApplicationDisplayName();
		QMessageBox::about(this, "About " + appName, appName + " " + SynGlyphX::Application::applicationVersion() + "\n"+de_version+"\n\n" + s_copyright);
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

	void MainWindow::CreateEditMenu() {
		//Create Edit Menu
		m_editMenu = menuBar()->addMenu(tr("Edit"));
		m_undoAction = m_undoStack->createUndoAction(this, tr("&Undo"));
		m_undoAction->setShortcuts(QKeySequence::Undo);
		m_redoAction = m_undoStack->createRedoAction(this, tr("&Redo"));
		m_redoAction->setShortcuts(QKeySequence::Redo);

		m_editMenu->addAction(m_undoAction);
		m_editMenu->addAction(m_redoAction);
		m_editMenu->addSeparator();
	}

	QString MainWindow::GetFileNameOpenDialog(const QString& settingKey, const QString& caption, const QString& defaultDir, const QString& filter) {

		QSettings settings;
		settings.beginGroup(s_fileDialogSettingsGroup);
		QString initialDir = settings.value(settingKey, defaultDir).toString();
		if (initialDir.isEmpty()) {

			initialDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		}

		QString filename = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, caption, initialDir, filter));
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
		if (initialDir.isEmpty()) {

			initialDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		}

		QStringList filenames = QFileDialog::getOpenFileNames(this, caption, initialDir, filter);
		for (unsigned int i = 0; i < filenames.size(); ++i) {

			filenames[i] = QDir::toNativeSeparators(filenames[i]);
		}
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
		if (initialDir.isEmpty()) {

			initialDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		}

		QString filename = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, caption, initialDir, filter));
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

	QString MainWindow::GetExistingEmptyDirectory(const QSet<QString> invalidDirectories, const QString& settingKey, const QString& caption, const QString& defaultDir, const QString& invalidDirError) {
		
		QString directoryName;
		bool isDirectoryInvalid = false;

		do {

			directoryName = QDir::toNativeSeparators(GetExistingDirectoryDialog(settingKey, caption, defaultDir));
			if (directoryName.isEmpty()) {

				break;
			}

			bool doesDirectoryMatchAnInvalidDirectory = false;

			for (const auto& invalidDir : invalidDirectories) {

				if (invalidDir == directoryName) {

					doesDirectoryMatchAnInvalidDirectory = true;
					break;
				}
			}

			if (doesDirectoryMatchAnInvalidDirectory) {

				QMessageBox::warning(this, tr("Invalid Directory"), invalidDirError + tr("  Directory can not be used.  Select another directory."));
				isDirectoryInvalid = true;
			}
			else if (QDir(directoryName).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() > 0) {

				if (QMessageBox::question(this, tr("Directory isn't empty"), tr("Selected directory is not empty.  All contents of the directory will be deleted before export.  Do you wish to continue?")) == QMessageBox::No) {

					directoryName.clear();
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

	const SettingsStoredFileList& MainWindow::GetRecentFileListInstance() {

		return s_recentFileList;
	}

	bool MainWindow::DoesHelpExist() const {

		return false;
	}

} //namespace SynGlyphX
