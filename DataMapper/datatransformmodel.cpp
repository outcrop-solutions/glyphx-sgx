#include "datatransformmodel.h"
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <boost/uuid/uuid_io.hpp>
#include "application.h"
#include "downloadedmapproperties.h"
#include "userdefinedbaseimageproperties.h"
#include <boost/filesystem.hpp>
#include "datamappingglyphfile.h"
#include "glyphnodeconverter.h"
#include "glyphmimedata.h"
#include "defaultbaseimageproperties.h"
#include "defaultbaseimagescombobox.h"
#include "surfaceradiobuttonwidget.h"
#include "dataenginestatement.h"
#include "stringconvert.h"
#include <set>
#include <QtWidgets/QMessageBox>
DataTransformModel::DataTransformModel(QObject *parent)
	: QAbstractItemModel(parent),
	m_dataMapping(new SynGlyphX::DataTransformMapping()),
	m_dataEngineConnection(nullptr)
{
	ClearAndReset();
}

DataTransformModel::~DataTransformModel()
{

}

int DataTransformModel::columnCount(const QModelIndex& parent) const {

	return 1;
}

void DataTransformModel::SetDataEngineConnection(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection){
	
	m_dataEngineConnection = dataEngineConnection;
}

bool DataTransformModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if ((role >= OptionsRole) && (role <= PropertyRole::GeometryTorusRatio) && (GetDataType(index) == DataType::GlyphTrees)) {

		if (!m_dataMapping->GetGlyphGraphs().empty()) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));

			if (role == OptionsRole) {

				auto glyphGraph = m_dataMapping->GetGlyphGraphs().begin();
				std::advance(glyphGraph, index.row() - GetFirstIndexForDataType(DataType::GlyphTrees));
				glyphGraph->second->SetMergeRoots(value.toBool());
			} 
			else if (role == PropertyRole::PositionX) {

				iterator->second.GetPosition()[0] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::PositionY) {

				iterator->second.GetPosition()[1] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::PositionZ) {

				iterator->second.GetPosition()[2] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationX) {

				iterator->second.GetRotation()[0] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationY) {

				iterator->second.GetRotation()[1] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationZ) {

				iterator->second.GetRotation()[2] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::ScaleX) {

				iterator->second.GetScale()[0] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::ScaleY) {

				iterator->second.GetScale()[1] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::ScaleZ) {

				iterator->second.GetScale()[2] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::Color) {

				iterator->second.GetColor() = value.value<SynGlyphX::ColorMappingProperty>();
			}
			else if (role == PropertyRole::Transparency) {

				iterator->second.GetTransparency() = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::Tag) {

				iterator->second.GetTag() = value.value<SynGlyphX::TextMappingProperty>();
			}
			else if (role == PropertyRole::Description) {

				iterator->second.GetDescription() = value.value<SynGlyphX::TextMappingProperty>();
			}
			else if (role == PropertyRole::RotationRateX) {

				iterator->second.GetRotationRate()[0] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationRateY) {

				iterator->second.GetRotationRate()[1] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::RotationRateZ) {

				iterator->second.GetRotationRate()[2] = value.value<SynGlyphX::NumericMappingProperty>();
			}
			else if (role == PropertyRole::GeometryShape) {

				iterator->second.GetStructure().SetGeometryShape(value.value<SynGlyphX::GeometryShapeMappingProperty>());
			}
			else if (role == PropertyRole::GeometrySurface) {

				iterator->second.GetStructure().SetGeometrySurface(static_cast<SynGlyphX::GlyphGeometryInfo::Surface>(value.toInt()));
			}
			else if (role == PropertyRole::GeometryTorusRatio) {

				iterator->second.GetStructure().SetTorusRatio(value.toDouble());
			}
			else if (role == PropertyRole::VirtualTopology) {

				iterator->second.GetVirtualTopology().SetType(value.value<SynGlyphX::VirtualTopologyMappingProperty>());
			}
			else if (role == PropertyRole::URL) {

				iterator->second.GetDescription() = value.value<SynGlyphX::TextMappingProperty>();
			}

			QVector<int> roles;
			roles.push_back(role);

			if ((role == PropertyRole::GeometryShape) || (role == PropertyRole::VirtualTopology)) {

				roles.push_back(Qt::DisplayRole);
			}

			emit dataChanged(index, index, roles);

			return true;
		}

	}

	return false;
}

QVariant DataTransformModel::data(const QModelIndex& index, int role) const {

	if (!index.isValid()) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {

		return GetDisplayData(index);
	}
	else if (role == UUIDRole) {

		if (!index.parent().isValid()) {

			if (IsParentlessRowInDataType(DataType::DataSources, index.row())) {

				auto datasource = m_dataMapping->GetDatasources().begin();
				std::advance(datasource, index.row() - GetFirstIndexForDataType(DataType::DataSources));
				return QString::fromStdString(boost::uuids::to_string(datasource->first));
			}
			else if (IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

				auto glyphGraph = m_dataMapping->GetGlyphGraphs().begin();
				std::advance(glyphGraph, index.row() - GetFirstIndexForDataType(DataType::GlyphTrees));
				return QString::fromStdString(boost::uuids::to_string(glyphGraph->first));
			}
		}
	}
	else if (role == DataTypeRole) {

		return GetDataTypeData(index);
	}
	else if (role == OptionsRole) {

		if (!index.parent().isValid() && IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

			auto glyphGraph = m_dataMapping->GetGlyphGraphs().begin();
			std::advance(glyphGraph, index.row() - GetFirstIndexForDataType(DataType::GlyphTrees));
			return glyphGraph->second->GetMergeRoots();
		}
	}
	else if ((role >= PropertyRole::PositionX) && (role <= PropertyRole::GeometryTorusRatio)) {

		return GetPropertyData(index, role);
	}

	return QVariant();
}

