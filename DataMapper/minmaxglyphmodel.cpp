#include "minmaxglyphmodel.h"
#include <QtGui/QColor>
#include "sourcedatamanager.h"
#include <QtWidgets/QMessageBox>
#include <stack>
#include "interpolationmappingfunction.h"
#include "valuemappingfunction.h"

MinMaxGlyphModel::MinMaxGlyphModel(DataTransformModel* dataTransformModel, QObject *parent)
	: QAbstractTableModel(parent),
	m_dataTransformModel(dataTransformModel),
	//m_glyphTree(nullptr),
	m_glyphTreeID(boost::uuids::nil_uuid()),
	m_selectedDataTransformModelIndex(QModelIndex())
{
	setHeaderData(0, Qt::Vertical, tr("Position X"));
	setHeaderData(1, Qt::Vertical, tr("Position Y"));
	setHeaderData(2, Qt::Vertical, tr("Position Z"));
	setHeaderData(3, Qt::Vertical, tr("Rotation X"));
	setHeaderData(4, Qt::Vertical, tr("Rotation Y"));
	setHeaderData(5, Qt::Vertical, tr("Rotation Z"));
	setHeaderData(6, Qt::Vertical, tr("Scale X"));
	setHeaderData(7, Qt::Vertical, tr("Scale Y"));
	setHeaderData(8, Qt::Vertical, tr("Scale Z"));
	setHeaderData(9, Qt::Vertical, tr("Color"));
	setHeaderData(10, Qt::Vertical, tr("Transparency"));
	setHeaderData(11, Qt::Vertical, tr("Tag"));
	setHeaderData(12, Qt::Vertical, tr("Description"));
	setHeaderData(13, Qt::Vertical, tr("Rotation Rate X"));
	setHeaderData(14, Qt::Vertical, tr("Rotation Rate Y"));
	setHeaderData(15, Qt::Vertical, tr("Rotation Rate Z"));
	setHeaderData(16, Qt::Vertical, tr("Geometry Shape"));
	setHeaderData(17, Qt::Vertical, tr("Geometry Surface"));
	setHeaderData(18, Qt::Vertical, tr("Torus Ratio"));
	setHeaderData(19, Qt::Vertical, tr("Virtual Topology Type"));

	setHeaderData(0, Qt::Horizontal, tr("Value"));
	setHeaderData(1, Qt::Horizontal, tr("Function"));
	setHeaderData(2, Qt::Horizontal, tr("Input"));
}

MinMaxGlyphModel::~MinMaxGlyphModel()
{

}

int MinMaxGlyphModel::columnCount(const QModelIndex& parent) const {

	if (parent.isValid()) {

		return 0;
	}
	else {

		return 3;
	}
}

const SynGlyphX::InputField& MinMaxGlyphModel::GetInputField(SynGlyphX::InputField::HashID fieldID) const {

	return m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().at(m_glyphTreeID)->GetInputFields().at(fieldID);
}

QVariant MinMaxGlyphModel::data(const QModelIndex& index, int role) const {

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

				return QVariant::fromValue(shapeProp.GetValue());
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

				return QVariant::fromValue(topologyProp.GetValue());
			}
		}
		else {

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
	}

	return QVariant();
}

int	MinMaxGlyphModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		return DataTransformModel::PropertyRole::VirtualTopology - DataTransformModel::DataTypeRole;
	}
	else {

		return 0;
	}
}

void MinMaxGlyphModel::SetMinMaxGlyph(const QModelIndex& index) {

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
	}
	else {

		Clear();
	}
}

void MinMaxGlyphModel::Clear() {

	beginResetModel();
	//m_glyph = SynGlyphX::DataMappingGlyphGraph::const_iterator();
	//m_glyphTree = nullptr;
	m_glyphTreeID = boost::uuids::nil_uuid();
	m_selectedDataTransformModelIndex = QModelIndex();
	endResetModel();
}

