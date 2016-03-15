#include "valuemappingrange.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace SynGlyphX {

	ValueMappingRange::ValueMappingRange(double min, double max) :
		Interval(min, max, Interval::Type::Proper, Interval::IncludedEndpoints::Min)
	{
		if (!IsMinMaxValidForThisInterval(m_min, m_max)) {

			throw std::invalid_argument("Min and max values are not valid for an interval of this type.");
		}
	}

	ValueMappingRange::ValueMappingRange(const ValueMappingRange& interval) :
		Interval(interval) {


	}

	ValueMappingRange::~ValueMappingRange()
	{
	}

	ValueMappingRange& ValueMappingRange::operator=(const ValueMappingRange& interval) {

		Interval::operator=(interval);

		return *this;
	}

	bool ValueMappingRange::operator==(const ValueMappingRange& interval) const {

		return Interval::operator==(interval);
	}

	bool ValueMappingRange::operator!=(const ValueMappingRange& interval) const {

		return !operator==(interval);
	}

	bool ValueMappingRange::operator<(const ValueMappingRange& interval) const {

		return Interval::operator<(interval);
	}

	ValueMappingRangeTranslator::ValueMappingRangeTranslator() {


	}

	boost::optional<ValueMappingRange> ValueMappingRangeTranslator::get_value(std::wstring const &v) {

		int position = v.find(L',');

		if (position != -1) {

			try {

				ValueMappingRange interval(boost::lexical_cast<double>(v.substr(0, position)), boost::lexical_cast<double>(v.substr(position + 1)));
				return interval;
			}
			catch (...) {

			}
		}

		return boost::none;
	}

	boost::optional<std::wstring> ValueMappingRangeTranslator::put_value(ValueMappingRange const& v) {

		return (boost::lexical_cast<std::wstring>(v.GetMin()) + L',' + boost::lexical_cast<std::wstring>(v.GetMax()));
	}

} //namespace SynGlyphX