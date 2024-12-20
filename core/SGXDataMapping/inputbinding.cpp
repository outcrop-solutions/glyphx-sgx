#include "inputbinding.h"
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	const std::wstring InputBinding::PropertyTreeName = L"Binding";

	InputBinding::InputBinding() :
		m_inputFieldID(L""),
		m_minOverride(0.0),
		m_maxOverride(0.0),
		m_overrideInUse(false)
	{
	}

	InputBinding::InputBinding(const std::wstring& inputFieldID) :
		m_inputFieldID(inputFieldID),
		m_minOverride(0.0),
		m_maxOverride(0.0),
		m_overrideInUse(false) {

	}

	InputBinding::InputBinding(const boost::property_tree::wptree& propertyTree) /*:
		m_inputFieldID(propertyTree.get<HashID>(L"<xmlattr>.fieldId")) */
	{ 
		// to deal with backwards compatibility, TODO: remove in the future 
		auto hash = propertyTree.get_optional<HashID>(L"<xmlattr>.id");
		if (hash.is_initialized())
			m_inputFieldID = L"~" + std::to_wstring(hash.get());
		
		auto fieldId = propertyTree.get_optional<std::wstring>(L"<xmlattr>.fieldId");
		if (fieldId.is_initialized())
			m_inputFieldID = fieldId.get();
			

		boost::optional<const boost::property_tree::wptree&> overridePropertyTree = propertyTree.get_child_optional(L"Override");

		m_overrideInUse = overridePropertyTree.is_initialized();
		if (m_overrideInUse) {

			m_minOverride = overridePropertyTree.get().get<double>(L"<xmlattr>.Min");
			m_maxOverride = overridePropertyTree.get().get<double>(L"<xmlattr>.Max");
		}
	}

	InputBinding::InputBinding(const InputBinding& binding) :
		m_inputFieldID(binding.m_inputFieldID),
		m_minOverride(binding.m_minOverride),
		m_maxOverride(binding.m_maxOverride),
		m_overrideInUse(binding.m_overrideInUse) {

	}

	InputBinding::~InputBinding()
	{
	}

	InputBinding& InputBinding::operator=(const InputBinding& binding) {

		m_inputFieldID = binding.m_inputFieldID;
		m_minOverride = binding.m_minOverride;
		m_maxOverride = binding.m_maxOverride;
		m_overrideInUse = binding.m_overrideInUse;

		return *this;
	}

	bool InputBinding::operator==(const InputBinding& binding) const {

		if (m_inputFieldID != binding.m_inputFieldID) {

			return false;
		}

		if (m_overrideInUse != binding.m_overrideInUse) {

			return false;
		}

		if (m_overrideInUse) {

			if (m_minOverride != binding.m_minOverride) {

				return false;
			}

			if (m_maxOverride != binding.m_maxOverride) {

				return false;
			}
		}

		return true;
	}

	bool InputBinding::operator!=(const InputBinding& binding) const {

		return !operator==(binding);
	}

	void InputBinding::SetMinMaxOverride(double min, double max) {

		m_minOverride = min;
		m_maxOverride = max;
		m_maxMinOverrideDifference = max - min;
		m_overrideInUse = true;
	}

	double InputBinding::GetMinOverride() const {

		return m_minOverride;
	}

	double InputBinding::GetMaxOverride() const {

		return m_maxOverride;
	}

	bool InputBinding::IsMinMaxOverrideInUse() const {

		return m_overrideInUse;
	}

	void InputBinding::ClearMinMaxOverride() {

		m_overrideInUse = false;
	}

	double InputBinding::GetMaxMinOverrideDifference() const {

		return m_maxMinOverrideDifference;
	}

	void InputBinding::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& inputFieldPropertyTree = propertyTree.add(PropertyTreeName, L"");
		inputFieldPropertyTree.put(L"<xmlattr>.fieldId", m_inputFieldID);

		if (m_overrideInUse) {

			boost::property_tree::wptree& overridePropertyTree = inputFieldPropertyTree.add(L"Override", L"");
			overridePropertyTree.put(L"<xmlattr>.Min", m_minOverride);
			overridePropertyTree.put(L"<xmlattr>.Max", m_maxOverride);
		}
	}

	bool InputBinding::IsBoundToInputField() const {

		return (!m_inputFieldID.empty());
	}

	const std::wstring& InputBinding::GetInputFieldID() const {

		return m_inputFieldID;
	}

	void InputBinding::Clear() {

		m_inputFieldID = L"";
		ClearMinMaxOverride();
	}

} //namespace SynGlyphX