QVariant DataTransformModel::GetPropertyData(const QModelIndex& index, int role) const {

	if ((role >= PropertyRole::PositionX) && (role <= PropertyRole::GeometryTorusRatio) && (GetDataType(index) == DataType::GlyphTrees)) {

		if (!m_dataMapping->GetGlyphGraphs().empty()) {

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));

			QVariant var;
			if (role == PropertyRole::PositionX) {

				var.setValue(iterator->second.GetPosition()[0]);
			}
			else if (role == PropertyRole::PositionY) {

				var.setValue(iterator->second.GetPosition()[1]);
			}
			else if (role == PropertyRole::PositionZ) {

				var.setValue(iterator->second.GetPosition()[2]);
			}
			else if (role == PropertyRole::RotationX) {

				var.setValue(iterator->second.GetRotation()[0]);
			}
			else if (role == PropertyRole::RotationY) {

				var.setValue(iterator->second.GetRotation()[1]);
			}
			else if (role == PropertyRole::RotationZ) {

				var.setValue(iterator->second.GetRotation()[2]);
			}
			else if (role == PropertyRole::ScaleX) {

				var.setValue(iterator->second.GetScale()[0]);
			}
			else if (role == PropertyRole::ScaleY) {

				var.setValue(iterator->second.GetScale()[1]);
			}
			else if (role == PropertyRole::ScaleZ) {

				var.setValue(iterator->second.GetScale()[2]);
			}
			else if (role == PropertyRole::Color) {

				var.setValue(iterator->second.GetColor());
			}
			else if (role == PropertyRole::Transparency) {

				var.setValue(iterator->second.GetTransparency());
			}
			else if (role == PropertyRole::Tag) {

				var.setValue(iterator->second.GetTag());
			}
			else if (role == PropertyRole::Description) {

				var.setValue(iterator->second.GetDescription());
			}
			else if (role == PropertyRole::RotationRateX) {

				var.setValue(iterator->second.GetRotationRate()[0]);
			}
			else if (role == PropertyRole::RotationRateY) {

				var.setValue(iterator->second.GetRotationRate()[1]);
			}
			else if (role == PropertyRole::RotationRateZ) {

				var.setValue(iterator->second.GetRotationRate()[2]);
			}
			else if (role == PropertyRole::GeometryShape) {

				var.setValue(iterator->second.GetStructure().GetGeometryShape());
			}
			else if (role == PropertyRole::GeometrySurface) {

				var.setValue(iterator->second.GetStructure().GetGeometrySurface());
			}
			else if (role == PropertyRole::GeometryTorusRatio) {

				var.setValue(iterator->second.GetStructure().GetTorusRatio());
			}
			else if (role == PropertyRole::VirtualTopology) {

				var.setValue(iterator->second.GetVirtualTopology().GetType());
			}
			else if (role == PropertyRole::URL) {

				var.setValue(iterator->second.GetURL());
			}

			if (var.isValid()) {

				return var;
			}
		}
	}

	return QVariant();
}

QVariant DataTransformModel::GetDisplayData(const QModelIndex& index) const {

	if (!index.parent().isValid()) {

		if (IsParentlessRowInDataType(DataType::BaseObjects, index.row())) {

			int baseObjectIndex = index.row() - m_dataMapping->GetGlyphGraphs().size();
			const SynGlyphX::BaseImage& baseObject = m_dataMapping->GetBaseObjects()[baseObjectIndex];
			SynGlyphX::BaseImage::Type baseImageType = baseObject.GetType();
			if (baseImageType == SynGlyphX::BaseImage::UserImage) {

				auto properties = std::dynamic_pointer_cast<const SynGlyphX::UserDefinedBaseImageProperties>(baseObject.GetProperties());
				QFileInfo baseObjectFileInfo(QString::fromStdWString(properties->GetFilename()));
				return baseObjectFileInfo.fileName();
			}
			else if (baseImageType == SynGlyphX::BaseImage::DownloadedMap) {

				auto properties = std::dynamic_pointer_cast<const SynGlyphX::DownloadedMapProperties>(baseObject.GetProperties());
				return QString::fromStdWString(L"Downloaded Map: " + SynGlyphX::DownloadedMapProperties::s_mapSourceStrings.left.at(properties->GetSource()) + L", " + SynGlyphX::DownloadedMapProperties::s_mapTypeStrings.left.at(properties->GetType()));
			}
			else {

				auto properties = std::dynamic_pointer_cast<const SynGlyphX::DefaultBaseImageProperties>(baseObject.GetProperties());
				return "Default: " + SynGlyphX::DefaultBaseImagesComboBox::s_typeToNameMap.at(properties->GetDefaultBaseImageType());
			}
		}
		else if (IsParentlessRowInDataType(DataType::DataSources, index.row())) {

			int datasourceIndex = index.row() - m_dataMapping->GetGlyphGraphs().size() - m_dataMapping->GetBaseObjects().size();
			SynGlyphX::DataTransformMapping::DatasourceMap::const_iterator datasource = m_dataMapping->GetDatasources().begin();
			std::advance(datasource, datasourceIndex);
			return QString::fromStdWString(datasource->second->GetFormattedName());
		}
		else if (IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

			return GetGlyphData(index);
		}
		else if (IsParentlessRowInDataType(DataType::FieldGroup, index.row())) {

			int fieldGroupIndex = index.row() - (m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().size());
			SynGlyphX::DataTransformMapping::FieldGroupMap::const_iterator fieldGroup = m_dataMapping->GetFieldGroupMap().begin();
			std::advance(fieldGroup, fieldGroupIndex);
			return QString::fromStdWString(fieldGroup->first);
		}
		else if (IsParentlessRowInDataType(DataType::Legends, index.row())) {

			int legendIndex = index.row() - GetFirstIndexForDataType(DataType::Legends);
			std::vector<SynGlyphX::Legend>::const_iterator legend = m_dataMapping->GetLegends().begin();
			std::advance(legend, legendIndex);
			QFileInfo baseObjectFileInfo(QString::fromStdWString(legend->GetFilename()));
			return QString::fromStdWString(legend->GetTitle()) + ": " + baseObjectFileInfo.fileName();
		}
	}
	else {

		return GetGlyphData(index);
	}

	return QVariant();
}

QVariant DataTransformModel::GetDataTypeData(const QModelIndex& index) const {

	return GetDataType(index);
}

