#include "singleglyphrolestablemodel.h"
#include <QtGui/QColor>
#include "sourcedatamanager.h"
#include <QtWidgets/QMessageBox>
#include <stack>
#include "interpolationmappingfunction.h"
#include "valuemappingfunction.h"
#include "colorminmaxwidget.h"
#include "doubleminmaxwidget.h"
#include "intminmaxwidget.h"
#include "inputfieldmimedata.h"

SingleGlyphRolesTableModel::SingleGlyphRolesTableModel(DataTransformModel* dataTransformModel, QObject *parent)
	: QAbstractTableModel(parent),
	m_dataTransformModel(dataTransformModel),
	//m_glyphTree(nullptr),
	m_glyphTreeID(boost::uuids::nil_uuid()),
	m_selectedDataTransformModelIndex(QModelIndex())
{
	m_propertyHeaders.push_back(tr("Position X"));
	m_propertyHeaders.push_back(tr("Position Y"));
	m_propertyHeaders.push_back(tr("Position Z"));
	m_propertyHeaders.push_back(tr("Rotation X"));
	m_propertyHeaders.push_back(tr("Rotation Y"));
	m_propertyHeaders.push_back(tr("Rotation Z"));
	m_propertyHeaders.push_back(tr("Scale X"));
	m_propertyHeaders.push_back(tr("Scale Y"));
	m_propertyHeaders.push_back(tr("Scale Z"));
	m_propertyHeaders.push_back(tr("Color"));
	m_propertyHeaders.push_back(tr("Transparency"));
	m_propertyHeaders.push_back(tr("Tag"));
	m_propertyHeaders.push_back(tr("Description"));
	m_propertyHeaders.push_back(tr("Rotation Rate X"));
	m_propertyHeaders.push_back(tr("Rotation Rate Y"));
	m_propertyHeaders.push_back(tr("Rotation Rate Z"));
	m_propertyHeaders.push_back(tr("Virtual Topology Type"));
	m_propertyHeaders.push_back(tr("Geometry Shape"));
	m_propertyHeaders.push_back(tr("Geometry Surface"));
	m_propertyHeaders.push_back(tr("Torus Ratio"));
	
	m_columnHeaders.push_back(tr("Default(s)"));
	m_columnHeaders.push_back(tr("Function"));
	m_columnHeaders.push_back(tr("Input"));
}

SingleGlyphRolesTableModel::~SingleGlyphRolesTableModel()
{

}

int SingleGlyphRolesTableModel::columnCount(const QModelIndex& parent) const {

	if (parent.isValid()) {

		return 0;
	}
	else {

		return 3;
	}
}

const SynGlyphX::InputField SingleGlyphRolesTableModel::GetInputField(SynGlyphX::InputField::HashID fieldID) const {

	if (fieldID == 0) {

		return SynGlyphX::InputField();
	}
	else {

		return m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().at(m_glyphTreeID)->GetInputFields().at(fieldID);
	}
}

