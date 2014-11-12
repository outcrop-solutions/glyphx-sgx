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

    QModelIndex IndexFromANTzID(int id);

    void AppendChild(const QModelIndex& parent, boost::shared_ptr<const SynGlyphX::GlyphProperties> glyph, unsigned int numberOfChildren = 1);

    static PropertyUpdates FindUpdates(boost::shared_ptr<const SynGlyphX::GlyphProperties> oldGlyph, boost::shared_ptr<const SynGlyphX::GlyphProperties> newGlyph);
    static bool GreaterBranchLevel(const QModelIndex& left, const QModelIndex& right);

	void ShowGlyph(bool show);

public slots:
	void SetEditingMode(EditingMode mode);

signals:
    void NodeUpdated(const QModelIndex& index);
    void ModelChanged(bool isDifferentFromSavedFileOrDefaultGlyph);

protected:
	virtual void BeforeDrawScene();
	virtual void AfterDrawScene();

private slots:
	void NotifyModelUpdate();
	void MarkDifferentNotifyModelUpdate();

	void OnModelReset();

private:
	pNPnode GetRootGlyph() const;
	void RebuildTree();
	pNPnode CreateNodeFromTemplate(pNPnode parent, const SynGlyphX::MinMaxGlyphTree::const_iterator& minMaxGlyph);
    int GetChildIndexFromParent(pNPnode node) const;
	void CreateNewSubTree(pNPnode parent, const SynGlyphX::MinMaxGlyphTree::const_iterator& minMaxGlyph);
	bool IsANTzCSVFile(const QString& filename) const;
	void DeleteGlyphRootNode();
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
