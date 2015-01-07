#ifndef SYNGLYPHX_ANTZ_GVANTZTRANSFORMER_H
#define SYNGLYPHX_ANTZ_GVANTZTRANSFORMER_H

#include "antzdatatransform_global.h"
#include "antztransformer.h"

namespace SynGlyphXANTz {

	class ANTZDATATRANSFORM_EXPORT GlyphViewerANTzTransformer : public ANTzTransformer

	{
	public:
		GlyphViewerANTzTransformer(const QString& cacheBaseDir);
		virtual ~GlyphViewerANTzTransformer();

	protected:
		virtual QString GenerateBaseImageFilename(unsigned int index) const;
		virtual void CreateGlyphsFromMapping(const SynGlyphX::DataTransformMapping& mapping);
		bool DoesCacheNeedToBeRegenerated(const SynGlyphX::DataTransformMapping& mapping, const QStringList& csvFilenames, const QString& mappingFilename) const;
	};

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_ANTZ_GVANTZTRANSFORMER_H