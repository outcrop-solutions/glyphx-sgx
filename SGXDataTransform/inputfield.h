#ifndef SYNGLYPHX_INPUTFIELD_H
#define SYNGLYPHX_INPUTFIELD_H

#include "sgxdatatransform_global.h"
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/property_tree/ptree.hpp>
#include <QtCore/QMetaType>
#include <boost/bimap.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT InputField
	{

	public:
		enum Type {
			Null = 0,
			Integer,
			Real,
			Text,
			Date
		};

		InputField();
		InputField(const boost::uuids::uuid& datasourceID, const std::wstring& table, const std::wstring field, Type type);
		InputField(const boost::property_tree::wptree& propertyTree);
		InputField(const InputField& inputField);
		~InputField();

		InputField& operator=(const InputField& inputField);

		const boost::uuids::uuid& GetDatasourceID() const;
		const std::wstring& GetTable() const;
		const std::wstring& GetField() const;

		double GetMin() const;
		double GetMax() const;
		void SetMinMax(double min, double max);

		bool IsValid() const;

		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		bool IsNumeric() const;

		static const boost::bimap<Type, std::wstring> s_fieldTypeStrings;

	private:
		boost::uuids::uuid m_datasourceID;
		std::wstring m_table;
		std::wstring m_field;
		double m_min;
		double m_max;
		Type m_type;
	};

} //namespace SynGlyphX

Q_DECLARE_METATYPE(SynGlyphX::InputField)

#endif SYNGLYPHX_INPUTFIELD_H

