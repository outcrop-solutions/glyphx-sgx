#ifndef SELECTIONTRANSLATOR_H
#define SELECTIONTRANSLATOR_H

#include <QtCore/QObject>
#include <QtCore/QItemSelection>
#include <boost/uuid/uuid.hpp>
#include "minmaxglyphtree.h"
#include "datatransformmodel.h"

class SelectionTranslator : public QObject
{
	Q_OBJECT

public:
	SelectionTranslator(DataTransformModel* dataTransformModel, QObject *parent);
	~SelectionTranslator();

signals:
	void GlyphTreeChanged(boost::uuids::uuid treeID);

public slots:
	void OnTreeViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	SynGlyphX::MinMaxGlyphTree::iterator m_glyph;
	const SynGlyphX::MinMaxGlyphTree* m_glyphTree;
	DataTransformModel* m_dataTransformModel;
};

#endif // SELECTIONTRANSLATOR_H
