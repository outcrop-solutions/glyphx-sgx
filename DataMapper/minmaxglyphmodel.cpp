#include "minmaxglyphmodel.h"
#include <QtGui/QColor>
#include "sourcedatamanager.h"
#include <QtWidgets/QMessageBox>

MinMaxGlyphModel::MinMaxGlyphModel(DataTransformModel* dataTransformModel, QObject *parent)
	: QAbstractTableModel(parent),
	m_dataTransformModel(dataTransformModel),
	m_glyphTree(nullptr)
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
		<< tr("Torus Ratio")
		<< tr("Tag");

	m_columnHeaders << tr("Min")
		<< tr("Max")
		<< tr("Input");
}

MinMaxGlyphModel::~MinMaxGlyphModel()
{

}

int MinMaxGlyphModel::columnCount(const QModelIndex& parent) const {

	return 3;
}

QVariant MinMaxGlyphModel::data(const QModelIndex& index, int role) const {

	if (index.isValid() && m_glyph.valid()) {

		if (index.column() == 0) {

			SynGlyphX::GlyphNumericMappableProperties zeroProperties;
			zeroProperties.SetToZero();
			return GetDataByRow(m_glyph->GetMinGlyph(), zeroProperties, index.row());
		}
		else if (index.column() == 1) {

			return GetDataByRow(m_glyph->GetMinGlyph(), m_glyph->GetDifference(), index.row());
		}
		else if (index.column() == 2) {

			const SynGlyphX::InputBinding& binding = m_glyph->GetInputBinding(index.row());

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

		return SynGlyphX::MinMaxGlyph::NumInputBindings;
	}
	else {

		return 0;
	}
}

void MinMaxGlyphModel::SetMinMaxGlyph(const QModelIndex& index) {

	if (index.isValid()) {

		SynGlyphX::MinMaxGlyphTree::Node* node = static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(index.internalPointer());
		beginResetModel();
		m_glyph = SynGlyphX::MinMaxGlyphTree::iterator(node);
		m_glyphTree = static_cast<const SynGlyphX::MinMaxGlyphTree*>(m_glyph.owner());
		endResetModel();

		for (auto glyphTree : m_dataTransformModel->GetDataMapping()->GetGlyphTrees()) {

			if (glyphTree.second.get() == m_glyphTree) {
				m_glyphTreeID = glyphTree.first;
				break;
			}
		}
	}
	else {

		Clear();
	}
}

void MinMaxGlyphModel::Clear() {

	beginResetModel();
	m_glyph = SynGlyphX::MinMaxGlyphTree::iterator();
	m_glyphTree = nullptr;
	m_glyphTreeID = boost::uuids::nil_uuid();
	endResetModel();
}

bool MinMaxGlyphModel::IsClear() const {

	return (m_glyphTree == nullptr);
}

QVariant MinMaxGlyphModel::GetDataByRow(const SynGlyphX::GlyphNumericMappableProperties& minProperties, const SynGlyphX::GlyphNumericMappableProperties& diffProperties, int row) const {

	if (row == 0) {
		return minProperties.GetPosition()[0] + diffProperties.GetPosition()[0];
	}
	else if (row == 1) {
		return minProperties.GetPosition()[1] + diffProperties.GetPosition()[1];
	}
	else if (row == 2) {
		return minProperties.GetPosition()[2] + diffProperties.GetPosition()[2];
	}
	else if (row == 3) {
		return minProperties.GetRotation()[0] + diffProperties.GetRotation()[0];
	}
	else if (row == 4) {
		return minProperties.GetRotation()[1] + diffProperties.GetRotation()[1];
	}
	else if (row == 5) {
		return minProperties.GetRotation()[2] + diffProperties.GetRotation()[2];
	}
	else if (row == 6) {
		return minProperties.GetScale()[0] + diffProperties.GetScale()[0];
	}
	else if (row == 7) {
		return minProperties.GetScale()[1] + diffProperties.GetScale()[1];
	}
	else if (row == 8) {
		return minProperties.GetScale()[2] + diffProperties.GetScale()[2];
	}
	else if (row == 9) {
		return QColor(minProperties.GetColor()[0] + diffProperties.GetColor()[0], minProperties.GetColor()[1] + diffProperties.GetColor()[1], minProperties.GetColor()[2] + diffProperties.GetColor()[2]);
	} 
	else if (row == 10) {
		return minProperties.GetColor()[3] + diffProperties.GetColor()[3];
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
					m_dataTransformModel->SetInputField(m_glyphTreeID, m_glyph.constify(), index.row(), inputfield);
				}
				else {
					m_dataTransformModel->ClearInputBinding(m_glyphTreeID, m_glyph.constify(), index.row());
				}
			}
			catch (const std::invalid_argument& e) {

				QMessageBox::warning(nullptr, "Data does not match", e.what());
				return false;
			}
		}
		else {

			SynGlyphX::GlyphMappableProperties minProperties = m_glyph->GetMinGlyph();
			SynGlyphX::GlyphNumericMappableProperties diffProperties = m_glyph->GetDifference();

			if (!SetDataByRow(minProperties, diffProperties, value, index)) {
				
				return false;
			}

			m_glyph->SetMinGlyphProperties(minProperties);
			m_glyph->SetDifference(diffProperties);

			SynGlyphX::GlyphProperties::SharedPtr newGlyph(new SynGlyphX::GlyphProperties(m_glyph->GetMinGlyph()));
			emit GlyphPropertiesUpdated(newGlyph);
		}

		emit dataChanged(index, index);
		return true;
	}

	return false;
}