DataTransformModel::DataType DataTransformModel::GetDataType(const QModelIndex& index) const {

	if (!index.parent().isValid()) {

		if (IsParentlessRowInDataType(DataType::BaseObjects, index.row())) {

			return DataType::BaseObjects;
		}
		else if (IsParentlessRowInDataType(DataType::DataSources, index.row())) {

			return DataType::DataSources;
		}
		else if (IsParentlessRowInDataType(DataType::FieldGroup, index.row())) {

			return DataType::FieldGroup;
		}
		else if (IsParentlessRowInDataType(DataType::Legends, index.row())) {

			return DataType::Legends;
		}
		else if (IsParentlessRowInDataType(DataType::GlyphTrees, index.row())) {

			return DataType::GlyphTrees;
		}
	}
	else {

		return DataType::GlyphTrees;
	}
}

QVariant DataTransformModel::GetGlyphData(const QModelIndex& index) const {

	if (!m_dataMapping->GetGlyphGraphs().empty()) {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));

		std::wstring glyphData = SynGlyphX::GlyphGeometryInfo::s_shapeNames.left.at(iterator->second.GetStructure().GetGeometryShape().GetValue()) + L": " + 
			SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left.at(iterator->second.GetVirtualTopology().GetType().GetValue());
		if (iterator == iterator.owner()->root().constify()) {
			glyphData += L" (Root)";
		}

		return QString::fromStdWString(glyphData);
	}
	else {

		return QVariant();
	}
}

QModelIndex	DataTransformModel::index(int row, int column, const QModelIndex& parent) const{

	if (!hasIndex(row, column, parent)) {
		
		return QModelIndex();
	}

	if (!parent.isValid()) {

		if (IsParentlessRowInDataType(DataType::GlyphTrees, row)) {

			SynGlyphX::DataTransformMapping::DataMappingGlyphGraphMap::const_iterator iterator = m_dataMapping->GetGlyphGraphs().begin();
			std::advance(iterator, row);
			return createIndex(row, column, static_cast<void*>(iterator->second->GetRoot().node()));
		}
		else {

			return createIndex(row, column, nullptr);
		}
	}
	else {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator parentIterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator childIterator = parentIterator.owner()->child(parentIterator, row);
		if (childIterator.valid()) {

			return createIndex(row, column, static_cast<void*>(childIterator.node()));
		}
	}

	return QModelIndex();
}

QModelIndex	DataTransformModel::parent(const QModelIndex& index) const {

	if (!index.isValid()) {
		
		return QModelIndex();
	}

	if (index.internalPointer() == nullptr) {

		return QModelIndex();
	}

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	if (iterator != iterator.owner()->root()) {

		const SynGlyphX::DataMappingGlyphGraph* currentTree = static_cast<const SynGlyphX::DataMappingGlyphGraph*>(iterator.owner());
		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator parent = currentTree->GetParent(iterator);

		int row = 0;

		if (parent == currentTree->GetRoot()) {

			for (auto tree : m_dataMapping->GetGlyphGraphs()) {

				if (currentTree->GetRoot() == tree.second->GetRoot().constify()) {

					break;
				}
				++row;
			}
		}
		else {

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator grandparent = currentTree->GetParent(parent);
			if (grandparent.valid()) {

				for (int i = 0; i < currentTree->ChildCount(grandparent); ++i) {

					if (currentTree->GetChild(grandparent, i) == parent) {
						row = i;
						break;
					}
				}
			}
		}

		return createIndex(row, 0, static_cast<void*>(parent.node()));
	}

	return QModelIndex();
}

int	DataTransformModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		return m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().size() + 
			m_dataMapping->GetFieldGroupMap().size() + m_dataMapping->GetLegends().size();
	}

	if (parent.internalPointer() != nullptr) {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator iterator(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		return iterator.owner()->children(iterator);
	}
	
	return 0;
}

boost::uuids::uuid DataTransformModel::AddFileDatasource(const SynGlyphX::FileDatasource& datasource) {

	int newRow = GetFirstIndexForDataType(DataType::DataSources) + m_dataMapping->GetDatasources().size();
	beginInsertRows(QModelIndex(), newRow, newRow);
	boost::uuids::uuid id = m_dataMapping->AddFileDatasource(datasource);
	AddDatasourceInfoFromDataEngine(id, m_dataMapping->GetDatasources().at(id));
	endInsertRows();

	return id;
}

boost::uuids::uuid DataTransformModel::AddDatabaseServer(const SynGlyphX::DatabaseServerDatasource& datasource) {

	int newRow = GetFirstIndexForDataType(DataType::DataSources) + m_dataMapping->GetDatasources().size();
	beginInsertRows(QModelIndex(), newRow, newRow);
	boost::uuids::uuid id = m_dataMapping->AddDatabaseServer(datasource);
	AddDatasourceInfoFromDataEngine(id, m_dataMapping->GetDatasources().at(id));
	endInsertRows();

	return id;
}

/*
void DataTransformModel::SetInputField(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field, const SynGlyphX::InputField& inputfield) {

	m_dataMapping->SetInputField(treeID, node, field, inputfield);
}*/

