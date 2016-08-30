#include "defaultbaseimagescombobox.h"
#include "application.h"

namespace SynGlyphX {

	const std::map<DefaultBaseImageProperties::Type, QString> DefaultBaseImagesComboBox::s_typeToNameMap = DefaultBaseImagesComboBox::SetupTypeToNameMap();

	DefaultBaseImagesComboBox::DefaultBaseImagesComboBox(QWidget *parent)
		: QComboBox(parent)
	{
		setInsertPolicy(QComboBox::InsertPolicy::InsertAlphabetically);

		for (auto defaultBaseMap : s_typeToNameMap) {

			addItem(defaultBaseMap.second, defaultBaseMap.first);
		}

		SetDefaultBaseImage(DefaultBaseImageProperties::Type::World);
	}

	DefaultBaseImagesComboBox::~DefaultBaseImagesComboBox()
	{

	}

	void DefaultBaseImagesComboBox::SetDefaultBaseImage(DefaultBaseImageProperties::Type defaultBaseImage) {

		std::map<DefaultBaseImageProperties::Type, QString>::const_iterator iT = s_typeToNameMap.find(defaultBaseImage);
		if (iT != s_typeToNameMap.end()) {

			setCurrentText(iT->second);
		}
	}

	DefaultBaseImageProperties::Type DefaultBaseImagesComboBox::GetDefaultBaseImage() const {

		return static_cast<DefaultBaseImageProperties::Type>(currentData().toInt());
	}

	std::map<DefaultBaseImageProperties::Type, QString> DefaultBaseImagesComboBox::SetupTypeToNameMap() {

		std::map<DefaultBaseImageProperties::Type, QString> typeToNameMap;
		for (auto defaultBaseImageType : DefaultBaseImageProperties::s_typeStrings) {

			if (defaultBaseImageType.left == DefaultBaseImageProperties::Type::WorldGrayscale) {

				typeToNameMap.insert(std::pair<DefaultBaseImageProperties::Type, QString>(defaultBaseImageType.left, "World (Grayscale)"));
			} 
			else {

				typeToNameMap.insert(std::pair<DefaultBaseImageProperties::Type, QString>(defaultBaseImageType.left, QString::fromStdWString(defaultBaseImageType.right)));
			}
		}

		return typeToNameMap;
	}

} //namespace SynGlyphX