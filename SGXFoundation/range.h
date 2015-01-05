#ifndef SYNGLYPHX_RANGE_H
#define SYNGLYPHX_RANGE_H

#include "sgxfoundation.h"

namespace SynGlyphX {

	class SGXFOUNDATION_API Range
	{
	public:
		Range(double min, double max);
		Range(const Range& range);
		~Range();

		Range& operator=(const Range& range);
		bool operator==(const Range& range) const;
		bool operator!=(const Range& range) const;
		bool operator<(const Range& range) const;
		bool operator<(double value) const;

		double GetMin() const;
		double GetMax() const;

		bool IsValueInRange(double value) const;

	private:
		double m_min;
		double m_max;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_RANGE_H