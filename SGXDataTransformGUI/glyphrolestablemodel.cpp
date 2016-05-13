#include "glyphrolestablemodel.h"
#include <QtGui/QColor>
#include <QtWidgets/QMessageBox>
#include <stack>
#include "interpolationmappingfunction.h"
#include "valuemappingfunction.h"
#include "colorminmaxwidget.h"
#include "doubleminmaxwidget.h"
#include "intminmaxwidget.h"
#include "inputfieldmimedata.h"
#include "nonmappablegeometrywidget.h"
#include "glyphenumcombobox.h"

GlyphRolesTableModel::GlyphRolesTableModel(SynGlyphX::DataTransformModel* dataTransformModel, QObject *parent)
	: QAbstractTableModel(parent),
	m_isAnySelectedGlyphRoot(false),
	m_dataTransformModel(dataTransformModel)
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
	m_propertyHeaders.push_back(tr("URL"));
	m_propertyHeaders.push_back(tr("Rotation Rate X"));
	m_propertyHeaders.push_back(tr("Rotation Rate Y"));
	m_propertyHeaders.push_back(tr("Rotation Rate Z"));
	m_propertyHeaders.push_back(tr("Virtual Topology Type"));
	m_propertyHeaders.push_back(tr("Geometry Shape"));
	m_propertyHeaders.push_back(tr("Geometry Surface"));
	m_propertyHeaders.push_back(tr("Torus Ratio"));
	
	m_columnHeaders.push_back(tr("Property"));
	m_columnHeaders.push_back(tr("Default(s)"));
	m_columnHeaders.push_back(tr("Function"));
	m_columnHeaders.push_back(tr("Input"));

	m_headerFont.setBold(true);
}

GlyphRolesTableModel::~GlyphRolesTableModel()
{

}

int GlyphRolesTableModel::columnCount(const QModelIndex& parent) const {

	if (parent.isValid()) {

		return 0;
	}
	else {

		return m_columnHeaders.size();
	}
}

int	GlyphRolesTableModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		return m_propertyHeaders.size();
	}
	else {

		return 0;
	}
}

const SynGlyphX::InputField GlyphRolesTableModel::GetInputField(SynGlyphX::InputField::HashID fieldID) const {

	if ((fieldID == 0) || (m_selectedDataTransformModelIndexes.isEmpty())) {

		return SynGlyphX::InputField();
	}

	const SynGlyphX::DataMappingGlyphGraph::InputFieldMap& fieldMap = m_dataTransformModel->GetInputFieldsForTree(m_selectedDataTransformModelIndexes.last());
	if (fieldMap.count(fieldID) > 0) {

		return fieldMap.at(fieldID);
	}
	else {

		throw std::invalid_argument("FieldID is not listed in input field list");
	}
}

QVariant GlyphRolesTableModel::data(const QModelIndex& index, int role) const {

	if (index.isValid()) {

		if ((role == Qt::DisplayRole) && (index.column() == s_propertyNameColumn)) {

			return GetPropertyHeader(index);
		}
		else if (role == Qt::EditRole) {

			return GetEditData(index);
		}
		else if ((role == Qt::FontRole) && (index.column() == s_propertyNameColumn)) {

			return m_headerFont;
		}
		else if ((role == Qt::BackgroundRole) && (index.column() != s_propertyNameColumn) && (!IsClear())) {
			
			if ((index.column() == s_mappedFieldColumn) && (!m_associatedInputTable.is_initialized())) {

				return QBrush(Qt::red);
			}
			if (IsDataAtIndexDifferent(index)) {

				return QBrush(Qt::yellow);
			}
		}
	}
	
	return QVariant();
}

QString GlyphRolesTableModel::GetPropertyHeader(const QModelIndex& index) const {

	return m_propertyHeaders[index.row()];
}

