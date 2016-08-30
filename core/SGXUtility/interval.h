///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///
#pragma once
#ifndef SYNGLYPHX_INTERVAL_H
#define SYNGLYPHX_INTERVAL_H

#include "sgxutility.h"
#include <vector>

namespace SynGlyphX {

	class SGXUTILITY_API Interval
	{
	public:
		enum Type {

			Proper,		//min must be less than max
			Degenerate	//min can be the same as max
		};

		Interval(double min, double max, Type type);
		Interval(const Interval& interval);
		virtual ~Interval();

		Interval& operator=(const Interval& interval);
		bool operator==(const Interval& interval) const;
		bool operator!=(const Interval& interval) const;
		bool operator<(const Interval& interval) const;
		//bool operator<(double value) const;

		double GetMin() const;
		double GetMax() const;
		Type GetType() const;

		virtual bool IsValueInInterval(double value) const = 0;

	protected:
		virtual bool IsMinMaxValidForThisInterval(double min, double max) const = 0;

		double m_min;
		double m_max;
		Type m_type;
	};

	class SGXUTILITY_API DegenerateInterval : public Interval {

	public:
		DegenerateInterval(double min = 0.0, double max = 1.0);
		DegenerateInterval(const DegenerateInterval& interval);
		virtual ~DegenerateInterval();

		DegenerateInterval& operator=(const DegenerateInterval& interval);
		bool operator==(const DegenerateInterval& interval) const;
		bool operator!=(const DegenerateInterval& interval) const;

		bool IsValueInInterval(double value) const override;

	protected:
		bool IsMinMaxValidForThisInterval(double min, double max) const override;
	};

	typedef std::vector<DegenerateInterval> DegenerateIntervalUnion;

	class SGXUTILITY_API ProperInterval : public Interval {

	public:
		enum IncludedEndpoints {

			Neither,
			Min,
			Max,
			Both
		};

		ProperInterval(double min = 0.0, double max = 1.0, IncludedEndpoints includedEndpoints = IncludedEndpoints::Both);
		ProperInterval(const ProperInterval& interval);
		virtual ~ProperInterval();

		ProperInterval& operator=(const ProperInterval& interval);
		bool operator==(const ProperInterval& interval) const;
		bool operator!=(const ProperInterval& interval) const;

		IncludedEndpoints GetIncludedEndpoints() const;
		bool IsValueInInterval(double value) const override;

	protected:
		bool IsMinMaxValidForThisInterval(double min, double max) const override;

	private:
		IncludedEndpoints m_includedEndpoints;
	};

	typedef std::vector<ProperInterval> ProperIntervalUnion;

} //namespace SynGlyphX

#endif //SYNGLYPHX_INTERVAL_H