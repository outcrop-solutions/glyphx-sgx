#ifndef GLYPHFORESTMODEL_H
#define GLYPHFORESTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <unordered_map>
#include "antzdata.h"
#include "glyph.h"

class GlyphForestModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	GlyphForestModel(QObject *parent);
	~GlyphForestModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex	parent(const QModelIndex& index) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	ANTzPlus::ANTzData::SharedPtr GetANTzData() const;

	void LoadANTzFiles(const QStringList& filenames);

	QModelIndex IndexFromANTzID(int id) const;

	void UseDefaultBaseImage();
	void UseLocalBaseImage(const QString& filename);

private:
	int GetChildIndexFromParent(pNPnode node) const;
	int FindRowForRootNode(pNPnode) const;
	std::unordered_map<std::wstring, int> m_textures;
	QString m_defaultBaseImage;
	ANTzPlus::ANTzData::SharedPtr m_antzData;
};

#endif // GLYPHFORESTMODEL_H
