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
	SelectionTranslator(DataTransformModel* dataTransformModel, GlyphTreeModel* glyphTreeModel, QItemSelectionModel* treeViewSelectionModel, QObject *parent);
	~SelectionTranslator();

	QItemSelectionModel* Get3DViewSelectionModel() const;

	void UpdateSelectedGlyphProperties(SynGlyphX::GlyphProperties::ConstSharedPtr glyph);

private slots:
	void On3DViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnTreeViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
	void Connect3DViewSelectionModel();
	void ConnectTreeViewSelectionModel();

	SynGlyphX::MinMaxGlyphTree::iterator m_glyph;
	const SynGlyphX::MinMaxGlyphTree* m_glyphTree;
	unsigned int m_glyphTreeIndex;
	DataTransformModel* m_dataTransformModel;
	GlyphTreeModel* m_glyphTreeModel;
	QItemSelectionModel* m_3DViewSelectionModel;
	QItemSelectionModel* m_treeViewSelectionModel;
	QMetaObject::Connection m_3DViewConnection;
	QMetaObject::Connection m_treeViewConnection;
};

#endif // SELECTIONTRANSLATOR_H
