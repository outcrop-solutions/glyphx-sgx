#include "glyphmimedata.h"

namespace SynGlyphX {

	const QString GlyphMimeData::Format("application/x-sgx-glyph");

	//GlyphMimeData::GlyphMimeData(const std::vector<boost::shared_ptr<SynGlyphX::Glyph>>& glyphs)
	GlyphMimeData::GlyphMimeData(const QModelIndexList& glyphs)
		: QMimeData(),
		m_glyphs(glyphs)
	{

	}

	GlyphMimeData::~GlyphMimeData()
	{

	}

	QStringList GlyphMimeData::formats() const {

		QStringList types;
		types.push_back(Format);
		return types;
	}

	bool GlyphMimeData::hasFormat(const QString & mimeType) const {

		return (mimeType == Format);
	}

	//const std::vector<boost::shared_ptr<SynGlyphX::Glyph>>& GlyphMimeData::GetGlyphs() const {
	const QModelIndexList& GlyphMimeData::GetGlyphs() const {

		return m_glyphs;
	}

} //namespace SynGlyphX