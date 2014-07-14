#ifndef SYNGLYPHX_INPUTFIELD_H
#define SYNGLYPHX_INPUTFIELD_H

#include <string>
#include <boost/uuid/uuid.hpp>

namespace SynGlyphX {

	class InputField
	{
	public:
		InputField(const boost::uuids::uuid& uuid, const std::wstring& table, const std::wstring field);
		InputField(const InputField& inputField);
		~InputField();

		InputField& operator=(const InputField& inputField);

		void SetMinMax(double min, double max);

		const boost::uuids::uuid& GetUUID() const;
		const std::wstring& GetTable() const;
		const std::wstring& GetField() const;
		double GetMin() const;
		double GetMax() const;

	private:
		boost::uuids::uuid m_uuid;
		std::wstring m_table;
		std::wstring m_field;
		double m_min;
		double m_max;
	};

} //namespace SynGlyphX

#endif SYNGLYPHX_INPUTFIELD_H