QVariant SingleGlyphRolesTableModel::data(const QModelIndex& index, int role) const {

	if ((role == Qt::EditRole) && index.isValid()) { // && m_glyph.valid()) {

		QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndex, index.row() + DataTransformModel::PropertyRole::PositionX);
		PropertyType propertyType = GetFieldType(index.row());
		if (propertyType == PropertyType::Color) {

			SynGlyphX::ColorMappingProperty colorProp = prop.value<SynGlyphX::ColorMappingProperty>();
			if (index.column() == 1) {

				return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(colorProp.GetMappingFunctionData()->GetFunction()));
			}
			else if (index.column() == 2) {

				return QVariant::fromValue(GetInputField(colorProp.GetBinding().GetInputFieldID()));
			}
			else {

				return QVariant::fromValue(colorProp.GetValue());
			}
		}
		else if (propertyType == PropertyType::Text) {

			SynGlyphX::TextMappingProperty textProp = prop.value<SynGlyphX::TextMappingProperty>();
			if (index.column() == 1) {

				return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(textProp.GetMappingFunctionData()->GetFunction()));
			}
			else if (index.column() == 2) {

				return QVariant::fromValue(GetInputField(textProp.GetBinding().GetInputFieldID()));
			}
			else {

				return QString::fromStdWString(textProp.GetValue());
			}
		}
		else if (propertyType == PropertyType::GeometryShape) {

			SynGlyphX::GeometryShapeMappingProperty shapeProp = prop.value<SynGlyphX::GeometryShapeMappingProperty>();
			if (index.column() == 1) {

				return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(shapeProp.GetMappingFunctionData()->GetFunction()));
			}
			else if (index.column() == 2) {

				return QVariant::fromValue(GetInputField(shapeProp.GetBinding().GetInputFieldID()));
			}
			else {

				return QString::fromStdWString(SynGlyphX::GlyphGeometryInfo::s_shapeNames.left.at(shapeProp.GetValue()));
			}
		}
		else if (propertyType == PropertyType::VirtualTopology) {

			SynGlyphX::VirtualTopologyMappingProperty topologyProp = prop.value<SynGlyphX::VirtualTopologyMappingProperty>();
			if (index.column() == 1) {

				return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(topologyProp.GetMappingFunctionData()->GetFunction()));
			}
			else if (index.column() == 2) {

				return QVariant::fromValue(GetInputField(topologyProp.GetBinding().GetInputFieldID()));
			}
			else {

				return QString::fromStdWString(SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left.at(topologyProp.GetValue()));
			}
		}
		else if (propertyType == PropertyType::Numeric) {

			SynGlyphX::NumericMappingProperty numericProp = prop.value<SynGlyphX::NumericMappingProperty>();
			if (index.column() == 1) {

				return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(numericProp.GetMappingFunctionData()->GetFunction()));
			}
			else if (index.column() == 2) {

				return QVariant::fromValue(GetInputField(numericProp.GetBinding().GetInputFieldID()));
			}
			else {

				return QVariant::fromValue(numericProp.GetValue());
			}
		}
		else {

			return prop;
		}
	}

	return QVariant();
}

int	SingleGlyphRolesTableModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		return DataTransformModel::PropertyRole::GeometryTorusRatio - DataTransformModel::DataTypeRole;
	}
	else {

		return 0;
	}
}

void SingleGlyphRolesTableModel::SetMinMaxGlyph(const QModelIndex& index) {

	if (index.isValid()) {

		std::stack<unsigned int> childIndices;
		QModelIndex parent = index;
		while (parent.isValid()) {

			childIndices.push(parent.row());
			parent = parent.parent();
		}

		SynGlyphX::DataTransformMapping::DataMappingGlyphGraphMap::const_iterator glyphTree = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().begin();
		std::advance(glyphTree, childIndices.top());
		childIndices.pop();

		QObject::disconnect(m_dataChangedConnection);

		beginResetModel();
		m_selectedDataTransformModelIndex = index;
		m_glyphTreeID = glyphTree->first;
		/*m_glyphTree = glyphTree->second;
		SynGlyphX::DataMappingGlyphGraph::const_iterator iT = m_glyphTree->root();

		while (!childIndices.empty()) {

			iT = m_glyphTree->child(iT, childIndices.top());
			childIndices.pop();
		}
		m_glyph = iT;*/
		endResetModel();

		m_dataChangedConnection = QObject::connect(m_dataTransformModel, &DataTransformModel::dataChanged, this, &SingleGlyphRolesTableModel::OnSourceModelDataUpdated);
	}
	else {

		Clear();
	}
}

void SingleGlyphRolesTableModel::Clear() {

	QObject::disconnect(m_dataChangedConnection);

	beginResetModel();
	m_glyphTreeID = boost::uuids::nil_uuid();
	m_selectedDataTransformModelIndex = QModelIndex();
	endResetModel();
}

bool SingleGlyphRolesTableModel::IsClear() const {

	return (!m_selectedDataTransformModelIndex.isValid());
}