bool GlyphRolesTableModel::IsDataAtIndexDifferent(const QModelIndex& index) const {

	if (m_selectedDataTransformModelIndexes.count() > 1) {

		PropertyType propertyType = GetFieldType(index.row());
		QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndexes.first(), index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX);
		if (propertyType == PropertyType::Color) {

			return IsDataAtIndexDifferentFromGivenData<SynGlyphX::ColorMappingProperty>(prop, index);
		}
		else if (propertyType == PropertyType::Text) {

			return IsDataAtIndexDifferentFromGivenData<SynGlyphX::TextMappingProperty>(prop, index);
		}
		else if (propertyType == PropertyType::GeometryShape) {

			return IsDataAtIndexDifferentFromGivenData<SynGlyphX::GeometryShapeMappingProperty>(prop, index);
		}
		else if (propertyType == PropertyType::VirtualTopology) {

			return IsDataAtIndexDifferentFromGivenData<SynGlyphX::VirtualTopologyMappingProperty>(prop, index);
		}
		else if (propertyType == PropertyType::Numeric) {

			return IsDataAtIndexDifferentFromGivenData<SynGlyphX::NumericMappingProperty>(prop, index);
		}
		else if (index.column() == s_valueColumn) {

			//Non mappable fields
			if (index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX == SynGlyphX::DataTransformModel::PropertyRole::GeometrySurface) {

				return IsDataAtIndexDifferentFromNonMappableProperties<SynGlyphX::GlyphGeometryInfo::Surface>(prop, index);
			}
			else {

				//torus ratio
				return IsDataAtIndexDifferentFromNonMappableProperties<double>(prop, index);
			}
		}
	}

	return false;
}

template<typename MappingPropertyType>
bool GlyphRolesTableModel::IsDataAtIndexDifferentFromGivenData(const QVariant& propVariant, const QModelIndex& index) const {

	MappingPropertyType testingProp = propVariant.value<MappingPropertyType>();
	for (const QPersistentModelIndex& selectedIndex : m_selectedDataTransformModelIndexes) {

		QVariant selectedProp = m_dataTransformModel->data(selectedIndex, index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX);
		MappingPropertyType selectedTypeProp = selectedProp.value<MappingPropertyType>();
		if (index.column() == s_mappingDataColumn) {

			if (!selectedTypeProp.GetMappingFunctionData()->IsSame(*testingProp.GetMappingFunctionData())) {

				return true;
			}
		}
		else if (index.column() == s_mappedFieldColumn) {

			if (selectedTypeProp.GetBinding() != testingProp.GetBinding()) {

				return true;
			}
		}
		else {

			if (selectedTypeProp.GetValue() != testingProp.GetValue()) {

				return true;
			}
		}
	}

	return false;
}

template<typename ValueType>
bool GlyphRolesTableModel::IsDataAtIndexDifferentFromNonMappableProperties(const QVariant& valueVariant, const QModelIndex& index) const {

	if (index.column() == s_valueColumn) {

		ValueType testingValue = valueVariant.value<ValueType>();
		for (const QPersistentModelIndex& selectedIndex : m_selectedDataTransformModelIndexes) {

			QVariant selectedValue = m_dataTransformModel->data(selectedIndex, index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX);
			if (testingValue != selectedValue.value<ValueType>()) {

				return true;
			}
		}
	}

	return false;
}

QVariant GlyphRolesTableModel::GetEditData(const QModelIndex& index) const {

	if ((index.column() != s_propertyNameColumn) && (!m_selectedDataTransformModelIndexes.isEmpty())) {

		QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndexes.last(), index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX);
		PropertyType propertyType = GetFieldType(index.row());
		if (propertyType == PropertyType::Color) {

			return GetEditDataForType<SynGlyphX::ColorMappingProperty>(prop, index);
		}
		else if (propertyType == PropertyType::Text) {

			return GetEditDataForTextMappingProperty(prop, index);
		}
		else if (propertyType == PropertyType::GeometryShape) {

			return GetEditDataForType<SynGlyphX::GeometryShapeMappingProperty>(prop, index);
		}
		else if (propertyType == PropertyType::VirtualTopology) {

			return GetEditDataForType<SynGlyphX::VirtualTopologyMappingProperty>(prop, index);
		}
		else if (propertyType == PropertyType::Numeric) {

			return GetEditDataForType<SynGlyphX::NumericMappingProperty>(prop, index);
		}
		else {

			if (index.column() == s_valueColumn) {

				return prop;
			}
		}
	}

	return QVariant();
}

