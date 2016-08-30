#include "SettingsStoredFileList.h"
#include <QtCore/QSettings>

namespace SynGlyphX {

	SettingsStoredFileList::SettingsStoredFileList(const QString& settingsName, unsigned int maxFileCount) :
		m_settingsName(settingsName),
		m_maxFileCount(maxFileCount)
	{
	}


	SettingsStoredFileList::~SettingsStoredFileList()
	{
	}

	const QStringList& SettingsStoredFileList::GetFiles() const {

		return m_files;
	}

	unsigned int SettingsStoredFileList::GetMaxFileCount() const {

		return m_maxFileCount;
	}

	void SettingsStoredFileList::ReadFromSettings() {

		QSettings settings;
		settings.beginGroup("FileLists");
		m_files = settings.value(m_settingsName).toStringList();
		settings.endGroup();

		emit FileListChanged();
	}

	void SettingsStoredFileList::WriteToSettings() {

		QSettings settings;
		settings.beginGroup("FileLists");
		settings.setValue(m_settingsName, m_files);
		settings.endGroup();
	}

	void SettingsStoredFileList::RemoveFile(const QString& filename) {

		m_files.removeAll(filename);
		emit FileListChanged();
	}

	void SettingsStoredFileList::AddFile(const QString& filename) {

		RemoveFile(filename);
		m_files.prepend(filename);

		if (m_maxFileCount > 0) {

			while (m_files.size() > m_maxFileCount) {
				
				m_files.removeLast();
			}
		}
		
		emit FileListChanged();
	}

	void SettingsStoredFileList::ClearFiles() {

		m_files.clear();
		emit FileListChanged();
	}

} //namespace SynGlyphX