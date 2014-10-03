#ifndef DATATRANSFORMMODEL_H
#define DATATRANSFORMMODEL_H

#include <QtCore/QAbstractItemModel>
#include "datatransformmapping.h"
#include "sourcedatamanager.h"

class DataTransformModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	DataTransformModel(QObject *parent);
	~DataTransformModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex	parent(const QModelIndex& index) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;

	void AddGlyphFile(const QString& filename);
	void LoadDataTransformFile(const QString& filename);
	void Clear();

	void SetBaseImage(const SynGlyphX::BaseImage& baseImage);

	boost::uuids::uuid AddFileDatasource(SynGlyphX::FileDatasource::SourceType type, const std::wstring& name);

	void SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::MinMaxGlyphTree::const_iterator& node, int index, const SynGlyphX::InputField& inputfield);
	void ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::MinMaxGlyphTree::const_iterator& node, int index);

	void EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableSet& tables, bool enable = true);

	SynGlyphX::DataTransformMapping::ConstSharedPtr GetDataTransform() const;

private:
	SynGlyphX::DataTransformMapping::SharedPtr m_dataTransform;
	SynGlyphX::SourceDataManager m_sourceDataManager;
};

#endif // DATATRANSFORMMODEL_H
