#ifndef SYNGLYPHX_DATAMAPPING_H
#define SYNGLYPHX_DATAMAPPING_H

#include <vector>
#include "inputfield.h"

namespace SynGlyphX {

	class DataMapping
	{
	public:
		enum Function {
			LinearInterpolation,
			ColorInterpolation
		};

		DataMapping();
		~DataMapping();

		void AddInputField(const InputField& inputField);

	private:
		std::vector<InputField> m_inputFields;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPING_H
