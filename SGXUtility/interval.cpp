#include "interval.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace SynGlyphX {

	Interval::Interval(double min, double max, Type type, IncludedEndpoints includedEndpoints) :
		m_min(min),
		m_max(max),
		m_type(type),
		m_includedEndpoints(includedEndpoints)
	{
		if (!IsMinMaxValidForThisInterval(m_min, m_max)) {

			throw std::invalid_argument("Min and max values are not valid for an interval of this type.");
		}
	}

	Interval::Interval(const Interval& interval) :
		m_min(interval.m_min),
		m_max(interval.m_max),
		m_type(interval.m_type),
		m_includedEndpoints(interval.m_includedEndpoints) {


	}

	Interval::~Interval()
	{
	}

	Interval& Interval::operator=(const Interval& interval) {

		m_min = interval.m_min;
		m_max = interval.m_max;
		m_type = interval.m_type;
		m_includedEndpoints = interval.m_includedEndpoints;

		return *this;
	}

	bool Interval::operator==(const Interval& interval) const {

		return ((m_min == interval.m_min) && 
				(m_max == interval.m_max) && 
				(m_type == interval.m_type) &&
				(m_includedEndpoints == interval.m_includedEndpoints));
	}

	bool Interval::operator!=(const Interval& interval) const {

		return !operator==(interval);
	}

	bool Interval::operator<(const Interval& interval) const {

		return (m_max <= interval.m_min);
	}

	//bool Interval::operator<(double value) const {
	//
	//	return IsValueInInterval(value);
	//}

	double Interval::GetMin() const {

		return m_min;
	}

	double Interval::GetMax() const {

		return m_max;
	}

	Interval::Type Interval::GetType() const {

		return m_type;
	}

	Interval::IncludedEndpoints Interval::GetIncludedEndpoints() const {

		return m_includedEndpoints;
	}

	bool Interval::IsValueInInterval(double value) const {

		if (m_includedEndpoints == IncludedEndpoints::Neither) {
			
			return ((value > m_min) && (value < m_max));
		}
		else if (m_includedEndpoints == IncludedEndpoints::Min) {

			return ((value >= m_min) && (value < m_max));
		}
		else if (m_includedEndpoints == IncludedEndpoints::Max) {

			return ((value > m_min) && (value <= m_max));
		}
		else if (m_includedEndpoints == IncludedEndpoints::Both) {

			return ((value >= m_min) && (value <= m_max));
		}
		
		return false;
	}

	bool Interval::IsMinMaxValidForThisInterval(double min, double max) const {

		if (m_type == Type::Proper) {

			return (min < max);
		}
		else {

			return (min <= max);
		}
	}

} //namespace SynGlyphX