QVariant GlyphRolesTableModel::GetEditDataForTextMappingProperty(const QVariant& propVariant, const QModelIndex& index) const {

	SynGlyphX::TextMappingProperty mappingProperty = propVariant.value<SynGlyphX::TextMappingProperty>();
	if (index.column() == s_mappingDataColumn) {

		return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(mappingProperty.GetMappingFunctionData()->GetFunction()));
	}
	else if (index.column() == s_mappedFieldColumn) {

		return QVariant::fromValue(GetInputField(mappingProperty.GetBinding().GetInputFieldID()));
	}
	else {

		return QString::fromStdWString(mappingProperty.GetValue());
	}
}

template<typename MappingPropertyType>
QVariant GlyphRolesTableModel::GetEditDataForType(const QVariant& propVariant, const QModelIndex& index) const {

	MappingPropertyType mappingProperty = propVariant.value<MappingPropertyType>();
	if (index.column() == s_mappingDataColumn) {

		return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(mappingProperty.GetMappingFunctionData()->GetFunction()));
	}
	else if (index.column() == s_mappedFieldColumn) {

		return QVariant::fromValue(GetInputField(mappingProperty.GetBinding().GetInputFieldID()));
	}
	else {

		return QVariant::fromValue(mappingProperty.GetValue());
	}
}

void GlyphRolesTableModel::SetSelectedGlyphTreeIndexes(const QModelIndexList& indexList) {

	m_isAnySelectedGlyphRoot = false;
	bool doesListHaveValidIndexes = !indexList.isEmpty();
	for (const QModelIndex& index : indexList) {

		if (!index.isValid()) {

			doesListHaveValidIndexes = false;
			break;
		}
	}

	if (doesListHaveValidIndexes) {

		DisconnectAllSignalsFromSourceModel();
		m_selectedDataTransformModelIndexes.clear();
		for (const QModelIndex& index : indexList) {

			m_selectedDataTransformModelIndexes.push_back(QPersistentModelIndex(index));
			if (!index.parent().isValid()) {

				m_isAnySelectedGlyphRoot = true;
			}
		}
		DetermineAssociatedInputTable();
		OnAllDataUpdated();

		m_sourceModelConnections.push_back(QObject::connect(m_dataTransformModel, &SynGlyphX::DataTransformModel::dataChanged, this, &GlyphRolesTableModel::OnSourceModelDataUpdated));
		m_sourceModelConnections.push_back(QObject::connect(m_dataTransformModel, &SynGlyphX::DataTransformModel::modelReset, this, [&, this](){ Clear(); }));
	}
	else {

		Clear();
	}
}

void GlyphRolesTableModel::DetermineAssociatedInputTable() {

	m_associatedInputTable.reset();
	SynGlyphX::InputTable inputTable;
	for (const QPersistentModelIndex& index : m_selectedDataTransformModelIndexes) {

		const SynGlyphX::DataMappingGlyphGraph::InputFieldMap& inputFields = m_dataTransformModel->GetInputFieldsForTree(index);
		if (!inputFields.empty()) {

			if (inputTable.IsValid()) {

				if (inputTable != inputFields.begin()->second) {

					return;
				}
			}
			else {

				inputTable = inputFields.begin()->second;
			}
		}
	}
	m_associatedInputTable.reset(inputTable);
}

void GlyphRolesTableModel::DisconnectAllSignalsFromSourceModel() {

	for (auto& connection : m_sourceModelConnections) {

		QObject::disconnect(connection);
	}
	m_sourceModelConnections.clear();
}

void GlyphRolesTableModel::Clear() {

	DisconnectAllSignalsFromSourceModel();

	m_selectedDataTransformModelIndexes.clear();
	OnAllDataUpdated();
}

bool GlyphRolesTableModel::IsClear() const {

	return m_selectedDataTransformModelIndexes.isEmpty();
}