bool MinMaxGlyphModel::IsClear() const {

	return (!m_selectedDataTransformModelIndex.isValid());
}
/*
SynGlyphX::NumericMappingProperty& MinMaxGlyphModel::GetGlyphProperty(SynGlyphX::DataMappingGlyph& glyph, int row) const {

	if (row == 0) {

		return glyph.GetPosition()[0];
	}
	else if (row == 1) {

		return glyph.GetPosition()[1];
	}
	else if (row == 2) {

		return glyph.GetPosition()[2];
	}
	else if (row == 3) {

		return glyph.GetRotation()[0];
	}
	else if (row == 4) {

		return glyph.GetRotation()[1];
	}
	else if (row == 5) {

		return glyph.GetRotation()[2];
	}
	else if (row == 6) {

		return glyph.GetScale()[0];
	}
	else if (row == 7) {

		return glyph.GetScale()[1];
	}
	else if (row == 8) {

		return glyph.GetScale()[2];
	}
	else if (row == 10) {

		return glyph.GetTransparency();
	}
	else if (row == 13) {

		return glyph.GetRotationRate()[0];
	}
	else if (row == 14) {

		return glyph.GetRotationRate()[1];
	}
	else if (row == 15) {

		return glyph.GetRotationRate()[2];
	}
}

QVariant MinMaxGlyphModel::GetDataByRow(SynGlyphX::DataMappingGlyph& glyph, const QModelIndex& index) const {

	int row = index.row();
	if ((row < m_propertyHeaders.size()) && (!IsTextField(row))) {

		if (IsColorField(row)) {

			SynGlyphX::ColorMappingProperty& colorProperty = glyph.GetColor();
			
			if (index.column() == 2) {

				return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(colorProperty.GetMappingFunctionData()->GetFunction()));
			}
			else {
			
				const std::pair<SynGlyphX::GlyphColor, SynGlyphX::GlyphColor>& colorMinMax = colorProperty.GetValue();
				if (index.column() == 0) {

					return QColor(colorMinMax.first[0], colorMinMax.first[1], colorMinMax.first[2]);
				}
				else {

					return QColor(colorMinMax.first[0] + colorMinMax.second[0], colorMinMax.first[1] + colorMinMax.second[1], colorMinMax.first[2] + colorMinMax.second[2]);
				}
			}
		}
		else {

			SynGlyphX::NumericMappingProperty& numericProperty = GetGlyphProperty(glyph, row);
			if (index.column() == 2) {

				return QString::fromStdWString(SynGlyphX::MappingFunctionData::s_functionNames.left.at(numericProperty.GetMappingFunctionData()->GetFunction()));
			}
			else if(index.column() == 0) {

				return numericProperty.GetValue().first;
			}
			else {

				return numericProperty.GetValue().first + numericProperty.GetValue().second;
			}
		}
	}

	return QVariant();
}
*/
Qt::ItemFlags MinMaxGlyphModel::flags(const QModelIndex & index) const {

	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool MinMaxGlyphModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if ((index.isValid()) && (role == Qt::EditRole)) { // && m_glyph.valid()) {

		if (index.column() == 2) {

			SynGlyphX::DataMappingGlyph::MappableField mappableField;
			if (index.row() >= DataTransformModel::VirtualTopology) {

				mappableField = static_cast<SynGlyphX::DataMappingGlyph::MappableField>(index.row() - 2);
			}
			else {

				mappableField = static_cast<SynGlyphX::DataMappingGlyph::MappableField>(index.row());
			}
			m_dataTransformModel->SetInputField(m_glyphTreeID, m_selectedDataTransformModelIndex, mappableField, value.value<SynGlyphX::InputField>());
		}
		else {

			int sourceDataRole = index.row() + DataTransformModel::PropertyRole::PositionX;
			QVariant newProp;
			QVariant prop = m_dataTransformModel->data(m_selectedDataTransformModelIndex, sourceDataRole);
			PropertyType propertyType = GetFieldType(index.row());
			if (propertyType == PropertyType::Color) {

				SynGlyphX::ColorMappingProperty colorProp = prop.value<SynGlyphX::ColorMappingProperty>();
				if (index.column() == 1) {

					SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
					if (function != colorProp.GetMappingFunctionData()->GetFunction()) {

						colorProp.SetMappingFunctionData(CreateNewMappingFunction(function, PropertyType::Color));
					}
				}
				else {

					colorProp.GetValue() = value.value<SynGlyphX::ColorMinDiff>();
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
					}
				}
				else {

					shapeProp.GetValue() = value.value<SynGlyphX::GlyphGeometryInfo::Shape>();
				}
			}
			else if (propertyType == PropertyType::VirtualTopology) {

				SynGlyphX::VirtualTopologyMappingProperty topologyProp = prop.value<SynGlyphX::VirtualTopologyMappingProperty>();
				if (index.column() == 1) {

					SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
					if (function != topologyProp.GetMappingFunctionData()->GetFunction()) {

						topologyProp.SetMappingFunctionData(CreateNewMappingFunction(function, PropertyType::VirtualTopology));
					}
				}
				else {

					topologyProp.GetValue() = value.value<SynGlyphX::VirtualTopologyInfo::Type>();
				}
			}
			else {

				SynGlyphX::NumericMappingProperty numericProp = prop.value<SynGlyphX::NumericMappingProperty>();
				if (index.column() == 1) {

					SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
					if (function != numericProp.GetMappingFunctionData()->GetFunction()) {

						numericProp.SetMappingFunctionData(CreateNewMappingFunction(function, PropertyType::Numeric));
					}
				}
				else {

					numericProp.GetValue() = value.value<SynGlyphX::DoubleMinDiff>();
				}
			}

			if (newProp.isValid()) {

				if (m_dataTransformModel->setData(m_selectedDataTransformModelIndex, newProp, sourceDataRole)) {

					emit dataChanged(index, index);
					return true;
				}
			}
		}
	}

	return false;
}
/*
bool MinMaxGlyphModel::SetDataByRow(SynGlyphX::DataMappingGlyph& glyph, const QVariant& value, const QModelIndex& index) {

	int row = index.row();
	if ((row < m_propertyHeaders.size()) && (!IsTextField(row))) {

		if (IsColorField(row)) {

			QColor color = value.value < QColor >();
			std::pair<SynGlyphX::GlyphColor, SynGlyphX::GlyphColor>& colorMinMax = glyph.GetColor().GetValue();
			if (index.column() == 2) {

				SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
				if (function != glyph.GetColor().GetMappingFunctionData()->GetFunction()) {

					glyph.GetColor().SetMappingFunctionData(CreateNewMappingFunction(function, true));
				}
			}
			else if (index.column() == 0) {

				colorMinMax.second.Set(0, colorMinMax.second[0] - (color.red() - colorMinMax.first[0]));
				colorMinMax.second.Set(1, colorMinMax.second[1] - (color.green() - colorMinMax.first[1]));
				colorMinMax.second.Set(2, colorMinMax.second[2] - (color.blue() - colorMinMax.first[2]));
				colorMinMax.first.Set(color.red(), color.green(), color.blue());
			}
			else {

				colorMinMax.second.Set(0, color.red() - colorMinMax.first[0]);
				colorMinMax.second.Set(1, color.green() - colorMinMax.first[1]);
				colorMinMax.second.Set(2, color.blue() - colorMinMax.first[2]);
			}
		}
		else {

			double newValue = value.toDouble();
			SynGlyphX::NumericMappingProperty& prop = GetGlyphProperty(glyph, row);
			if (index.column() == 2) {

				SynGlyphX::MappingFunctionData::Function function = SynGlyphX::MappingFunctionData::s_functionNames.right.at(value.toString().toStdWString());
				if (function != prop.GetMappingFunctionData()->GetFunction()) {

					prop.SetMappingFunctionData(CreateNewMappingFunction(function, false));
				}
			}
			else if (index.column() == 0) {

				prop.GetValue().second -= (newValue - prop.GetValue().first);
				prop.GetValue().first = newValue;
			}
			else {

				prop.GetValue().second = (newValue - prop.GetValue().first);
			}
		}

		return true;
	}

	return false;
}

QVariant MinMaxGlyphModel::headerData(int section, Qt::Orientation orientation, int role) const {

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
}*/

