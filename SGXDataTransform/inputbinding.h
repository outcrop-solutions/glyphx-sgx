#ifndef SYNGLYPHX_INPUTBINDING_H
#define SYNGLYPHX_INPUTBINDING_H

#include "sgxdatatransform_global.h"
#include "inputfield.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT InputBinding
	{
	public:
		static const std::wstring PropertyTreeName;

		InputBinding();
		InputBinding(InputField::HashID inputFieldID);
		InputBinding(const InputBinding& binding);
		~InputBinding();

		InputBinding& operator=(const InputBinding& binding);

		/*double GetMin() const;
		double GetMax() const;
		void SetMinMax(double min, double max);*/

		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		bool IsBoundToInputField() const;

		InputField::HashID GetInputFieldID() const;

	private:
		InputField::HashID m_inputFieldID;

		double m_min;
		double m_max;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_INPUTBINDING_H