void DataTransformModel::SetInputField(const QModelIndex& index, SynGlyphX::DataMappingGlyph::MappableField field, const SynGlyphX::InputField& inputfield) {

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator node(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	//SetInputField(treeID, node, field, inputfield);
	m_dataMapping->SetInputField(GetTreeId(index), node, field, inputfield);
	emit dataChanged(index, index);
}
/*
void DataTransformModel::ClearInputBinding(const boost::uuids::uuid& treeID, SynGlyphX::DataMappingGlyphGraph::const_iterator& node, SynGlyphX::DataMappingGlyph::MappableField field) {

	m_dataMapping->ClearInputBinding(treeID, node, field);
}*/

void DataTransformModel::ClearInputBinding(const QModelIndex& index, SynGlyphX::DataMappingGlyph::MappableField field) {

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator node(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	//ClearInputBinding(treeID, node, field);
	m_dataMapping->ClearInputBinding(GetTreeId(index), node, field);
	emit dataChanged(index, index);
}

void DataTransformModel::ClearAllInputBindings(const QModelIndex& index) {

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator node(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	m_dataMapping->ClearAllInputBindings(GetTreeId(index), node);
	emit dataChanged(index, index);
}

void DataTransformModel::ClearAbsentBindings(const QModelIndex& index) {
	//we need a InputFieldMap copy, using const& will cause problems after deleting Inputfield
	SynGlyphX::DataMappingGlyphGraph::InputFieldMap fieldMap = GetInputFieldsForTree(index); 
	std::set<SynGlyphX::InputField::HashID> sourceFields;
	for (const auto& t : m_tableStatsMap) {
		auto table = t.first;
		auto stats = t.second;
		for (const auto& stat : stats) {
			// use arbitrary Type since it does not affect HashID
			SynGlyphX::InputField sInputField(table.GetDatasourceID(), table.GetTable(), stat[0].toStdWString(), SynGlyphX::InputField::Type::Text);
			sourceFields.insert(sInputField.GetHashID());
			}
		}
	for (const auto& field : fieldMap) {
		if (sourceFields.find(field.first) == sourceFields.end()) { 
			SynGlyphX::InputField f(field.second);
			QMessageBox::warning(nullptr, tr("Warning"),
				tr("Source data does not have field:\n") + QString::fromWCharArray(f.GetField().c_str()) + tr("\nMapping will be removed"));
			m_dataMapping->ClearInputFieldBindings(GetTreeId(index), f);
		}
					
	}
	emit dataChanged(index, index);
}

const SynGlyphX::DataMappingGlyphGraph::InputFieldMap& DataTransformModel::GetInputFieldsForTree(const QModelIndex& index) const {

	if (!index.isValid()) {

		throw std::invalid_argument("Can't get input field map from invalid index");
	}

	return m_dataMapping->GetGlyphGraphs().at(GetTreeId(index))->GetInputFields();
}
/*
void DataTransformModel::EnableTables(const boost::uuids::uuid& id, const SynGlyphX::Datasource::TableNames& tables, bool enable) {

	m_dataMapping->EnableTables(id, tables, enable);
	if (enable) {

		for (int i = 0; i < tables.size(); ++i) {

			GenerateStats(SynGlyphX::InputTable(id, tables[i]), i, SynGlyphX::FileDatasource::FileType::SQLITE3);
		}
	}
}*/

bool DataTransformModel::removeRows(int row, int count, const QModelIndex& parent) {

	if (count > 0) {

		int lastRow = row + count - 1;

		if (parent.isValid()) {

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator parentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
			boost::uuids::uuid treeId = GetTreeId(parent);

			beginRemoveRows(parent, row, lastRow);
			for (int i = lastRow; i >= row; --i) {

				m_dataMapping->RemoveGlyph(treeId, parentGlyph, i);
			}
			endRemoveRows();
			
		} else {

			bool emitGlyphDataChanged = false;
			for (int i = lastRow; i >= row; --i) {

				if (IsParentlessRowInDataType(DataType::DataSources, i)) {

					emitGlyphDataChanged = true;

					//Clear all field groups that use the datasource before removing the datasource
					std::vector<SynGlyphX::DataTransformMapping::FieldGroupName> fieldGroupsToBeRemoved;
					for (auto& fieldGroup : m_dataMapping->GetFieldGroupMap()) {

						if (m_dataMapping->DoesFieldGroupHaveFieldsFromDatasource(fieldGroup.first, GetDatasourceId(i))) {

							fieldGroupsToBeRemoved.push_back(fieldGroup.first);
						}
					}

					for (const SynGlyphX::DataTransformMapping::FieldGroupName& name : fieldGroupsToBeRemoved) {

						RemoveFieldGroup(name, false);
					}
				}
			}
			
			beginRemoveRows(parent, row, lastRow);
			for (int i = lastRow; i >= row; --i) {

				if (IsParentlessRowInDataType(DataType::GlyphTrees, i)) {

					m_dataMapping->RemoveGlyphTree(GetTreeId(i));
				}
				else if (IsParentlessRowInDataType(DataType::BaseObjects, i)) {

					m_dataMapping->RemoveBaseObject(i - m_dataMapping->GetGlyphGraphs().size());
				}
				else if (IsParentlessRowInDataType(DataType::DataSources, i)) {

					boost::uuids::uuid id = GetDatasourceId(i);
					RemoveAllAdditionalData(id);
					m_dataMapping->RemoveDatasource(id);
				}
				else if (IsParentlessRowInDataType(DataType::FieldGroup, i)) {

					m_dataMapping->RemoveFieldGroup(GetFieldGroupName(i));
					emitGlyphDataChanged = true;
				}
				else if (IsParentlessRowInDataType(DataType::Legends, i)) {

					m_dataMapping->RemoveLegend(i - GetFirstIndexForDataType(DataType::Legends));
					emitGlyphDataChanged = true;
				}
			}
			endRemoveRows();

			if (emitGlyphDataChanged) {

				//Since bindings could have been cleared due to datasources being removed, notify that the data of glyphs has been changed
				unsigned int firstGlyphIndex = GetFirstIndexForDataType(DataType::GlyphTrees);
				emit dataChanged(index(firstGlyphIndex), index(firstGlyphIndex + m_dataMapping->GetGlyphGraphs().size() - 1));
			}
		}
		
		return true;
	}

	return false;
}

void DataTransformModel::SetDefaults(const SynGlyphX::DataMappingDefaults& defaults) {

	m_dataMapping->SetDefaults(defaults);
}

void DataTransformModel::SetSceneProperties(const SynGlyphX::SceneProperties& sceneProperties) {

	m_dataMapping->SetSceneProperties(sceneProperties);
}

SynGlyphX::DataTransformMapping::ConstSharedPtr DataTransformModel::GetDataMapping() const {

	return m_dataMapping;
}

void DataTransformModel::LoadDataTransformFile(const QString& filename) {

	Clear();
	beginResetModel();
	m_dataMapping->ReadFromFile(filename.toStdString());
	const SynGlyphX::DataTransformMapping::DatasourceMap& datasources = m_dataMapping->GetDatasources();
	SynGlyphX::DataTransformMapping::DatasourceMap::const_iterator iT = datasources.begin();
	for (; iT != datasources.end(); ++iT) {

		AddDatasourceInfoFromDataEngine(iT->first, iT->second);
	}
	
	ClearAbsentBindings(createIndex(0,0));
	endResetModel();
}

void DataTransformModel::SaveDataTransformFile(const QString& filename) {

	m_dataMapping->WriteToFile(filename.toStdString());
}

void DataTransformModel::ClearAndReset() {

	Clear();
}

void DataTransformModel::Clear() {

	beginResetModel();
	m_tableStatsMap.clear();
	m_numericFields.clear();
	m_dataMapping->Clear();
	endResetModel();
}

void DataTransformModel::AddGlyphFile(const QString& filename) {

	SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphTree;
	QString extension = filename.right(4);
	if (extension == ".csv") {

		glyphTree = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*SynGlyphXANTz::GlyphNodeConverter::CreateGlyphGraphFromCSV(filename.toStdString()).get());
	}
	else if (extension == ".sgt") {

		SynGlyphX::DataMappingGlyphFile fileReader;
		fileReader.ReadFromFile(filename.toStdString());
		glyphTree = fileReader.GetDataMappingGlyphGraph();
	}
	else {

		throw std::invalid_argument("File does not contain a glyph tree");
	}

	AddGlyphTree(glyphTree);
}

void DataTransformModel::AddGlyphTree(SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphTree) {

	if (!glyphTree) {

		throw std::invalid_argument("Can't add empty glyph tree to data mapping");
	}

	int row = m_dataMapping->GetGlyphGraphs().size();
	beginInsertRows(QModelIndex(), row, row);
	m_dataMapping->AddGlyphTree(glyphTree);
	endInsertRows();
}

void DataTransformModel::SetBaseObject(unsigned int position, const SynGlyphX::BaseImage& baseImage) {

	m_dataMapping->SetBaseObject(position, baseImage);
	QModelIndex modelIndex = index(m_dataMapping->GetGlyphGraphs().size() + position);
	emit dataChanged(modelIndex, modelIndex);
}

void DataTransformModel::AddBaseObject(const SynGlyphX::BaseImage& baseImage) {

	int row = m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size();
	beginInsertRows(QModelIndex(), row, row);
	m_dataMapping->AddBaseObject(baseImage);
	endInsertRows();
}

void DataTransformModel::SetLegend(unsigned int position, const SynGlyphX::Legend& legend) {

	m_dataMapping->SetLegend(position, legend);
	QModelIndex modelIndex = index(GetFirstIndexForDataType(DataType::Legends) + position);
	emit dataChanged(modelIndex, modelIndex);
}

void DataTransformModel::AddLegend(const SynGlyphX::Legend& legend) {

	int row = GetFirstIndexForDataType(DataType::Legends) + m_dataMapping->GetLegends().size();
	beginInsertRows(QModelIndex(), row, row);
	m_dataMapping->AddLegend(legend);
	endInsertRows();
}

bool DataTransformModel::IsParentlessRowInDataType(DataType type, int row) const {

	int min = GetFirstIndexForDataType(type);
	int max = 0;
	if (type == DataType::BaseObjects) {

		max = min + m_dataMapping->GetBaseObjects().size();
	}
	else if (type == DataType::DataSources) {

		max = min + m_dataMapping->GetDatasources().size();
	}
	else if (type == DataType::FieldGroup) {

		max = min + m_dataMapping->GetFieldGroupMap().size();
	}
	else if (type == DataType::Legends) {

		max = min + m_dataMapping->GetLegends().size();
	}
	else if (type == DataType::GlyphTrees) {

		max = min + m_dataMapping->GetGlyphGraphs().size();
	}

	return ((row >= min) && (row < max));
}

unsigned int DataTransformModel::GetFirstIndexForDataType(DataType type) const {

	if (type == DataType::BaseObjects) {

		return m_dataMapping->GetGlyphGraphs().size();
	}
	else if (type == DataType::DataSources) {

		return m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size();
	}
	else if (type == DataType::FieldGroup) {

		return m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().size();
	}
	else if (type == DataType::Legends) {

		return m_dataMapping->GetGlyphGraphs().size() + m_dataMapping->GetBaseObjects().size() + m_dataMapping->GetDatasources().size() +
			m_dataMapping->GetFieldGroupMap().size();
	}
	else if (type == DataType::GlyphTrees) {

		return 0;
	}
}

void DataTransformModel::UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyph& newGlyph) {

	if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator vertex(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		m_dataMapping->UpdateGlyph(GetTreeId(index), vertex, newGlyph);
		emit dataChanged(index, index);
	}
}

void DataTransformModel::UpdateGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& subgraph) {

	if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

		throw std::invalid_argument("Index given to MinMaxGlyphTreeModel::GetSubgraph is invalid");
	}

	SynGlyphX::DataMappingGlyphGraph& nonConstGraph = const_cast<SynGlyphX::DataMappingGlyphGraph&>(subgraph);

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator vertex(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	SynGlyphX::DataMappingGlyph glyph = nonConstGraph.GetRoot()->second;
	glyph.GetPosition() = vertex->second.GetPosition();
	UpdateGlyph(index, glyph);

	unsigned int numberOfChildrenOfRootInSubgraph = subgraph.ChildCount(subgraph.GetRoot());
	if (numberOfChildrenOfRootInSubgraph > 0) {

		boost::uuids::uuid treeId = GetTreeId(index);
		unsigned int numberOfChildrenInNewParent = rowCount(index);
		beginInsertRows(index, numberOfChildrenInNewParent, numberOfChildrenInNewParent + numberOfChildrenOfRootInSubgraph - 1);

		for (int i = 0; i < numberOfChildrenOfRootInSubgraph; ++i) {

			m_dataMapping->AddChildTree(treeId, vertex, nonConstGraph.GetSubgraph(nonConstGraph.GetChild(nonConstGraph.GetRoot(), i), true));
		}

		endInsertRows();
	}
}