bool SingleGlyphRolesTableModel::IsInputFieldCompatible(const SynGlyphX::InputField& inputField) const {

	if (!IsClear()) {

		const SynGlyphX::DataMappingGlyphGraph::InputFieldMap& inputFields = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().at(m_glyphTreeID)->GetInputFields();
		if (inputFields.empty()) {

			return true;
		}

		const SynGlyphX::InputField& firstInputField = inputFields.begin()->second;
		if ((firstInputField.GetTable() == inputField.GetTable()) && (firstInputField.GetDatasourceID() == inputField.GetDatasourceID())) {

			return true;
		}
	}

	return false;
}

bool SingleGlyphRolesTableModel::DoesGlyphHaveAssociatedDatasoruceTable() const {

	const SynGlyphX::DataMappingGlyphGraph::InputFieldMap& inputFields = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().at(m_glyphTreeID)->GetInputFields();
	return (!inputFields.empty());
}

const SynGlyphX::DatasourceTable& SingleGlyphRolesTableModel::GetAssociatedDatasourceTable() const {

	if (!DoesGlyphHaveAssociatedDatasoruceTable()) {

		throw std::exception("Glyph tree associated with index has no datasource table associated with it");
	}

	const SynGlyphX::InputTable& inputTable = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().at(m_glyphTreeID)->GetInputFields().begin()->second;
}

