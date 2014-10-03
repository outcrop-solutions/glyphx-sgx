#ifndef INPUTFIELDDATA_H
#define INPUTFIELDDATA_H

#include <QtCore/QVariant>
#include <boost/shared_ptr.hpp>

namespace SynGlyphX {

	class InputFieldData
	{
	public:
		typedef boost::shared_ptr<InputFieldData> SharedPtr;
		typedef boost::shared_ptr<const InputFieldData> ConstSharedPtr;

		InputFieldData(const QVariantList& data, double min = 0.0, double max = 0.0);
		~InputFieldData();

		const QVariantList& GetData() const;
		double GetMin() const;
		double GetMax() const;
		double GetMaxMinDifference() const;

	private:
		double m_min;
		double m_max;
		QVariantList m_data;
		double m_maxMinDifference;
	};

} //namespace SynGlyphX

#endif // INPUTFIELDDATA_H
