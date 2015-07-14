#include "defaultbaseimagescombobox.h"
#include "application.h"

namespace SynGlyphX {

	std::map<DefaultBaseImageProperties::Type, QString> DefaultBaseImagesComboBox::s_typeToNameMap;

	DefaultBaseImagesComboBox::DefaultBaseImagesComboBox(QWidget *parent)
		: QComboBox(parent)
	{
		Setup();

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

		std::map<DefaultBaseImageProperties::Type, QString>::iterator iT = s_typeToNameMap.find(defaultBaseImage);
		if (iT != s_typeToNameMap.end()) {

			setCurrentText(iT->second);
		}
	}

	DefaultBaseImageProperties::Type DefaultBaseImagesComboBox::GetDefaultBaseImage() const {

		return static_cast<DefaultBaseImageProperties::Type>(currentData().toInt());
	}

	void DefaultBaseImagesComboBox::Setup() {

		if (s_typeToNameMap.empty()) {

			for (auto defaultBaseImageType : DefaultBaseImageProperties::s_typeStrings) {

				if (defaultBaseImageType.left == DefaultBaseImageProperties::Type::WorldGrayscale) {

					s_typeToNameMap.insert(std::pair<DefaultBaseImageProperties::Type, QString>(defaultBaseImageType.left, "World (Grayscale)"));
				} 
				else {

					s_typeToNameMap.insert(std::pair<DefaultBaseImageProperties::Type, QString>(defaultBaseImageType.left, QString::fromStdWString(defaultBaseImageType.right)));
				}
			}

		}
	}

} //namespace SynGlyphX