Qt::ItemFlags SingleGlyphRolesTableModel::flags(const QModelIndex & index) const {

	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool SingleGlyphRolesTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if ((index.isValid()) && (role == Qt::EditRole)) { // && m_glyph.valid()) {

		if (index.column() == 2) {

			SynGlyphX::DataMappingGlyph::MappableField mappableField = static_cast<SynGlyphX::DataMappingGlyph::MappableField>(index.row());
			SynGlyphX::InputField inputField = value.value<SynGlyphX::InputField>();
			if (inputField.IsValid()) {

				m_dataTransformModel->SetInputField(m_glyphTreeID, m_selectedDataTransformModelIndex, mappableField, inputField);
			}
			else {

				m_dataTransformModel->ClearInputBinding(m_glyphTreeID, m_selectedDataTransformModelIndex, mappableField);
			}
		}
		else {

			int sourceDataRole = index.row() + DataTransformModel::PropertyRole::PositionX;
			QVariant newProp;

			if (sourceDataRole >= DataTransformModel::PropertyRole::GeometrySurface) {

				newProp = value;
			}
			else {

				bool updateNewProp = false;
				QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndex, sourceDataRole);
				PropertyType propertyType = GetFieldType(index.row());
				if (propertyType == PropertyType::Color) {

					SynGlyphX::ColorMappingProperty colorProp = prop.value<SynGlyphX::ColorMappingProperty>();
					if (index.column() == 1) {

						SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
						if (function != colorProp.GetMappingFunctionData()->GetFunction()) {

							colorProp.SetMappingFunctionData(CreateNewMappingFunction(function, PropertyType::Color));
							updateNewProp = true;
						}
					}
					else {

						SynGlyphX::ColorMinDiff newColorMinDiff = value.value<SynGlyphX::ColorMinDiff>();
						if (colorProp.GetValue() != newColorMinDiff) {

							colorProp.GetValue() = newColorMinDiff;
							updateNewProp = true;
						}
					}

					if (updateNewProp) {

						newProp.setValue(colorProp);
					}
				}
				/*else if (propertyType == PropertyType::Text) { //Currently Text fields do not have a value nor choice in mapping functions

					SynGlyphX::TextMappingProperty textProp = prop.value<SynGlyphX::TextMappingProperty>();
					if (index.column() == 1) {

					return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(textProp.GetMappingFunctionData()->GetFunction()));
					}
					else {

					return QString::fromStdWString(textProp.GetValue());
					}
					}*/
				else if (propertyType == PropertyType::GeometryShape) {

					SynGlyphX::GeometryShapeMappingProperty shapeProp = prop.value<SynGlyphX::GeometryShapeMappingProperty>();
					if (index.column() == 1) {

						SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
						if (function != shapeProp.GetMappingFunctionData()->GetFunction()) {

							shapeProp.SetMappingFunctionData(CreateNewMappingFunction(function, PropertyType::GeometryShape));
							updateNewProp = true;
						}
					}
					else {

						SynGlyphX::GlyphGeometryInfo::Shape newShape;
						if (value.type() == QVariant::Type::String) {

							newShape = SynGlyphX::GlyphGeometryInfo::s_shapeNames.right.at(value.toString().toStdWString());
						}
						else {

							newShape = value.value<SynGlyphX::GlyphGeometryInfo::Shape>();
						}

						if (shapeProp.GetValue() != newShape) {

							shapeProp.GetValue() = newShape;
							updateNewProp = true;
						}
					}

					if (updateNewProp) {

						newProp.setValue(shapeProp);
					}
				}
				else if (propertyType == PropertyType::VirtualTopology) {

					SynGlyphX::VirtualTopologyMappingProperty topologyProp = prop.value<SynGlyphX::VirtualTopologyMappingProperty>();
					if (index.column() == 1) {

						SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
						if (function != topologyProp.GetMappingFunctionData()->GetFunction()) {

							topologyProp.SetMappingFunctionData(CreateNewMappingFunction(function, PropertyType::VirtualTopology));
							updateNewProp = true;
						}
					}
					else {

						SynGlyphX::VirtualTopologyInfo::Type newVirtualTopology;
						if (value.type() == QVariant::Type::String) {

							newVirtualTopology = SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.right.at(value.toString().toStdWString());
						}
						else {

							newVirtualTopology = value.value<SynGlyphX::VirtualTopologyInfo::Type>();
						}

						if (topologyProp.GetValue() != newVirtualTopology) {

							topologyProp.GetValue() = newVirtualTopology;
							updateNewProp = true;
						}
					}

					if (updateNewProp) {

						newProp.setValue(topologyProp);
					}
				}
				else if (propertyType == PropertyType::Numeric) {

					SynGlyphX::NumericMappingProperty numericProp = prop.value<SynGlyphX::NumericMappingProperty>();
					if (index.column() == 1) {

						SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
						if (function != numericProp.GetMappingFunctionData()->GetFunction()) {

							numericProp.SetMappingFunctionData(CreateNewMappingFunction(function, PropertyType::Numeric));
							updateNewProp = true;
						}
					}
					else {

						SynGlyphX::DoubleMinDiff newNumericMinDiff = value.value<SynGlyphX::DoubleMinDiff>();
						if (numericProp.GetValue() != newNumericMinDiff) {

							numericProp.GetValue() = newNumericMinDiff;
							updateNewProp = true;
						}
					}

					if (updateNewProp) {

						newProp.setValue(numericProp);
					}
				}
			}

			if (newProp.isValid()) {

				if (m_dataTransformModel->setData(m_selectedDataTransformModelIndex, newProp, sourceDataRole)) {

					//emit dataChanged(index, index);
					return true;
				}
			}
		}
	}

	return false;
}

QVariant SingleGlyphRolesTableModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (role == Qt::DisplayRole) {

		if (orientation == Qt::Horizontal) {

			if (section < columnCount()) {
				return m_columnHeaders[section];
			}
		}
		else {
			
			if (section < rowCount()) {
				return m_propertyHeaders[section];
			}
		}
	}

	return QVariant();
}

SynGlyphX::DataTransformMapping::ConstSharedPtr SingleGlyphRolesTableModel::GetDataTransformMapping() const {

	return m_dataTransformModel->GetDataMapping();
}

bool SingleGlyphRolesTableModel::IsCurrentGlyphRoot() const {

	/*if (m_glyphTree == nullptr) {

		return false;
	}

	return (m_glyphTree->root() == m_glyph.constify());*/

	return (!m_selectedDataTransformModelIndex.parent().isValid());
}