void DataTransformModel::UpdateGlyphGeometry(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGeometry& structure) {

	if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		glyph->second.GetStructure() = structure;
		emit dataChanged(index, index);
	}
}

void DataTransformModel::UpdateVirtualTopology(const QModelIndex& index, const SynGlyphX::DataMappingVirtualTopology& virtualTopology) {

	if (!m_dataMapping->GetGlyphGraphs().empty() && index.isValid()) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
		glyph->second.GetVirtualTopology() = virtualTopology;
		emit dataChanged(index, index);
	}
}

const SynGlyphX::DataMappingGlyph& DataTransformModel::GetGlyph(const QModelIndex& index) const {

	if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

		throw std::invalid_argument("Index doesn't exist in DataTransformModel");
	}

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	return glyph->second;
}

SynGlyphX::DataMappingGlyphGraph DataTransformModel::GetSubgraph(const QModelIndex& index, bool includeChildren) {

	if (m_dataMapping->GetGlyphGraphs().empty() || !index.isValid()) {

		throw std::invalid_argument("Index doesn't exist in DataTransformModel");
	}

	SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator glyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer()));
	return m_dataMapping->GetSubgraph(GetTreeId(index), glyph, includeChildren);
}

void DataTransformModel::AddChildGlyph(const QModelIndex& parent, const SynGlyphX::DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren) {

	if (!parent.isValid()) {

		throw std::invalid_argument("Can't append children to invalid parent");
	}
	if (numberOfChildren == 0) {

		throw std::invalid_argument("Can't append 0 children");
	}

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));

	unsigned int startingNumberOfChildren = rowCount(parent);
	beginInsertRows(parent, startingNumberOfChildren, startingNumberOfChildren + numberOfChildren - 1);
	m_dataMapping->AddChildGlyph(GetTreeId(parent), parentGlyph, glyphTemplate, numberOfChildren);
	endInsertRows();
}

