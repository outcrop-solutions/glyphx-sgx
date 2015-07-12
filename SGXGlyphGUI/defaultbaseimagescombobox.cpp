#include "defaultbaseimagescombobox.h"
#include "application.h"
#include <QtCore/QDir>

namespace SynGlyphX {

	std::map<QString, QString> DefaultBaseImagesComboBox::s_filenameToNameMap;
	QString DefaultBaseImagesComboBox::s_worldDefaultBaseImageLocation;

	DefaultBaseImagesComboBox::DefaultBaseImagesComboBox(QWidget *parent)
		: QComboBox(parent)
	{
		BuildFilenameMap();

		setInsertPolicy(QComboBox::InsertPolicy::InsertAlphabetically);

		for (auto defaultBaseMap : s_filenameToNameMap) {

			addItem(defaultBaseMap.second, defaultBaseMap.first);
		}

		SetDefaultBaseImage(s_worldDefaultBaseImageLocation);
	}

	DefaultBaseImagesComboBox::~DefaultBaseImagesComboBox()
	{

	}

	void DefaultBaseImagesComboBox::SetDefaultBaseImage(const QString& filename) {

		std::map<QString, QString>::iterator iT = s_filenameToNameMap.find(filename);
		if (iT != s_filenameToNameMap.end()) {

			setCurrentText(iT->second);
		}
	}

	QString DefaultBaseImagesComboBox::GetDefaultBaseImage() const {

		return currentData().toString();
	}

	const QString& DefaultBaseImagesComboBox::GetWorldDefaultBaseImageLocation() {

		BuildFilenameMap();
		return s_worldDefaultBaseImageLocation;
	}

	void DefaultBaseImagesComboBox::BuildFilenameMap() {

		if (s_filenameToNameMap.empty()) {

			QString defaultBaseImagesLocation = QDir::toNativeSeparators(SynGlyphX::Application::applicationDirPath() + QDir::separator() + "DefaultBaseImages" + QDir::separator());
			QDir defaultBaseImagesDir(defaultBaseImagesLocation);
			QFileInfoList defaultBaseImages = defaultBaseImagesDir.entryInfoList(QStringList("*.png"));

			Q_FOREACH(const QFileInfo& defaultBaseImage, defaultBaseImages) {

				QString filename = defaultBaseImage.completeBaseName();
				if (filename == "World") {

					s_worldDefaultBaseImageLocation = defaultBaseImage.canonicalFilePath();
				}
				s_filenameToNameMap.insert(std::pair<QString, QString>(defaultBaseImage.canonicalFilePath(), filename.replace('_', ' ')));
			}

		}
	}

} //namespace SynGlyphX