#ifndef ANTZSINGLEGLYPHTREEWIDGET_H
#define ANTZSINGLEGLYPHTREEWIDGET_H

#include "antzgui_global.h"
#include "antzwidget.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include "data/nptypes.h"
#include "glyph.h"
#include "minmaxglyphtreemodel.h"
#include <QtCore/QItemSelectionModel>

class ANTZGUI_EXPORT ANTzSingleGlyphTreeWidget : public ANTzWidget
{
    Q_OBJECT

public:
	enum MinMaxGlyphTreeType {
		Min,
		Max
	};

	enum EditingMode {
		Move = 0,
		Rotate,
		Size,
		None
	};

	ANTzSingleGlyphTreeWidget(MinMaxGlyphTreeType glyphTreeType, QWidget *parent = 0);
	~ANTzSingleGlyphTreeWidget();

	void SetAllowMultiSelection(bool allowMultiSelection);
	void SetModel(MinMaxGlyphTreeModel* model);

    bool LoadFromFile(const QString& filename);
	void SaveToTemplateFile(const QString& filename) const;
    bool SaveToCSV(const std::string& filename, const QModelIndexList& selectedItems);

    void AppendChild(const QModelIndex& parent, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, unsigned int numberOfChildren = 1);

    static PropertyUpdates FindUpdates(boost::shared_ptr<const SynGlyphX::GlyphProperties> oldGlyph, boost::shared_ptr<const SynGlyphX::GlyphProperties> newGlyph);
    static bool GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right);

	void ShowGlyph(bool show);

public slots:
	void SetEditingMode(EditingMode mode);

signals:
	void ObjectEdited(const QModelIndex& index);
    void NodeUpdated(const QModelIndex& index);
    void ModelChanged(bool isDifferentFromSavedFileOrDefaultGlyph);

protected:
	virtual void BeforeDrawScene();
	virtual void AfterDrawScene();
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);

	bool IsRootNodeSelected() const;

private slots:
	void UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected);
	void OnNodeDeleted(const QModelIndex& parent, int start, int end);

	void NotifyModelUpdate();
	void MarkDifferentNotifyModelUpdate();

	void OnModelReset();
	void OnModelRowsInserted(const QModelIndex& parent, int first, int last);
	void OnModelRowsMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow);
	void OnModelRowsRemoved(const QModelIndex& parent, int first, int last);


private:

	QModelIndex IndexFromANTzID(int id);
	pNPnode GetGlyphFromModelIndex(const QModelIndex& index) const;
	pNPnode GetRootGlyph() const;
	
    int GetChildIndexFromParent(pNPnode node) const;
	
	bool IsANTzCSVFile(const QString& filename) const;

	void DeleteChildren(pNPnode parent, unsigned int first, unsigned int count);
	void DeleteGlyphRootNode();

	void CreateNewSubTree(pNPnode parent, const SynGlyphX::MinMaxGlyphTree::const_iterator& minMaxGlyph);
	pNPnode CreateNodeFromTemplate(pNPnode parent, const SynGlyphX::MinMaxGlyphTree::const_iterator& minMaxGlyph);
	void RebuildTree();

	void UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::MinMaxGlyphTree::const_iterator& minMaxGlyph);
	void UpdateGlyphProperties(pNPnode glyph, const SynGlyphX::GlyphProperties& glyphTemplate);

	pNPnode m_rootGlyph;
	
	SynGlyphX::GlyphProperties::SharedPtr m_clipboardGlyph;

	MinMaxGlyphTreeType m_glyphTreeType;

	QPoint m_lastMousePosition;

	EditingMode m_editingMode;
	bool m_selectionEdited;
	bool m_allowMultiSelection;

	MinMaxGlyphTreeModel* m_model;
	QItemSelectionModel* m_selectionModel;
};

#endif // ANTZSINGLEGLYPHTREEWIDGET_H