void DataTransformModel::AddChildGlyphGraph(const QModelIndex& parent, const SynGlyphX::DataMappingGlyphGraph& graph) {

	if (!parent.isValid()) {

		throw std::invalid_argument("Can't append children to invalid parent");
	}

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));

	unsigned int startingNumberOfChildren = rowCount(parent);
	beginInsertRows(parent, startingNumberOfChildren, startingNumberOfChildren);
	m_dataMapping->AddChildTree(GetTreeId(parent), parentGlyph, graph);
	endInsertRows();
}

void DataTransformModel::AddLink(const SynGlyphX::Link& link) {
	m_dataMapping->AddLink(link);
}
void DataTransformModel::ResetDataMappingID() {

	m_dataMapping->ResetID();
}

boost::uuids::uuid DataTransformModel::GetTreeId(int row) const {

	SynGlyphX::DataTransformMapping::DataMappingGlyphGraphMap::const_iterator glyphTree = m_dataMapping->GetGlyphGraphs().begin();
	std::advance(glyphTree, row);
	return glyphTree->first;
}

boost::uuids::uuid DataTransformModel::GetTreeId(const QModelIndex& index) const {

	QModelIndex treeRootIndex = index;
	int rootRow = -1;
	while (treeRootIndex.isValid()) {

		rootRow = treeRootIndex.row();
		treeRootIndex = treeRootIndex.parent();
	}

	return GetTreeId(rootRow);
}

boost::uuids::uuid DataTransformModel::GetDatasourceId(int row) const {

	SynGlyphX::DataTransformMapping::DatasourceMap::const_iterator datasource = m_dataMapping->GetDatasources().begin();
	std::advance(datasource, row - GetFirstIndexForDataType(DataType::DataSources));
	return datasource->first;
}

Qt::DropActions DataTransformModel::supportedDropActions() const {

	return Qt::MoveAction;
}

Qt::ItemFlags DataTransformModel::flags(const QModelIndex& index) const {

	if (!index.isValid()) {
		return 0;
	}

	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	
	if (GetDataType(index) == DataType::GlyphTrees) {

		flags |= Qt::ItemIsDropEnabled;

		if (index.parent().isValid()) {
			
			flags |= Qt::ItemIsDragEnabled;
		}
	}

	return flags;
}

QStringList DataTransformModel::mimeTypes() const {

	QStringList types;
	types.push_back(SynGlyphX::GlyphMimeData::s_format);
	return types;
}

bool DataTransformModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const {

	const SynGlyphX::GlyphMimeData* glyphData = qobject_cast<const SynGlyphX::GlyphMimeData*>(data);

	if ((glyphData == NULL) || (GetDataType(parent) != DataType::GlyphTrees)) {

		return false;
	}

	SynGlyphX::DataMappingGlyphGraph::GlyphIterator parentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
	const QModelIndexList& indexes = glyphData->GetGlyphs();
	for (int j = 0; j < indexes.length(); ++j) {

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator mimeDataGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(indexes[j].internalPointer()));
		if (mimeDataGlyph.owner() != parentGlyph.owner()) {

			return false;
		}
	}

	return true;
}

QMimeData* DataTransformModel::mimeData(const QModelIndexList& indexes) const {

	SynGlyphX::GlyphMimeData* mimeData = new SynGlyphX::GlyphMimeData(indexes);
	return mimeData;
}

bool DataTransformModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {

	//canDropMimeData isn't being called when it should as per this bug:
	//https://bugreports.qt.io/browse/QTBUG-30534
	//Thus it is being called here until we upgrade to Qt 5.4.1 or later
	if (!canDropMimeData(data, action, row, column, parent)) {

		return false;
	}

	const SynGlyphX::GlyphMimeData* glyphData = qobject_cast<const SynGlyphX::GlyphMimeData*>(data);

	if ((glyphData != NULL) && (row == -1)) {

		SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator newParentGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(parent.internalPointer()));
		const QModelIndexList& indexes = glyphData->GetGlyphs();

		bool glyphsMoved = false;

		for (int j = 0; j < indexes.length(); ++j) {

			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator oldGlyph(static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(indexes[j].internalPointer()));
			SynGlyphX::DataMappingGlyphGraph::ConstGlyphIterator oldParentGlyph = oldGlyph.owner()->parent(oldGlyph);

			//Only drop if parents are different
			if (oldParentGlyph != newParentGlyph) {
				
				unsigned int numberOfChildren = newParentGlyph.owner()->children(newParentGlyph);

				//Only do an insert here.  The MoveAction will take care of deleting the old object
				beginInsertRows(parent, numberOfChildren, numberOfChildren);
				SynGlyphX::DataMappingGlyphGraph oldGlyphSubtree = GetSubgraph(indexes[j], true);
				m_dataMapping->AddChildTreeResetPosition(GetTreeId(parent), newParentGlyph.deconstify(), oldGlyphSubtree);
				endInsertRows();
				glyphsMoved = true;
			}
		}

		return glyphsMoved;
	}

	return false;
}

