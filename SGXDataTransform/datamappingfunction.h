#ifndef SYNGLYPHX_DATAMAPPINGFUNCTION_H
#define SYNGLYPHX_DATAMAPPINGFUNCTION_H

#include <vector>
#include "inputfield.h"

namespace SynGlyphX {

	class DataMappingFunction
	{
	public:
		DataMappingFunction();
		~DataMappingFunction();

		void AddInputField(const InputField& inputField);
		void ClearInputFields();

	private:
		std::vector<InputField> m_inputFields;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPING_H
