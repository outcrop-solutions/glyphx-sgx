#ifndef SYNGLYPHX_INPUTBINDING_H
#define SYNGLYPHX_INPUTBINDING_H

#include "sgxdatatransform_global.h"
#include "inputfield.h"
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT InputBinding
	{
	public:
		static const std::wstring PropertyTreeName;

		InputBinding();
		InputBinding(InputField::HashID inputFieldID);
		InputBinding(const boost::property_tree::wptree& propertyTree);
		InputBinding(const InputBinding& binding);
		~InputBinding();

		InputBinding& operator=(const InputBinding& binding);
		bool operator==(const InputBinding& binding) const;
		bool operator!=(const InputBinding& binding) const;

		double GetMinOverride() const;
		double GetMaxOverride() const;
		void SetMinMaxOverride(double min, double max);
		bool IsMinMaxOverrideInUse() const;
		void ClearMinMaxOverride();
		double GetMaxMinOverrideDifference() const;

		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		bool IsBoundToInputField() const;

		InputField::HashID GetInputFieldID() const;

		void Clear();

	private:
		InputField::HashID m_inputFieldID;

		double m_minOverride;
		double m_maxOverride;
		bool m_overrideInUse;
		double m_maxMinOverrideDifference;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_INPUTBINDING_H