bool MinMaxGlyphModel::SetDataByRow(SynGlyphX::GlyphNumericMappableProperties& minProperties, SynGlyphX::GlyphNumericMappableProperties& diffProperties, const QVariant& value, const QModelIndex& index) {

	int row = index.row();
	if ((row == 0) || (row == 1) || (row == 2)) {

		double number = value.toDouble();
		SynGlyphX::Vector3 min = minProperties.GetPosition();
		SynGlyphX::Vector3 diff = diffProperties.GetPosition();
		if (index.column() == 0) {

			diff[row] += min[row] - number;
			min[row] = number;
		}
		else {
			diff[row] = number - min[row];
		}

		minProperties.SetPosition(min);
		diffProperties.SetPosition(diff);

		return true;
	}
	else if ((row == 3) || (row == 4) || (row == 5)) {
		
		row -= 3;
		double number = value.toDouble();
		SynGlyphX::Vector3 min = minProperties.GetRotation();
		SynGlyphX::Vector3 diff = diffProperties.GetRotation();
		if (index.column() == 0) {

			diff[row] += min[row] - number;
			min[row] = number;
		}
		else {
			diff[row] = number - min[row];
		}

		minProperties.SetRotation(min);
		diffProperties.SetRotation(diff);

		return true;
	}
	else if ((row == 6) || (row == 7) || (row == 8)) {

		row -= 6;
		double number = value.toDouble();
		SynGlyphX::Vector3 min = minProperties.GetScale();
		SynGlyphX::Vector3 diff = diffProperties.GetScale();
		if (index.column() == 0) {

			diff[row] += min[row] - number;
			min[row] = number;
		}
		else {
			diff[row] = number - min[row];
		}

		minProperties.SetScale(min);
		diffProperties.SetScale(diff);

		return true;
	}
	else if (row == 9) {

		QColor color = value.value<QColor>();
		SynGlyphX::Color min = minProperties.GetColor();
		SynGlyphX::Color diff = diffProperties.GetColor();
		if (index.column() == 0) {

			diff.Set(0, diff[0] + min[0] - color.red());
			diff.Set(1, diff[1] + min[1] - color.green());
			diff.Set(2, diff[2] + min[2] - color.blue());
			min.Set(0, color.red());
			min.Set(1, color.green());
			min.Set(2, color.blue());
		}
		else {

			diff.Set(0, color.red() - min[0]);
			diff.Set(1, color.green() - min[1]);
			diff.Set(2, color.blue() - min[2]);
		}

		minProperties.SetColor(min);
		diffProperties.SetColor(diff);

		return true;
	}
	else if (row == 10) {

		unsigned int alpha = value.toUInt();
		SynGlyphX::Color min = minProperties.GetColor();
		SynGlyphX::Color diff = diffProperties.GetColor();
		if (index.column() == 0) {

			diff.Set(3, diff[3] + min[3] - alpha);
			min.Set(3, alpha);
		}
		else {
			diff.Set(3, alpha - min[3]);
		}

		minProperties.SetColor(min);
		diffProperties.SetColor(diff);

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

		m_dataTransformModel->ClearInputBinding(m_glyphTreeID, m_glyph.constify(), i);
	}
	emit dataChanged(index(0, columnCount() - 1), index(rowCount() - 1, columnCount() - 1));
}