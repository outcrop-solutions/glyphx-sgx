#ifndef SELECTIONTRANSLATOR_H
#define SELECTIONTRANSLATOR_H

#include <QtCore/QObject>
#include <QtCore/QItemSelection>
#include <QtCore/QItemSelectionModel>
#include <boost/uuid/uuid.hpp>
#include "minmaxglyphtree.h"
#include "datatransformmodel.h"
#include "glyphtreemodel.h"

class SelectionTranslator : public QObject
{
	Q_OBJECT

public:
	SelectionTranslator(DataTransformModel* dataTransformModel, GlyphTreeModel* glyphTreeModel, QObject *parent);
	~SelectionTranslator();

	QItemSelectionModel* GetSelectionModel() const;

signals:
	

public slots:
	void OnTreeViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	SynGlyphX::MinMaxGlyphTree::iterator m_glyph;
	const SynGlyphX::MinMaxGlyphTree* m_glyphTree;
	DataTransformModel* m_dataTransformModel;
	GlyphTreeModel* m_glyphTreeModel;
	QItemSelectionModel* m_selectionModel;
};

#endif // SELECTIONTRANSLATOR_H
