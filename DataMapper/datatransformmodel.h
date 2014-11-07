#ifndef DATATRANSFORMMODEL_H
#define DATATRANSFORMMODEL_H

#include "datatransformmapping.h"
#include "sourcedatamanager.h"
#include <QtCore/QAbstractItemModel>

class DataTransformModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	static const int DataTypeRole = Qt::UserRole;

	enum DataType {

		GlyphTrees = 0,
		BaseObjects = 1,
		DataSources = 2
	};

	DataTransformModel(QObject *parent);
	~DataTransformModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex	parent(const QModelIndex& index) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	QVariant GetDisplayData(const QModelIndex& index) const;
	QVariant GetDataTypeData(const QModelIndex& index) const;

	SynGlyphX::DataTransformMapping::ConstSharedPtr GetDataMapping() const;

	void SetDefaults(const SynGlyphX::DataMappingDefaults& defaults);

	void LoadDataTransformFile(const QString& filename);
	void SaveDataTransformFile(const QString& filename);
	void Clear();

	void AddGlyphFile(const QString& filename);
	void SetBaseImage(const SynGlyphX::BaseImage& baseImage);

	boost::uuids::uuid AddFileDatasource(SynGlyphX::FileDatasource::SourceType type, const std::wstring& name);

	void SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::MinMaxGlyphTree::const_iterator& node, int index, const SynGlyphX::InputField& inputfield);
	void ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::MinMaxGlyphTree::const_iterator& node, int index);

	void EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableSet& tables, bool enable = true);

private:
	void SetIntermediateDirectoryToCurrentID();
	QString GetIntermediateDirectoryForID(const boost::uuids::uuid& id);
	QVariant GetGlyphData(const QModelIndex& index) const;
	bool IsRowInDataType(DataType type, int row) const;

	SynGlyphX::SourceDataManager m_sourceDataManager;
	SynGlyphX::DataTransformMapping::SharedPtr m_dataMapping;
};

#endif // DATATRANSFORMMODEL_H
