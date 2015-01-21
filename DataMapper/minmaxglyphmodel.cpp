#include "minmaxglyphmodel.h"
#include <QtGui/QColor>
#include "sourcedatamanager.h"
#include <QtWidgets/QMessageBox>
#include <stack>

MinMaxGlyphModel::MinMaxGlyphModel(DataTransformModel* dataTransformModel, QObject *parent)
	: QAbstractTableModel(parent),
	m_dataTransformModel(dataTransformModel),
	m_glyphTree(nullptr),
	m_selectedIndex(QModelIndex())
{
	m_propertyHeaders << tr("Position X")
		<< tr("Position Y")
		<< tr("Position Z")
		<< tr("Rotation X")
		<< tr("Rotation Y")
		<< tr("Rotation Z")
		<< tr("Scale X")
		<< tr("Scale Y")
		<< tr("Scale Z")
		<< tr("Color")
		<< tr("Transparency")
		<< tr("Tag")
		<< tr("Description")
		<< tr("Rotation Rate X")
		<< tr("Rotation Rate Y")
		<< tr("Rotation Rate Z");

	m_columnHeaders << tr("Min")
		<< tr("Max")
		<< tr("Input");
}

MinMaxGlyphModel::~MinMaxGlyphModel()
{

}

int MinMaxGlyphModel::columnCount(const QModelIndex& parent) const {

	if (parent.isValid()) {

		return 0;
	}
	else {

		return m_columnHeaders.size();
	}
}

QVariant MinMaxGlyphModel::data(const QModelIndex& index, int role) const {

	if ((role == Qt::EditRole) && index.isValid() && m_glyph.valid()) {

		if ((index.column() == 0) || (index.column() == 1)) {

			return GetDataByRow(const_cast<SynGlyphX::DataMappingGlyph&>(*m_glyph), index);
		}
		else if (index.column() == 2) {

			const SynGlyphX::InputBinding& binding = m_glyph->GetInputBinding(static_cast<SynGlyphX::DataMappingGlyph::MappableField>(index.row()));

			if (binding.IsBoundToInputField()) {

				const SynGlyphX::InputField& inputfield = m_glyphTree->GetInputFields().at(binding.GetInputFieldID());

				QVariant variant;
				variant.setValue(inputfield);
				return variant;
			}
		}
	}

	return QVariant();
}

int	MinMaxGlyphModel::rowCount(const QModelIndex& parent) const {

	if (!parent.isValid()) {

		return SynGlyphX::DataMappingGlyph::MappableField::MappableFieldSize;
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
		m_selectedIndex = index;
		m_glyphTreeID = glyphTree->first;
		m_glyphTree = glyphTree->second;
		SynGlyphX::DataMappingGlyphGraph::const_iterator iT = m_glyphTree->root();

		while (!childIndices.empty()) {

			iT = m_glyphTree->child(iT, childIndices.top());
			childIndices.pop();
		}
		m_glyph = iT;
		endResetModel();
	}
	else {

		Clear();
	}
}

void MinMaxGlyphModel::Clear() {

	beginResetModel();
	m_glyph = SynGlyphX::DataMappingGlyphGraph::const_iterator();
	m_glyphTree = nullptr;
	m_glyphTreeID = boost::uuids::nil_uuid();
	m_selectedIndex = QModelIndex();
	endResetModel();
}

bool MinMaxGlyphModel::IsClear() const {

	return (m_glyphTree == nullptr);
}

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

			const std::pair<SynGlyphX::GlyphColor, SynGlyphX::GlyphColor>& colorMinMax = glyph.GetColor().GetValue();
			if (index.column() == 0) {

				return QColor(colorMinMax.first[0], colorMinMax.first[1], colorMinMax.first[2]);
			}
			else {

				return QColor(colorMinMax.first[0] + colorMinMax.second[0], colorMinMax.first[1] + colorMinMax.second[1], colorMinMax.first[2] + colorMinMax.second[2]);
			}
		}
		else {

			SynGlyphX::NumericMappingProperty& prop = GetGlyphProperty(glyph, row);
			if (index.column() == 0) {

				return prop.GetValue().first;
			}
			else {

				return prop.GetValue().first + prop.GetValue().second;
			}
		}
	}

	return QVariant();
}

Qt::ItemFlags MinMaxGlyphModel::flags(const QModelIndex & index) const {

	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool MinMaxGlyphModel::setData(const QModelIndex& index, const QVariant& value, int role) {

	if (index.isValid() && m_glyph.valid()) {

		if (index.column() == 2) {

			try {

				SynGlyphX::InputField inputfield = value.value<SynGlyphX::InputField>();
				if (inputfield.IsValid()) {
					m_dataTransformModel->SetInputField(m_glyphTreeID, m_glyph.constify(), static_cast<SynGlyphX::DataMappingGlyph::MappableField>(index.row()), inputfield);
				}
				else {
					m_dataTransformModel->ClearInputBinding(m_glyphTreeID, m_glyph.constify(), static_cast<SynGlyphX::DataMappingGlyph::MappableField>(index.row()));
				}
			}
			catch (const std::invalid_argument& e) {

				QMessageBox::warning(nullptr, "Data does not match", e.what());
				return false;
			}
		}
		else {

			SynGlyphX::DataMappingGlyph minMaxGlyph(*m_glyph);

			if (!SetDataByRow(minMaxGlyph, value, index)) {
				
				return false;
			}

			m_dataTransformModel->UpdateGlyph(m_selectedIndex, minMaxGlyph);
		}

		emit dataChanged(index, index);
		return true;
	}

	return false;
}

bool MinMaxGlyphModel::SetDataByRow(SynGlyphX::DataMappingGlyph& glyph, const QVariant& value, const QModelIndex& index) {

	int row = index.row();
	if ((row < m_propertyHeaders.size()) && (!IsTextField(row))) {

		if (IsColorField(row)) {

			QColor color = value.value < QColor >();
			std::pair<SynGlyphX::GlyphColor, SynGlyphX::GlyphColor>& colorMinMax = glyph.GetColor().GetValue();
			if (index.column() == 0) {

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
			if (index.column() == 0) {

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
}

SynGlyphX::DataTransformMapping::ConstSharedPtr MinMaxGlyphModel::GetDataTransformMapping() const {

	return m_dataTransformModel->GetDataMapping();
}

bool MinMaxGlyphModel::IsCurrentGlyphRoot() const {

	if (m_glyphTree == nullptr) {

		return false;
	}

	return (m_glyphTree->root() == m_glyph.constify());
}

void MinMaxGlyphModel::ClearInputBindings() {

	for (int i = 0; i < rowCount(); ++i) {

		m_dataTransformModel->ClearInputBinding(m_glyphTreeID, m_glyph.constify(), static_cast<SynGlyphX::DataMappingGlyph::MappableField>(i));
	}
	emit dataChanged(index(0, columnCount() - 1), index(rowCount() - 1, columnCount() - 1));
}

bool MinMaxGlyphModel::IsTextField(int row) const {

	return ((row == 11) || (row == 12));
}

bool MinMaxGlyphModel::IsColorField(int row) const {

	return (row == 9);
}