void SingleGlyphRolesTableModel::ClearInputBindings() {

	for (int i = 0; i < rowCount(); ++i) {

		m_dataTransformModel->ClearInputBinding(m_glyphTreeID, m_selectedDataTransformModelIndex, static_cast<SynGlyphX::DataMappingGlyph::MappableField>(i));
	}
	emit dataChanged(index(0, columnCount() - 1), index(rowCount() - 1, columnCount() - 1));
}

SingleGlyphRolesTableModel::PropertyType SingleGlyphRolesTableModel::GetFieldType(int row) const {

	if ((row == 11) || (row == 12)) {

		return PropertyType::Text;
	}
	else if (row == 9) {

		return PropertyType::Color;
	}
	else if (row == 16) {

		return PropertyType::VirtualTopology;
	}
	else if (row == 17) {

		return PropertyType::GeometryShape;
	}
	else if (row > 17) {

		return PropertyType::NonMappable;
	}
	else {

		return PropertyType::Numeric;
	}
}

SynGlyphX::MappingFunctionData::SharedPtr SingleGlyphRolesTableModel::CreateNewMappingFunction(SynGlyphX::MappingFunctionData::Function function, PropertyType type) const {

	if ((function == SynGlyphX::MappingFunctionData::Function::LinearInterpolation) || (function == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation)) {

		return std::make_shared<SynGlyphX::InterpolationMappingData>(function == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation);
	}

	if (type == PropertyType::Color) {

		if (function == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

			return std::make_shared<SynGlyphX::Numeric2ColorMappingData>();
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Text2Value) {

			return std::make_shared<SynGlyphX::Text2ColorMappingData>();
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Range2Value) {

			return std::make_shared<SynGlyphX::Range2ColorMappingData>();
		}
		else if ((function == SynGlyphX::MappingFunctionData::Function::LinearInterpolation) || (function == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation)) {

			return std::make_shared<SynGlyphX::InterpolationMappingData>(function == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation);
		}
	}
	else if (type == PropertyType::Numeric) {

		if (function == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

			return std::make_shared<SynGlyphX::Numeric2NumericMappingData>();
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Text2Value) {

			return std::make_shared<SynGlyphX::Text2NumericMappingData>();
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Range2Value) {

			return std::make_shared<SynGlyphX::Range2NumericMappingData>();
		}
		else if ((function == SynGlyphX::MappingFunctionData::Function::LinearInterpolation) || (function == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation)) {

			return std::make_shared<SynGlyphX::InterpolationMappingData>(function == SynGlyphX::MappingFunctionData::Function::LogarithmicInterpolation);
		}
	}
	else if (type == PropertyType::GeometryShape) {

		if (function == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

			return std::make_shared<SynGlyphX::Numeric2ShapeMappingData>();
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Text2Value) {

			return std::make_shared<SynGlyphX::Text2ShapeMappingData>();
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Range2Value) {

			return std::make_shared<SynGlyphX::Range2ShapeMappingData>();
		}
	}
	else if (type == PropertyType::VirtualTopology) {

		if (function == SynGlyphX::MappingFunctionData::Function::Numeric2Value) {

			SynGlyphX::Numeric2VirtualTopologyMappingData::SharedPtr mappingData = std::make_shared<SynGlyphX::Numeric2VirtualTopologyMappingData>();
			mappingData->SetDefaultValue(SynGlyphX::VirtualTopologyInfo::Type::Circle);
			return mappingData;
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Text2Value) {

			SynGlyphX::Text2VirtualTopologyMappingData::SharedPtr mappingData = std::make_shared<SynGlyphX::Text2VirtualTopologyMappingData>();
			mappingData->SetDefaultValue(SynGlyphX::VirtualTopologyInfo::Type::Circle);
			return mappingData;
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Range2Value) {

			SynGlyphX::Range2VirtualTopologyMappingData::SharedPtr mappingData = std::make_shared<SynGlyphX::Range2VirtualTopologyMappingData>();
			mappingData->SetDefaultValue(SynGlyphX::VirtualTopologyInfo::Type::Circle);
			return mappingData;
		}
	}
	
	//If nothing else return a none mapping function
	return std::make_shared<SynGlyphX::MappingFunctionData>();
}

