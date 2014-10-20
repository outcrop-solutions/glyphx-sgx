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
	ANTzTransformer(const QString& baseOutputDir);
	virtual ~ANTzTransformer();

	const QStringList& GetCSVFilenames() const;
	const QString& GetBaseImageFilename() const;

protected:
	virtual void CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping);
	void GenerateCache(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename);
	void DownloadBaseImage(const SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilename);
	void CopyUserImage(const SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilename);

	QStringList m_csvFilenames;
	QString m_baseImageFilename;
	QString m_baseOutputDir;
};

#endif //ANTZTRANSFORMER_H