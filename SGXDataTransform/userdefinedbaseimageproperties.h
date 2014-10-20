#ifndef USERDEFINEDBASEIMAGEPROPERTIES_H
#define USERDEFINEDBASEIMAGEPROPERTIES_H

#include "sgxdatatransform_global.h"
#include "baseimageproperties.h"
#include <string>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT UserDefinedBaseImageProperties : public BaseImageProperties
	{
	public:
		UserDefinedBaseImageProperties(const std::wstring& filename);
		UserDefinedBaseImageProperties(const boost::property_tree::wptree& propertyTree);
		UserDefinedBaseImageProperties(const UserDefinedBaseImageProperties& properties);
		virtual ~UserDefinedBaseImageProperties();

		const std::wstring& GetFilename() const;

		virtual bool IsGeographic() const;
		virtual void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

	private:
		std::wstring m_filename;
	};

} //namespace SynGlyphX

#endif //USERDEFINEDBASEIMAGEPROPERTIES_H