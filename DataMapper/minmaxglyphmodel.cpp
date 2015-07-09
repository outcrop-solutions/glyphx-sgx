#include "minmaxglyphmodel.h"
#include <QtGui/QColor>
#include "sourcedatamanager.h"
#include <QtWidgets/QMessageBox>
#include <stack>
#include "interpolationmappingfunction.h"
#include "valuemappingfunction.h"
#include "colorminmaxwidget.h"
#include "doubleminmaxwidget.h"
#include "intminmaxwidget.h"

MinMaxGlyphModel::MinMaxGlyphModel(DataTransformModel* dataTransformModel, QObject *parent)
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
	
	m_columnHeaders.push_back(tr("Value"));
	m_columnHeaders.push_back(tr("Function"));
	m_columnHeaders.push_back(tr("Input"));
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

const SynGlyphX::InputField MinMaxGlyphModel::GetInputField(SynGlyphX::InputField::HashID fieldID) const {

	if (fieldID == 0) {

		return SynGlyphX::InputField();
	}
	else {

		return m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().at(m_glyphTreeID)->GetInputFields().at(fieldID);
	}
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

int	MinMaxGlyphModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		return DataTransformModel::PropertyRole::GeometryTorusRatio - DataTransformModel::DataTypeRole;
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

			SynGlyphX::DataMappingGlyph::MappableField mappableField = static_cast<SynGlyphX::DataMappingGlyph::MappableField>(index.row());
			m_dataTransformModel->SetInputField(m_glyphTreeID, m_selectedDataTransformModelIndex, mappableField, value.value<SynGlyphX::InputField>());
		}
		else {

			int sourceDataRole = index.row() + DataTransformModel::PropertyRole::PositionX;
			QVariant newProp;

			if (sourceDataRole >= DataTransformModel::PropertyRole::GeometrySurface) {

				newProp = value;
			}
			else {

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

					newProp.setValue(colorProp);
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

						if (value.type() == QVariant::Type::String) {

							shapeProp.GetValue() = SynGlyphX::GlyphGeometryInfo::s_shapeNames.right.at(value.toString().toStdWString());
						}
						else {

							shapeProp.GetValue() = value.value<SynGlyphX::GlyphGeometryInfo::Shape>();
						}
					}

					newProp.setValue(shapeProp);
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

						if (value.type() == QVariant::Type::String) {

							topologyProp.GetValue() = SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.right.at(value.toString().toStdWString());
						}
						else {

							topologyProp.GetValue() = value.value<SynGlyphX::VirtualTopologyInfo::Type>();
						}
					}

					newProp.setValue(topologyProp);
				}
				else if (propertyType == PropertyType::Numeric) {

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

					newProp.setValue(numericProp);
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
}*/

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
}

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
	
	//If nothing else return a none mapping function
	return std::make_shared<SynGlyphX::MappingFunctionData>();
}

SynGlyphX::MappingFunctionData::ConstSharedPtr MinMaxGlyphModel::GetMappingFunction(int row) const {

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

void MinMaxGlyphModel::SetMappingFunction(int row, SynGlyphX::MappingFunctionData::SharedPtr mappingFunction) {

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
/*
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