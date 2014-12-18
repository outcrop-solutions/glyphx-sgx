#include "datamappingfunction.h"

namespace SynGlyphX {

	DataMappingFunction::DataMappingFunction()
	{
	}


	DataMappingFunction::~DataMappingFunction()
	{
	}

	void DataMappingFunction::AddInputField(const InputField& inputField) {

		//This line is here until multiple input fields are supported
		ClearInputFields();
		m_inputFields.push_back(inputField);
	}

	void DataMappingFunction::ClearInputFields() {

		m_inputFields.clear();
	}

} //namespace SynGlyphX