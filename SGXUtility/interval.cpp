#include "interval.h"
#include <stdexcept>
#include <boost/lexical_cast.hpp>

namespace SynGlyphX {

	Interval::Interval(double min, double max, Type type) :
		m_min(min),
		m_max(max),
		m_type(type)
	{
		
	}

	Interval::Interval(const Interval& interval) :
		m_min(interval.m_min),
		m_max(interval.m_max),
		m_type(interval.m_type) {


	}

	Interval::~Interval()
	{
	}

	Interval& Interval::operator=(const Interval& interval) {

		m_min = interval.m_min;
		m_max = interval.m_max;
		m_type = interval.m_type;

		return *this;
	}

	bool Interval::operator==(const Interval& interval) const {

		return ((m_min == interval.m_min) && 
				(m_max == interval.m_max) && 
				(m_type == interval.m_type));
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

	DegenerateInterval::DegenerateInterval(double min, double max) :
		Interval(min, max, Type::Degenerate) {

		if (!IsMinMaxValidForThisInterval(m_min, m_max)) {

			throw std::invalid_argument("Min and max values are not valid for a degenerate interval.");
		}
	}

	DegenerateInterval::DegenerateInterval(const DegenerateInterval& interval) :
		Interval(interval) {

	}

	DegenerateInterval::~DegenerateInterval() {

	}

	DegenerateInterval& DegenerateInterval::operator=(const DegenerateInterval& interval) {

		Interval::operator=(interval);

		return *this;
	}
	
	bool DegenerateInterval::operator==(const DegenerateInterval& interval) const {

		return Interval::operator==(interval);
	}

	bool DegenerateInterval::operator!=(const DegenerateInterval& interval) const {

		return !operator==(interval);
	}

	bool DegenerateInterval::IsValueInInterval(double value) const {

		return ((value >= m_min) && (value <= m_max));
	}

	bool DegenerateInterval::IsMinMaxValidForThisInterval(double min, double max) const {

		return (min <= max);
	}

	ProperInterval::ProperInterval(double min, double max, IncludedEndpoints includedEndpoints) :
		Interval(min, max, Type::Proper),
		m_includedEndpoints(includedEndpoints) {

		if (!IsMinMaxValidForThisInterval(m_min, m_max)) {

			throw std::invalid_argument("Min and max values are not valid for a proper interval.");
		}
	}

	ProperInterval::ProperInterval(const ProperInterval& interval) :
		Interval(interval),
		m_includedEndpoints(interval.m_includedEndpoints) {

	}

	ProperInterval::~ProperInterval() {

	}

	ProperInterval& ProperInterval::operator=(const ProperInterval& interval) {

		Interval::operator=(interval);
		m_includedEndpoints = interval.m_includedEndpoints;

		return *this;
	}

	bool ProperInterval::operator==(const ProperInterval& interval) const {

		if (Interval::operator!=(interval)) {

			return false;
		}

		if (m_includedEndpoints != m_includedEndpoints) {

			return false;
		}

		return true;
	}

	bool ProperInterval::operator!=(const ProperInterval& interval) const {

		return !operator==(interval);
	}

	ProperInterval::IncludedEndpoints ProperInterval::GetIncludedEndpoints() const {

		return m_includedEndpoints;
	}

	bool ProperInterval::IsValueInInterval(double value) const {

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

	bool ProperInterval::IsMinMaxValidForThisInterval(double min, double max) const {

		return (min < max);
	}

} //namespace SynGlyphX