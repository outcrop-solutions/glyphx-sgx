#include "datamapping.h"

namespace SynGlyphX {

	DataMapping::DataMapping()
	{
	}


	DataMapping::~DataMapping()
	{
	}

	void DataMapping::AddInputField(const InputField& inputField) {

		//This line is here until multiple input fields are supported
		m_inputFields.clear();
		m_inputFields.push_back(inputField);
	}

} //namespace SynGlyphX