SynGlyphX::MappingFunctionData::ConstSharedPtr SingleGlyphRolesTableModel::GetMappingFunction(int row) const {

	QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndex, row + DataTransformModel::PropertyRole::PositionX);
	PropertyType propertyType = GetFieldType(row);
	if (propertyType == PropertyType::Color) {

		SynGlyphX::ColorMappingProperty colorProp = prop.value<SynGlyphX::ColorMappingProperty>();
		return colorProp.GetMappingFunctionData();
	}
	else if (propertyType == PropertyType::Text) {

		SynGlyphX::TextMappingProperty textProp = prop.value<SynGlyphX::TextMappingProperty>();
		return textProp.GetMappingFunctionData();
	}
	else if (propertyType == PropertyType::GeometryShape) {

		SynGlyphX::GeometryShapeMappingProperty shapeProp = prop.value<SynGlyphX::GeometryShapeMappingProperty>();
		return shapeProp.GetMappingFunctionData();
	}
	else if (propertyType == PropertyType::VirtualTopology) {

		SynGlyphX::VirtualTopologyMappingProperty topologyProp = prop.value<SynGlyphX::VirtualTopologyMappingProperty>();
		return topologyProp.GetMappingFunctionData();
	}
	else {

		SynGlyphX::NumericMappingProperty numericProp = prop.value<SynGlyphX::NumericMappingProperty>();
		return numericProp.GetMappingFunctionData();
	}
}

void SingleGlyphRolesTableModel::SetMappingFunction(int row, SynGlyphX::MappingFunctionData::SharedPtr mappingFunction) {

	int sourceDataRole = row + DataTransformModel::PropertyRole::PositionX;
	QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndex, sourceDataRole);
	PropertyType propertyType = GetFieldType(row);
	QVariant newProp;
	if (propertyType == PropertyType::Color) {

		SynGlyphX::ColorMappingProperty colorProp = prop.value<SynGlyphX::ColorMappingProperty>();
		colorProp.SetMappingFunctionData(mappingFunction);
		newProp.setValue(colorProp);
	}
	else if (propertyType == PropertyType::Text) {

		SynGlyphX::TextMappingProperty textProp = prop.value<SynGlyphX::TextMappingProperty>();
		textProp.SetMappingFunctionData(mappingFunction);
		newProp.setValue(textProp);
	}
	else if (propertyType == PropertyType::GeometryShape) {

		SynGlyphX::GeometryShapeMappingProperty shapeProp = prop.value<SynGlyphX::GeometryShapeMappingProperty>();
		shapeProp.SetMappingFunctionData(mappingFunction);
		newProp.setValue(shapeProp);
	}
	else if (propertyType == PropertyType::VirtualTopology) {

		SynGlyphX::VirtualTopologyMappingProperty topologyProp = prop.value<SynGlyphX::VirtualTopologyMappingProperty>();
		topologyProp.SetMappingFunctionData(mappingFunction);
		newProp.setValue(topologyProp);
	}
	else {

		SynGlyphX::NumericMappingProperty numericProp = prop.value<SynGlyphX::NumericMappingProperty>();
		numericProp.SetMappingFunctionData(mappingFunction);
		newProp.setValue(numericProp);
	}

	if (newProp.isValid()) {

		m_dataTransformModel->setData(m_selectedDataTransformModelIndex, newProp, sourceDataRole);
	}
}

void SingleGlyphRolesTableModel::OnSourceModelDataUpdated(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {

	if ((m_selectedDataTransformModelIndex == topLeft) && (m_selectedDataTransformModelIndex == bottomRight)) {

		if (roles.empty()) {

			beginResetModel();
			endResetModel();
		}
		else {

			Q_FOREACH(int role, roles) {

				if (role >= DataTransformModel::PropertyRole::PositionX) {

					int row = role - DataTransformModel::PropertyRole::PositionX;
					emit dataChanged(index(row, 0), index(row, columnCount() - 1));
				}
			}
		}
	}
}