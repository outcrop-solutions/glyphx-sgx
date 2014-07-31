#ifndef SYNGLYPHX_INPUTFIELD_H
#define SYNGLYPHX_INPUTFIELD_H

#include "sgxdatatransform_global.h"
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/property_tree/ptree.hpp>
#include <QtCore/QMetaType>
#include <boost/bimap.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/functional/hash.hpp>

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

		typedef boost::shared_ptr<InputField> SharedPtr;
		typedef boost::shared_ptr<const InputField> ConstSharedPtr;
		typedef size_t HashID;

		InputField();
		InputField(const boost::uuids::uuid& datasourceID, const std::wstring& table, const std::wstring field, Type type);
		InputField(const boost::property_tree::wptree& propertyTree);
		InputField(const InputField& inputField);
		~InputField();

		InputField& operator=(const InputField& inputField);

		const boost::uuids::uuid& GetDatasourceID() const;
		const std::wstring& GetTable() const;
		const std::wstring& GetField() const;

		bool IsValid() const;

		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		bool IsNumeric() const;

		static const boost::bimap<Type, std::wstring> s_fieldTypeStrings;

		friend std::size_t hash_value(InputField const& inputfield) {

			std::size_t seed = 0;
			boost::hash_combine(seed, inputfield.m_datasourceID);
			boost::hash_combine(seed, inputfield.m_table);
			boost::hash_combine(seed, inputfield.m_field);

			return seed;
		}

	private:
		boost::uuids::uuid m_datasourceID;
		std::wstring m_table;
		std::wstring m_field;
		Type m_type;
	};

} //namespace SynGlyphX

Q_DECLARE_METATYPE(SynGlyphX::InputField)

#endif SYNGLYPHX_INPUTFIELD_H