bool GlyphRolesTableModel::IsInputFieldCompatible(const SynGlyphX::InputField& inputField) const {

	if (!IsClear()) {

		if (m_associatedInputTable.is_initialized()) {

			if ((!m_associatedInputTable->IsValid()) || (m_associatedInputTable.get().operator==(inputField))) {

				return true;
			}
		}
	}

	return false;
}
/*
bool GlyphRolesTableModel::DoesGlyphHaveAssociatedDatasoruceTable() const {

	const SynGlyphX::DataMappingGlyphGraph::InputFieldMap& inputFields = m_dataTransformModel->GetInputFieldsForTree(m_selectedSynGlyphX::DataTransformModelIndex);
	return (!inputFields.empty());
}

const SynGlyphX::DatasourceTable& GlyphRolesTableModel::GetAssociatedDatasourceTable() const {

	if (!DoesGlyphHaveAssociatedDatasoruceTable()) {

		throw std::runtime_error("Glyph tree associated with index has no datasource table associated with it");
	}

	const SynGlyphX::InputTable& inputTable = m_dataTransformModel->GetInputFieldsForTree(m_selectedSynGlyphX::DataTransformModelIndex).begin()->second;
}*/

Qt::ItemFlags GlyphRolesTableModel::flags(const QModelIndex & index) const {

	return QAbstractTableModel::flags(index); // | Qt::ItemIsEditable;
}

template<typename MappingPropertyType>
bool GlyphRolesTableModel::SetMappingFunctionEditData(const QVariant& propVariant, PropertyType propertyType, const QModelIndex& index) const {

	int sourceDataRole = index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX;
	SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(propVariant.toString().toStdWString());

	for (const QPersistentModelIndex& selectedIndex : m_selectedDataTransformModelIndexes) {

		QVariant prop = m_dataTransformModel->data(selectedIndex, sourceDataRole);
		SynGlyphX::DataMappingProperty<MappingPropertyType> mappingProperty = prop.value<SynGlyphX::DataMappingProperty<MappingPropertyType>>();
		if (function != mappingProperty.GetMappingFunctionData()->GetFunction()) {

			mappingProperty.SetMappingFunctionData(CreateNewMappingFunction(function, propertyType));
			if (!m_dataTransformModel->setData(selectedIndex, QVariant::fromValue(mappingProperty), sourceDataRole)) {

				return false;
			}
		}
	}

	return true;
}

template<typename MappingPropertyType>
bool GlyphRolesTableModel::SetSingleValueEditData(const QVariant& propVariant, const QModelIndex& index) const {

	int sourceDataRole = index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX;
	MappingPropertyType newValue = propVariant.value<MappingPropertyType>();
	for (const QPersistentModelIndex& selectedIndex : m_selectedDataTransformModelIndexes) {

		QVariant prop = m_dataTransformModel->data(selectedIndex, sourceDataRole);
		SynGlyphX::DataMappingProperty<MappingPropertyType> mappingProperty = prop.value<SynGlyphX::DataMappingProperty<MappingPropertyType>>();
		if (newValue != mappingProperty.GetValue()) {

			mappingProperty.GetValue() = newValue;
			if (!m_dataTransformModel->setData(selectedIndex, QVariant::fromValue(mappingProperty), sourceDataRole)) {

				return false;
			}
		}
	}

	return true;
}

template<typename MappingPropertyType>
bool GlyphRolesTableModel::SetMinMaxValueEditData(const QVariant& propVariant, const QModelIndex& index) const {

	int sourceDataRole = index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX;
	MappingPropertyType newValue = propVariant.value<MappingPropertyType>();

	QVariant lastProp = m_dataTransformModel->data(m_selectedDataTransformModelIndexes.last(), sourceDataRole);
	SynGlyphX::DataMappingProperty<MappingPropertyType> lastMappingProperty = lastProp.value<SynGlyphX::DataMappingProperty<MappingPropertyType>>();

	bool updateMin = (newValue.GetMin() != lastMappingProperty.GetValue().GetMin());
	bool updateMax = (newValue.GetMax() != lastMappingProperty.GetValue().GetMax());

	for (const QPersistentModelIndex& selectedIndex : m_selectedDataTransformModelIndexes) {

		QVariant prop = m_dataTransformModel->data(selectedIndex, sourceDataRole);
		SynGlyphX::DataMappingProperty<MappingPropertyType> mappingProperty = prop.value<SynGlyphX::DataMappingProperty<MappingPropertyType>>();
		if (newValue != mappingProperty.GetValue()) {

			if (updateMin && updateMax) {

				mappingProperty.GetValue() = newValue;
			}
			else if (updateMin) {

				mappingProperty.GetValue().SetMinMax(newValue.GetMin(), mappingProperty.GetValue().GetMax());
			}
			else if (updateMax) {

				mappingProperty.GetValue().SetMinMax(mappingProperty.GetValue().GetMin(), newValue.GetMax());
			}
			
			if (!m_dataTransformModel->setData(selectedIndex, QVariant::fromValue(mappingProperty), sourceDataRole)) {

				return false;
			}
		}
	}

	return true;
}