SynGlyphX::DataTransformMapping::ConstSharedPtr MinMaxGlyphModel::GetDataTransformMapping() const {

	return m_dataTransformModel->GetDataMapping();
}

bool MinMaxGlyphModel::IsCurrentGlyphRoot() const {

	/*if (m_glyphTree == nullptr) {

		return false;
	}

	return (m_glyphTree->root() == m_glyph.constify());*/

	return (!m_selectedDataTransformModelIndex.parent().isValid());
}

void MinMaxGlyphModel::ClearInputBindings() {

	for (int i = 0; i < rowCount(); ++i) {

		m_dataTransformModel->ClearInputBinding(m_glyphTreeID, m_selectedDataTransformModelIndex, static_cast<SynGlyphX::DataMappingGlyph::MappableField>(i));
	}
	emit dataChanged(index(0, columnCount() - 1), index(rowCount() - 1, columnCount() - 1));
}

MinMaxGlyphModel::PropertyType MinMaxGlyphModel::GetFieldType(int row) const {

	if ((row == 11) || (row == 12)) {

		return PropertyType::Text;
	}
	else if (row == 9) {

		return PropertyType::Color;
	}
	else if (row == 16) {

		return PropertyType::GeometryShape;
	}
	else if (row == 19) {

		return PropertyType::VirtualTopology;
	}
	else {

		return PropertyType::Numeric;
	}
}

