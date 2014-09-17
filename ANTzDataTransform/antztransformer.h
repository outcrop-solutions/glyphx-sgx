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
	ANTzTransformer(const QString& cacheBaseDir);
	virtual ~ANTzTransformer();

	const QStringList& GetGeneratedFilenames() const;
	const QString& GetBaseImageFilename() const;

protected:
	virtual void CreateGlyphsFromMapping(SynGlyphX::DataTransformMapping& mapping);
	bool DoesCacheNeedToBeRegenerated(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename) const;
	void GenerateCache(SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename);
	void DownloadBaseImage(SynGlyphX::DataTransformMapping& mapping, const QString& baseImageFilename) const;

	QString m_cacheBaseDir;
	QStringList m_generatedFilenames;
	QString m_baseImageFilename;
};

#endif //ANTZTRANSFORMER_H