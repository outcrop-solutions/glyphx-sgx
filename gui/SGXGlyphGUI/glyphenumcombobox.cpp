#include "glyphenumcombobox.h"

namespace SynGlyphX {

	GlyphShapeComboBox::GlyphShapeComboBox(QWidget *parent)
		: QComboBox(parent) {
	
		//Set to the shape with the longest text name and add one additional character
		setMinimumContentsLength(13);
		for (auto shape : SynGlyphX::GlyphGeometryInfo::s_shapeNames.left) {

			addItem(QString::fromStdWString(shape.second));
		}
	}

	GlyphShapeComboBox::~GlyphShapeComboBox()
	{

	}

	GlyphGeometryInfo::Shape GlyphShapeComboBox::GetCurrentValue() const {

		return SynGlyphX::GlyphGeometryInfo::s_shapeNames.right.at(currentText().toStdWString());
	}

	void GlyphShapeComboBox::SetCurrentValue(GlyphGeometryInfo::Shape value) {

		blockSignals(true);
		setCurrentText(QString::fromStdWString(SynGlyphX::GlyphGeometryInfo::s_shapeNames.left.at(value)));
		blockSignals(false);
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

	VirtualTopologyInfo::Type VirtualTopologyComboBox::GetCurrentValue() const {

		return SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.right.at(currentText().toStdWString());
	}

	void VirtualTopologyComboBox::SetCurrentValue(VirtualTopologyInfo::Type value) {

		blockSignals(true);
		if (value == SynGlyphX::VirtualTopologyInfo::Type::Null) {

			setCurrentText(QString::fromStdWString(SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left.at(SynGlyphX::VirtualTopologyInfo::Type::Circle)));
		}
		else {

			setCurrentText(QString::fromStdWString(SynGlyphX::VirtualTopologyInfo::s_virtualTopologyNames.left.at(value)));
		}
		blockSignals(false);
	}
}