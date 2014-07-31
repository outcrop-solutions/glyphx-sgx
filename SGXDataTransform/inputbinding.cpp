#include "inputbinding.h"

namespace SynGlyphX {

	const std::wstring InputBinding::PropertyTreeName = L"Binding";

	InputBinding::InputBinding() :
		m_inputFieldID(0),
		m_min(0.0),
		m_max(0.0)
	{
	}

	InputBinding::InputBinding(InputField::HashID inputFieldID) :
		m_inputFieldID(inputFieldID),
		m_min(0.0),
		m_max(0.0) {

	}

	InputBinding::InputBinding(const InputBinding& binding) :
		m_inputFieldID(binding.m_inputFieldID),
		m_min(binding.m_min),
		m_max(binding.m_max) {

	}

	InputBinding::~InputBinding()
	{
	}

	InputBinding& InputBinding::operator=(const InputBinding& binding) {

		m_inputFieldID = binding.m_inputFieldID;
		m_min = binding.m_min;
		m_max = binding.m_max;

		return *this;
	}

	/*void InputBinding::SetMinMax(double min, double max) {

		m_min = min;
		m_max = max;
	}

	double InputBinding::GetMin() const {

		return m_min;
	}

	double InputBinding::GetMax() const {

		return m_max;
	}*/

	void InputBinding::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& inputFieldPropertyTree = propertyTree.add(s_propertyTreeName, L"");
		inputFieldPropertyTree.put(L"<xmlattr>.id", m_inputFieldID);
		//inputFieldPropertyTree.put(L"Min", m_min);
		//inputFieldPropertyTree.put(L"Max", m_max);
	}

	bool InputBinding::IsBoundToInputField() const {

		return (m_inputFieldID != 0);
	}

	InputField::HashID InputBinding::GetInputFieldID() const {

		return m_inputFieldID;
	}

} //namespace SynGlyphX