#ifndef GLYPHMIMEDATA_H
#define GLYPHMIMEDATA_H

#include "antzgui_global.h"
#include <QtCore/QMimeData>
#include "glyph.h"
#include <QtCore/QModelIndex>

class ANTZGUI_EXPORT GlyphMimeData : public QMimeData
{
    Q_OBJECT

public:
    static const QString Format;

    GlyphMimeData(const QModelIndexList& glyphs);
    //GlyphMimeData(const std::vector<boost::shared_ptr<SynGlyphX::Glyph>>& glyphs);
    ~GlyphMimeData();

    virtual QStringList formats() const;
    virtual bool hasFormat(const QString& mimeType) const;

    const QModelIndexList& GetGlyphs() const;
    //const std::vector<boost::shared_ptr<SynGlyphX::Glyph>>& GetGlyphs() const;

private:
    QModelIndexList m_glyphs;
    //std::vector<boost::shared_ptr<SynGlyphX::Glyph>> m_glyphs;
};

#endif // GLYPHMIMEDATA_H
