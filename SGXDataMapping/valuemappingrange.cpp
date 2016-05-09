#include "valuemappingrange.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace SynGlyphX {

	ValueMappingRange::ValueMappingRange(double min, double max) :
		ProperInterval(min, max, ProperInterval::IncludedEndpoints::Min)
	{
		
	}

	ValueMappingRange::ValueMappingRange(const ValueMappingRange& interval) :
		ProperInterval(interval) {


	}

	ValueMappingRange::~ValueMappingRange()
	{
	}

	ValueMappingRange& ValueMappingRange::operator=(const ValueMappingRange& interval) {

		ProperInterval::operator=(interval);

		return *this;
	}

	bool ValueMappingRange::operator==(const ValueMappingRange& interval) const {

		return ProperInterval::operator==(interval);
	}

	bool ValueMappingRange::operator!=(const ValueMappingRange& interval) const {

		return !operator== (interval);
	}

	bool ValueMappingRange::operator<(const ValueMappingRange& interval) const {

		return ProperInterval::operator<(interval);
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