const SynGlyphX::DataTransformMapping::FieldGroupMap& DataTransformModel::GetFieldGroupMap() const {

	return m_dataMapping->GetFieldGroupMap();
}

void DataTransformModel::UpdateFieldGroup(const SynGlyphX::DataTransformMapping::FieldGroupName& groupName, const SynGlyphX::FieldGroup& fieldGroup) {

	const SynGlyphX::DataTransformMapping::FieldGroupMap& fieldGroupMap = m_dataMapping->GetFieldGroupMap();
	SynGlyphX::DataTransformMapping::FieldGroupMap::const_iterator groupIterator = fieldGroupMap.find(groupName);
	bool addNew = (groupIterator == fieldGroupMap.end());
	
	int firstIndex = GetFirstIndexForDataType(DataType::FieldGroup);
	int lastIndex = firstIndex + fieldGroupMap.size() - 1;
	if (addNew) {

		lastIndex++;
		beginInsertRows(QModelIndex(), lastIndex, lastIndex);
	}

	m_dataMapping->UpdateFieldGroup(groupName, fieldGroup);

	if (addNew) {

		endInsertRows();
	}

	emit dataChanged(index(firstIndex), index(lastIndex));
}

void DataTransformModel::RemoveFieldGroup(const SynGlyphX::DataTransformMapping::FieldGroupName& groupName) {

	RemoveFieldGroup(groupName, true);
}

void DataTransformModel::RemoveFieldGroup(const SynGlyphX::DataTransformMapping::FieldGroupName& groupName, bool emitGlyphDataChanged) {

	const SynGlyphX::DataTransformMapping::FieldGroupMap& fieldGroupMap = m_dataMapping->GetFieldGroupMap();
	SynGlyphX::DataTransformMapping::FieldGroupMap::const_iterator groupIterator = fieldGroupMap.find(groupName);

	if (groupIterator == fieldGroupMap.end()) {

		return;
	}

	int row = GetFirstIndexForDataType(DataType::FieldGroup) + std::distance(fieldGroupMap.begin(), groupIterator);

	beginRemoveRows(QModelIndex(), row, row);
	m_dataMapping->RemoveFieldGroup(groupName);
	endRemoveRows();

	if (emitGlyphDataChanged) {

		//Since references to the removed field group could have been cleared, notify that the data of glyphs has been changed
		unsigned int firstGlyphIndex = GetFirstIndexForDataType(DataType::GlyphTrees);
		emit dataChanged(index(firstGlyphIndex), index(firstGlyphIndex + m_dataMapping->GetGlyphGraphs().size() - 1));
	}
}

const SynGlyphX::DataTransformMapping::FieldGroupName& DataTransformModel::GetFieldGroupName(int row) const {

	SynGlyphX::DataTransformMapping::FieldGroupMap::const_iterator fieldGroup = m_dataMapping->GetFieldGroupMap().begin();
	std::advance(fieldGroup, row - GetFirstIndexForDataType(DataType::FieldGroup));
	return fieldGroup->first;
}

const DataTransformModel::NumericFieldsByTable& DataTransformModel::GetNumericFieldsByTable() const {

	return m_numericFields;
}

void DataTransformModel::RemoveAllAdditionalData(const boost::uuids::uuid& datasourceId) {

	for (const auto& table : m_dataMapping->GetDatasources().at(datasourceId)->GetTables()) {

		SynGlyphX::InputTable inputTable(datasourceId, table.first);

		if (m_numericFields.count(inputTable) != 0) {

			m_numericFields.erase(inputTable);
		}
		m_tableStatsMap.erase(inputTable);
	}
}

const DataTransformModel::TableStatsMap& DataTransformModel::GetTableStatsMap() const {

	return m_tableStatsMap;
}

