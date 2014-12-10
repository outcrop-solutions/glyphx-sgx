#ifndef ANTZTRANSFORMER_H
#define ANTZTRANSFORMER_H

#include "antzdatatransform_global.h"
#include "transformer.h"
#include "datatransformmapping.h"
#include <QtCore/QString>
#include <QtCore/QStringList>

class ANTZDATATRANSFORM_EXPORT ANTzTransformer : public SynGlyphX::Transformer

{
public:
	//typedef std::unordered_map<QString, unsigned int> BaseImageTextureMap;

	ANTzTransformer(const QString& baseOutputDir);
	virtual ~ANTzTransformer();

	const QStringList& GetCSVFilenames() const;
	const QStringList& GetBaseImageFilenames() const;

	unsigned int GetTextureID(unsigned int index) const;

protected:
	const unsigned int NumberOfDefaultBaseImages = 1;

	virtual QString GenerateBaseImageFilename(unsigned int index) const;
	virtual void CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping);
	void GenerateCache(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilenameDirectory);
	void DownloadBaseImage(const SynGlyphX::DataTransformMapping& mapping, const SynGlyphX::BaseImage& baseImage, const QString& baseImageFilename);
	void CopyImage(const QString& sourceFilename, const QString& baseImageFilename);
	void Clear();
	QString GetUserImageFilename(const SynGlyphX::BaseImage& baseImage) const;

	QStringList m_csvFilenames;
	QStringList m_baseImageFilenames;
	QString m_baseOutputDir;
	std::vector<unsigned int> m_textureIDs;
};

#endif //ANTZTRANSFORMER_H