bool GlyphRolesTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if ((!m_selectedDataTransformModelIndexes.isEmpty()) && (index.isValid()) && (role == Qt::EditRole) && (index.column() != s_propertyNameColumn)) {

		if (index.column() == s_mappedFieldColumn) {

			SynGlyphX::DataMappingGlyph::MappableField mappableField = static_cast<SynGlyphX::DataMappingGlyph::MappableField>(index.row());
			SynGlyphX::InputField inputField = value.value<SynGlyphX::InputField>();
			if (inputField.IsValid()) {

				for (const QPersistentModelIndex& index : m_selectedDataTransformModelIndexes) {

					m_dataTransformModel->SetInputField(index, mappableField, inputField);
				}
			}
			else {

				for (const QPersistentModelIndex& index : m_selectedDataTransformModelIndexes) {

					m_dataTransformModel->ClearInputBinding(index, mappableField);
				}
			}
			DetermineAssociatedInputTable();
		}
		else {

			PropertyType propertyType = GetFieldType(index.row());
			if (propertyType == PropertyType::Color) {

				if (index.column() == s_mappingDataColumn) {

					return SetMappingFunctionEditData<SynGlyphX::ColorMinDiff>(value, propertyType, index);
				}
				else {

					return SetMinMaxValueEditData<SynGlyphX::ColorMinDiff>(value, index);
				}
			}
			else if (propertyType == PropertyType::GeometryShape) {

				if (index.column() == s_mappingDataColumn) {

					return SetMappingFunctionEditData<SynGlyphX::GlyphGeometryInfo::Shape>(value, propertyType, index);
				}
				else {

					return SetSingleValueEditData<SynGlyphX::GlyphGeometryInfo::Shape>(value, index);
				}
			}
			else if (propertyType == PropertyType::VirtualTopology) {

				if (index.column() == s_mappingDataColumn) {

					return SetMappingFunctionEditData<SynGlyphX::VirtualTopologyInfo::Type>(value, propertyType, index);
				}
				else {

					return SetSingleValueEditData<SynGlyphX::VirtualTopologyInfo::Type>(value, index);
				}
			}
			else if (propertyType == PropertyType::Numeric) {

				if (index.column() == s_mappingDataColumn) {

					return SetMappingFunctionEditData<SynGlyphX::DoubleMinDiff>(value, propertyType, index);
				}
				else {

					return SetMinMaxValueEditData<SynGlyphX::DoubleMinDiff>(value, index);
				}
			}
			else if (propertyType == PropertyType::NonMappable) {

				if (index.column() == s_valueColumn) {

					int sourceDataRole = index.row() + SynGlyphX::DataTransformModel::PropertyRole::PositionX;
					for (const QPersistentModelIndex& index : m_selectedDataTransformModelIndexes) {

						if (!(m_dataTransformModel->setData(index, value, sourceDataRole))) {

							return false;
						}
					}
					return true;
				}
			}
		}
	}

	return false;
}

QVariant GlyphRolesTableModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (role == Qt::DisplayRole) {

		if (orientation == Qt::Horizontal) {

			if (section < columnCount()) {
				return m_columnHeaders[section];
			}
		}
	}
	else if (role == Qt::FontRole) {

		return m_headerFont;
	}

	return QVariant();
}

SynGlyphX::DataTransformMapping::ConstSharedPtr GlyphRolesTableModel::GetDataTransformMapping() const {

	return m_dataTransformModel->GetDataMapping();
}

