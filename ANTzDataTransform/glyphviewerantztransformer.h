#ifndef GVANTZTRANSFORMER_H
#define GVANTZTRANSFORMER_H

#include "antzdatatransform_global.h"
#include "antztransformer.h"

class ANTZDATATRANSFORM_EXPORT GlyphViewerANTzTransformer : public ANTzTransformer

{
public:
	GlyphViewerANTzTransformer(const QString& cacheBaseDir);
	virtual ~GlyphViewerANTzTransformer();

protected:
	virtual void CreateGlyphsFromMapping(SynGlyphX::DataTransformMapping& mapping);
	bool DoesCacheNeedToBeRegenerated(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& baseImageFilename) const;
};

#endif //GVANTZTRANSFORMER_H