SynGlyphX::MappingFunctionData::SharedPtr MinMaxGlyphModel::CreateNewMappingFunction(SynGlyphX::MappingFunctionData::Function function, PropertyType type) const {

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

			return std::make_shared<SynGlyphX::Numeric2VirtualTopologyMappingData>();
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Text2Value) {

			return std::make_shared<SynGlyphX::Text2VirtualTopologyMappingData>();
		}
		else if (function == SynGlyphX::MappingFunctionData::Function::Range2Value) {

			return std::make_shared<SynGlyphX::Range2VirtualTopologyMappingData>();
		}
	}
	else {

		return std::make_shared<SynGlyphX::MappingFunctionData>();
	}
}
/*
SynGlyphX::MappingFunctionData::ConstSharedPtr MinMaxGlyphModel::GetMappingFunction(int row) const {

	if ((row >= m_propertyHeaders.size()) || (row < 0)) {

		throw std::invalid_argument("Can't get mapping function when row is less than zero or greater than number of properties");
	}
	if (IsTextField(row)) {

		throw std::invalid_argument("Mapping function doesn't apply to text properties");
	}

	if (IsColorField(row)) {

		return m_glyph->GetColor().GetMappingFunctionData();
	}
	else {

		return GetGlyphProperty(*m_glyph.deconstify(), row).GetMappingFunctionData();
	}
}

void MinMaxGlyphModel::SetMappingFunction(int row, SynGlyphX::MappingFunctionData::SharedPtr mappingFunction) {

	if ((row >= m_propertyHeaders.size()) || (row < 0)) {

		throw std::invalid_argument("Can't set mapping function when row is less than zero or greater than number of properties");
	}
	if (IsTextField(row)) {

		throw std::invalid_argument("Mapping function doesn't apply to text properties");
	}

	if (IsColorField(row)) {

		m_glyph.deconstify()->GetColor().SetMappingFunctionData(mappingFunction);
	}
	else {

		return GetGlyphProperty(*m_glyph.deconstify(), row).SetMappingFunctionData(mappingFunction);
	}
}

const SynGlyphX::DataMappingGlyphGeometry& MinMaxGlyphModel::GetGlyphGeometry() const {

	return m_dataTransformModel->GetGlyph(m_selectedDataTransformModelIndex).GetStructure();
}

const SynGlyphX::DataMappingVirtualTopology& MinMaxGlyphModel::GetVirtualTopology() const {

	return m_dataTransformModel->GetGlyph(m_selectedDataTransformModelIndex).GetVirtualTopology();
}

void MinMaxGlyphModel::SetGlyphGeometryAndVirtualTopology(const SynGlyphX::GlyphGeometry& structure, const SynGlyphX::VirtualTopology& virtualTopology) {

	m_dataTransformModel->UpdateGlyphGeometry(m_selectedDataTransformModelIndex, structure);
	m_dataTransformModel->UpdateVirtualTopology(m_selectedDataTransformModelIndex, virtualTopology);
}*/