bool GlyphRolesTableModel::IsAnyCurrentGlyphRoot() const {

	return m_isAnySelectedGlyphRoot;
}

void GlyphRolesTableModel::ClearInputBindings() {

	for (const QPersistentModelIndex& index : m_selectedDataTransformModelIndexes) {

		m_dataTransformModel->ClearAllInputBindings(index);
	}
	
	emit dataChanged(index(0, columnCount() - 1), index(rowCount() - 1, columnCount() - 1));
}

GlyphRolesTableModel::PropertyType GlyphRolesTableModel::GetFieldType(int row) const {

	if ((row == SynGlyphX::DataMappingGlyph::MappableField::Tag) || (row == SynGlyphX::DataMappingGlyph::MappableField::Description) || (row == SynGlyphX::DataMappingGlyph::MappableField::URL)) {

		return PropertyType::Text;
	}
	else if (row == SynGlyphX::DataMappingGlyph::MappableField::Color) {

		return PropertyType::Color;
	}
	else if (row == SynGlyphX::DataMappingGlyph::MappableField::VirtualTopology) {

		return PropertyType::VirtualTopology;
	}
	else if (row == SynGlyphX::DataMappingGlyph::MappableField::GeometryShape) {

		return PropertyType::GeometryShape;
	}
	else if (row > SynGlyphX::DataMappingGlyph::MappableField::GeometryShape) {

		return PropertyType::NonMappable;
	}
	else {

		return PropertyType::Numeric;
	}
}

SynGlyphX::MappingFunctionData::SharedPtr GlyphRolesTableModel::CreateNewMappingFunction(SynGlyphX::MappingFunctionData::Function function, PropertyType type) const {

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

SynGlyphX::MappingFunctionData::ConstSharedPtr GlyphRolesTableModel::GetMappingFunction(int row) const {

	QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndexes.last(), row + SynGlyphX::DataTransformModel::PropertyRole::PositionX);
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

void GlyphRolesTableModel::SetMappingFunction(int row, SynGlyphX::MappingFunctionData::SharedPtr mappingFunction) {

	int sourceDataRole = row + SynGlyphX::DataTransformModel::PropertyRole::PositionX;
	QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndexes.last(), sourceDataRole);
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

		for (const QPersistentModelIndex& index : m_selectedDataTransformModelIndexes) {

			m_dataTransformModel->setData(index, newProp, sourceDataRole);
		}
	}
}

void GlyphRolesTableModel::OnAllDataUpdated() {

	emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void GlyphRolesTableModel::OnSourceModelDataUpdated(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {

	if (IsAnySelectedIndexWithinIndexes(topLeft, bottomRight)) {

		DetermineAssociatedInputTable();
		if (roles.empty()) {

			OnAllDataUpdated();
		}
		else {

			Q_FOREACH(int role, roles) {

				if (role >= SynGlyphX::DataTransformModel::PropertyRole::PositionX) {

					int row = role - SynGlyphX::DataTransformModel::PropertyRole::PositionX;
					emit dataChanged(index(row, 0), index(row, columnCount() - 1));
				}
			}
		}
	}
}

bool GlyphRolesTableModel::IsAnySelectedIndexWithinIndexes(const QModelIndex& topLeft, const QModelIndex& bottomRight) const {

	if (m_selectedDataTransformModelIndexes.isEmpty()) {

		return false;
	}

	for (const QPersistentModelIndex& selectedIndex : m_selectedDataTransformModelIndexes) {

		QModelIndex index = selectedIndex;
		if (topLeft.parent().isValid()) {

			while (index.parent().isValid() && (index.parent() != topLeft.parent())) {

				index = index.parent();
			}
			if (!index.parent().isValid()) {

				return false;
			}
		}
		else {

			while (index.parent().isValid()) {

				index = index.parent();
			}
		}
		
		if ((index.row() >= topLeft.row()) && (index.row() <= bottomRight.row())) {

			return true;
		}
	}
	
	return false;
}

SynGlyphX::DataTransformModel* GlyphRolesTableModel::GetSourceModel() const {

	return m_dataTransformModel;
}