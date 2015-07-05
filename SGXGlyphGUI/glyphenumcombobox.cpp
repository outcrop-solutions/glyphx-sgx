#include "glyphenumcombobox.h"

namespace SynGlyphX {

	GlyphShapeComboBox::GlyphShapeComboBox(QWidget *parent)
		: QComboBox(parent)
	{
		for (auto shape : SynGlyphX::GlyphGeometryInfo::s_shapeNames.left) {

			addItem(QString::fromStdWString(shape.second));
		}
	}

	GlyphShapeComboBox::~GlyphShapeComboBox()
	{

	}

	GlyphGeometryInfo::Shape GlyphShapeComboBox::GetCurentValue() const {

		return SynGlyphX::GlyphGeometryInfo::s_shapeNames.right.at(currentText().toStdWString());
	}

	void GlyphShapeComboBox::SetCurrentValue(GlyphGeometryInfo::Shape value) {

		setCurrentText(QString::fromStdWString(SynGlyphX::GlyphGeometryInfo::s_shapeNames.left.at(value)));
	}

	

	VirtualTopologyComboBox::VirtualTopologyComboBox(QWidget *parent)
		: QComboBox(parent)
	{
		for (auto virtualTopology : SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left) {

			addItem(QString::fromStdWString(virtualTopology.second));
		}
	}

	VirtualTopologyComboBox::~VirtualTopologyComboBox()
	{

	}

	VirtualTopologyInfo::Type VirtualTopologyComboBox::GetCurentValue() const {

		return SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.right.at(currentText().toStdWString());
	}

	void VirtualTopologyComboBox::SetCurrentValue(VirtualTopologyInfo::Type value) {

		setCurrentText(QString::fromStdWString(SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left.at(value)));
	}
}