#include "minmaxglyphmodel.h"
#include <QtGui/QColor>

MinMaxGlyphModel::MinMaxGlyphModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	m_labels << tr("Position X")
		<< tr("Position Y")
		<< tr("Position Z")
		<< tr("Rotation X")
		<< tr("Rotation Y")
		<< tr("Rotation Z")
		<< tr("Scale X")
		<< tr("Scale Y")
		<< tr("Scale Z")
		<< tr("Color")
		<< tr("Transparency");
}

MinMaxGlyphModel::~MinMaxGlyphModel()
{

}

int MinMaxGlyphModel::columnCount(const QModelIndex& parent) const {

	return 4;
}

QVariant MinMaxGlyphModel::data(const QModelIndex& index, int role) const {

	if (index.column() == 0) {
		return m_labels[index.row()];
	}

	if (index.isValid() && m_glyph.valid()) {

		if (index.column() == 1) {
			return GetDataByRow(m_glyph->GetMinGlyph(), SynGlyphX::GlyphMappableProperties::GetPropertiesZero(), index.row());
		}
		else if (index.column() == 2) {
			return GetDataByRow(m_glyph->GetMinGlyph(), m_glyph->GetDifference(), index.row());
		}
	}

	return QVariant();
}

int	MinMaxGlyphModel::rowCount(const QModelIndex& parent) const {

	return 11;
}

QVariant MinMaxGlyphModel::headerData(int section, Qt::Orientation orientation, int role) const {

	//We don't need column headers in views so this gets rid of them
	return "";
}

void MinMaxGlyphModel::SetMinMaxGlyph(const QModelIndex& index) {

	beginResetModel();
	m_glyph = SynGlyphX::MinMaxGlyphTree::iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(index.internalPointer()));
	endResetModel();
}

QVariant MinMaxGlyphModel::GetDataByRow(const SynGlyphX::GlyphMappableProperties& minProperties, const SynGlyphX::GlyphMappableProperties& diffProperties, int row) const {

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

	return QAbstractTableModel::setData(index, value, role);
}