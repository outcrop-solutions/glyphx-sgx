#include "range.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace SynGlyphX {

	Range::Range(double min, double max) :
		m_min(min),
		m_max(max)
	{
		if (m_min >= m_max) {

			throw std::invalid_argument("Range object must have a min less than its max.");
		}
	}

	Range::Range(const Range& range) :
		m_min(range.m_min),
		m_max(range.m_max) {


	}

	Range::~Range()
	{
	}

	Range& Range::operator=(const Range& range) {

		m_min = range.m_min;
		m_max = range.m_max;

		return *this;
	}

	bool Range::operator==(const Range& range) const {

		return ((m_min == range.m_min) && (m_max == range.m_max));
	}

	bool Range::operator!=(const Range& range) const {

		return !operator==(range);
	}

	bool Range::operator<(const Range& range) const {

		return (m_max <= range.m_min);
	}

	//bool Range::operator<(double value) const {
	//
	//	return IsValueInRange(value);
	//}

	double Range::GetMin() const {

		return m_min;
	}

	double Range::GetMax() const {

		return m_max;
	}

	bool Range::IsValueInRange(double value) const {

		return ((value >= m_min) && (value < m_max));
	}

	RangeTranslator::RangeTranslator() {


	}

	boost::optional<Range> RangeTranslator::get_value(std::wstring const &v) {

		int position = v.find(L',');

		if (position != -1) {

			try {

				Range range(boost::lexical_cast<double>(v.substr(0, position)), boost::lexical_cast<double>(v.substr(position + 1)));
				return range;
			}
			catch (...) {

			}
		}

		return boost::none;
	}

	boost::optional<std::wstring> RangeTranslator::put_value(Range const& v) {

		return (boost::lexical_cast<std::wstring>(v.GetMin()) + L',' + boost::lexical_cast<std::wstring>(v.GetMax()));
	}

} //namespace SynGlyphX