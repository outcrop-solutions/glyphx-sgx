#include "glyphviewerantztransformer.h"
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "userdefinedbaseimageproperties.h"

GlyphViewerANTzTransformer::GlyphViewerANTzTransformer(const QString& cacheBaseDir) :
	ANTzTransformer(cacheBaseDir)
{
	SynGlyphX::SourceDataManager::SetIntermediateDirectory(QDir::toNativeSeparators(cacheBaseDir + QDir::separator() + "intermediate"));
}


GlyphViewerANTzTransformer::~GlyphViewerANTzTransformer()
{
}

void GlyphViewerANTzTransformer::CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping) {

	Clear();

	QString localOutputDir = QDir::toNativeSeparators(m_baseOutputDir + QDir::separator());

	QStringList csvFiles;
	csvFiles.push_back(localOutputDir + "antz.csv");
	csvFiles.push_back(localOutputDir + "antztag.csv");
	QStringList baseImageFilenames;

	QString cachedMappingFilename = localOutputDir + "mapping.sdt";

	if (DoesCacheNeedToBeRegenerated(mapping, csvFiles, cachedMappingFilename)) {

		GenerateCache(mapping, csvFiles, localOutputDir);
		//Write the mapping to the cache
		mapping.WriteToFile(cachedMappingFilename.toStdString());
	}
}

bool GlyphViewerANTzTransformer::DoesCacheNeedToBeRegenerated(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& mappingFilename) const {

	if (csvFilenames.empty()) {

		return true;
	}

	Q_FOREACH(QString csvFilename, csvFilenames) {

		QFile csvFile(csvFilename);
		if (!csvFile.exists()) {
			return true;
		}
	}
	/*
	if (!baseImageFilename.isEmpty()) {
		
		QFile baseImage(baseImageFilename);
		if (!baseImage.exists()) {
			
			return true;
		}
		else if (mapping.GetBaseObjects()[0].GetType() == SynGlyphX::BaseImage::Type::UserImage) {

			const SynGlyphX::UserDefinedBaseImageProperties* const properties = dynamic_cast<const SynGlyphX::UserDefinedBaseImageProperties* const>(mapping.GetBaseObjects()[0].GetProperties());
			if (HasFileBeenUpdated(properties->GetFilename(), boost::filesystem::last_write_time(baseImageFilename.toStdWString()))) {

				return true;
			}
		}
	}*/

	QFile cachedMappingFile(mappingFilename);
	if (!cachedMappingFile.exists()) {
	
		return true;
	}
	
	SynGlyphX::DataTransformMapping cachedMapping;
	cachedMapping.ReadFromFile(mappingFilename.toStdString());
	if (cachedMapping != mapping) {

		return true;
	}

	boost::filesystem::path firstCSVFilePath(csvFilenames[0].toStdString());
	return HaveDatasourcesBeenUpdated(mapping, boost::filesystem::last_write_time(firstCSVFilePath));
}

QString GlyphViewerANTzTransformer::GenerateBaseImageFilename(unsigned int index) const {

	return "base_image_" + QString::number(index) + ".png";
}