void DataTransformModel::AddDatasourceInfoFromDataEngine(const boost::uuids::uuid& datasourceId, const SynGlyphX::Datasource::SharedPtr datasource) {

	QStringList chosenTables;
	QString sourceTypeString;

	try {

		if (datasource->GetSourceType() == SynGlyphX::Datasource::SourceType::File) {

			SynGlyphX::FileDatasource::SharedPtr fileDatasource = std::dynamic_pointer_cast<SynGlyphX::FileDatasource>(datasource);
			sourceTypeString = QString::fromStdWString(SynGlyphX::FileDatasource::s_fileTypePrefixes.left.at(fileDatasource->GetFileType()));

			if (fileDatasource->GetFileType() == SynGlyphX::FileDatasource::SQLITE3) {

				ConnectToDatabase(QString::fromStdWString(L"sqlite:" + fileDatasource->GetFilename()),
								  QString::fromStdWString(fileDatasource->GetUsername()),
								  QString::fromStdWString(fileDatasource->GetPassword()),
								  sourceTypeString);

				if (datasource->DoAnyTablesHaveQueries()) {

					SynGlyphX::Datasource::Tables::const_iterator tablesQueryIterator = datasource->GetTables().begin();
					m_dataEngineConnection->setQueryTables(QString::fromStdWString(tablesQueryIterator->second.GetQuery()));
					chosenTables << QString::fromStdWString(tablesQueryIterator->first);
				}
				else {

					chosenTables = GetChosenTables("", fileDatasource->GetTableNames());
				}

			}
			else if (fileDatasource->GetFileType() == SynGlyphX::FileDatasource::FileType::CSV) {

				m_dataEngineConnection->loadCSV(SynGlyphX::StringConvert::ToStdString(fileDatasource->GetFilename()).c_str());
				chosenTables << QString::fromStdWString(SynGlyphX::FileDatasource::SingleTableName);
				//GenerateStats(SynGlyphX::InputTable(datasourceId, SynGlyphX::FileDatasource::SingleTableName), 0, SynGlyphX::FileDatasource::FileType::CSV);
			}
		}
		else if (datasource->GetSourceType() == SynGlyphX::Datasource::SourceType::DatabaseServer) {

			//std::vector<DataEngineConnection::ForeignKey> fkeys = dec->getForeignKeys(//table_name);
			//fkeys.at(0).key;
			//fkeys.at(0).origin;
			//fkeys.at(0).value;
			//dec->setChosenTables(chosenTables);
			/*
			QString query = "SELECT inventory_fact.qty_in_stock, product_dimension.product_price, ";
			query += "date_dimension.day_of_week, warehouse_dimension.warehouse_name FROM (inventory_fact ";
			query += "INNER JOIN product_dimension ON (inventory_fact.product_key=product_dimension.product_key) ";
			query += "INNER JOIN date_dimension ON (inventory_fact.date_key=date_dimension.date_key) ";
			query += "INNER JOIN warehouse_dimension ON (inventory_fact.warehouse_key=warehouse_dimension.warehouse_key))";*/

			SynGlyphX::DatabaseServerDatasource::SharedPtr dbmsDatasource = std::dynamic_pointer_cast<SynGlyphX::DatabaseServerDatasource>(datasource);
			sourceTypeString = QString::fromStdWString(SynGlyphX::DatabaseServerDatasource::s_dbTypePrefixes.left.at(dbmsDatasource->GetDBType()));

			ConnectToDatabase(QString::fromStdWString(dbmsDatasource->GetFullJDBCConnectionString()),
							  QString::fromStdWString(dbmsDatasource->GetUsername()),
							  QString::fromStdWString(dbmsDatasource->GetPassword()),
							  sourceTypeString);

			if (datasource->DoAnyTablesHaveQueries()) {

				SynGlyphX::Datasource::Tables::const_iterator tablesQueryIterator = datasource->GetTables().begin();
				m_dataEngineConnection->setQueryTables(QString::fromStdWString(tablesQueryIterator->second.GetQuery()));
				chosenTables << QString::fromStdWString(tablesQueryIterator->first);
			}
			else {
				chosenTables = GetChosenTables(
					QString::fromStdWString(dbmsDatasource->GetSchema()),
					dbmsDatasource->GetTableNames());
			}

			//QString query = "SELECT City.Population, Country.Code FROM (City INNER JOIN Country ON (City.CountryCode=Country.Code))";
			/*
			QString query = "SELECT inventory_fact.qty_in_stock, product_dimension.product_price, ";
			query += "date_dimension.day_of_week, warehouse_dimension.warehouse_name FROM (inventory_fact ";
			query += "INNER JOIN product_dimension ON (inventory_fact.product_key=product_dimension.product_key) ";
			query += "INNER JOIN date_dimension ON (inventory_fact.date_key=date_dimension.date_key) ";
			query += "INNER JOIN warehouse_dimension ON (inventory_fact.warehouse_key=warehouse_dimension.warehouse_key))";
			dec->setQueryTables(query);*/
		}

		for (int i = 0; i < chosenTables.size(); ++i) {

			GenerateStats(SynGlyphX::InputTable(datasourceId, chosenTables[i].toStdWString()), i, sourceTypeString);
		}

		m_dataEngineConnection->closeConnection();
	}
	catch (const std::exception& e) {

		m_dataEngineConnection->closeConnection();
		throw;
	}
}

void DataTransformModel::ConnectToDatabase(const QString& url, const QString& username, const QString& password, const QString& db_type) {

	if (!m_dataEngineConnection->IsConnectionOpen(url)) {

		m_dataEngineConnection->connectToServer(url, username, password, db_type);
	}
}

QStringList DataTransformModel::GetChosenTables(const QString& schema, const SynGlyphX::Datasource::TableNames& tables) {

	QStringList chosenTables;
	QStringList connectionTables;
	if (schema.isEmpty()) {

		connectionTables = m_dataEngineConnection->getTables();
	}
	else {

		connectionTables = m_dataEngineConnection->getSchemaTableNames(schema);
	}

	if (connectionTables.isEmpty()) {

		if (schema.isEmpty()) {

			throw std::runtime_error(tr("No tables listed can be found in database").toStdString().c_str());
		}
		else {

			throw std::runtime_error((tr("No tables listed can be found in ") + schema).toStdString().c_str());
		}
	}

	for (const auto& table : tables) {

		QString qTable = QString::fromStdWString(table);
		if (connectionTables.contains(qTable)) {

			chosenTables.push_back(qTable);
		}
	}

	m_dataEngineConnection->setChosenTables(chosenTables);

	return chosenTables;
}

//JDBC GENERATE STATS
void DataTransformModel::GenerateStats(const SynGlyphX::InputTable inputTable, int place, const QString& sourceTypeString) {

	TableStats tableStats;

	DataEngine::DataEngineStatement des;
	des.prepare(m_dataEngineConnection->getEnv(), m_dataEngineConnection->getJcls(), sourceTypeString); 

	SynGlyphX::WStringVector numericCols;
	des.getFieldsForTable(place);

	while (des.hasNext()) {

		QString field = des.getField();
		QString type = des.getType();

		if (type == "real") {
			
			numericCols.push_back(field.toStdWString());
		}

		QStringList fieldStats;
		fieldStats.append(field);
		fieldStats.append(type);
		fieldStats.append(des.getMin());
		fieldStats.append(des.getMax());
		fieldStats.append(des.getAverage());
		fieldStats.append(des.getCount());
		fieldStats.append(des.getDistinct());
		tableStats.append(fieldStats);
	}

	if (!numericCols.empty()) {

		m_numericFields.emplace(inputTable, numericCols);
	}
	m_tableStatsMap.emplace(inputTable, tableStats);
}
//END JDBC