#include "mindiffdatamappingproperty.h"

namespace SynGlyphX {

	template <typename DataType>
	MinDiffDataMappingProperty<DataType>::MinDiffDataMappingProperty()
	{
	}

	template <typename DataType>
	MinDiffDataMappingProperty<DataType>::MinDiffDataMappingProperty(const std::pair<DataType, DataType>& initialValue) :
		DataMappingProperty < std::pair<DataType, DataType> >(initialValue) {


	}

	template <typename DataType>
	MinDiffDataMappingProperty<DataType>::MinDiffDataMappingProperty(const boost::property_tree::wptree& propertyTree) :
		DataMappingProperty < std::pair<DataType, DataType> >(propertyTree) {


	}

	template <typename DataType>
	MinDiffDataMappingProperty<DataType>::MinDiffDataMappingProperty(const MinDiffDataMappingProperty& prop) :
		DataMappingProperty < std::pair<DataType, DataType> >(prop) {


	}

	template <typename DataType>
	MinDiffDataMappingProperty<DataType>::~MinDiffDataMappingProperty()
	{
	}

	template <typename DataType>
	MinDiffDataMappingProperty<DataType>& MinDiffDataMappingProperty<DataType>::operator=(const MinDiffDataMappingProperty& prop) {

		DataMappingProperty<std::pair<DataType, DataType>>::operator=(prop);

		return *this;
	}

	template <typename DataType>
	bool MinDiffDataMappingProperty<DataType>::operator==(const MinDiffDataMappingProperty& prop) const {

		return DataMappingProperty<std::pair<DataType, DataType>>::operator==(prop);
	}

	template <typename DataType>
	bool MinDiffDataMappingProperty<DataType>::operator!=(const MinDiffDataMappingProperty& prop) const {

		return DataMappingProperty::operator!=(prop);
	}

	template <typename DataType>
	void MinDiffDataMappingProperty<DataType>::ExportValueToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//boost::property_tree::wptree& valuePropertyTree = propertyTreeParent.add(name, L"");
		propertyTree.put<DataType>(L"Min", m_value.first);

		if (std::abs(m_value.second) > 0.01) {

			propertyTree.put<DataType>(L"Difference", m_value.second);
		}
	}

	template <>
	void MinDiffDataMappingProperty<GlyphColor>::ExportValueToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//m_value.first.ExportToPropertyTree(propertyTree.add(L"Min", L""));
		propertyTree.put<GlyphColor>(L"Min", m_value.first);

		if ((std::abs(m_value.second[0]) > 0) || (std::abs(m_value.second[1]) > 0) || (std::abs(m_value.second[2]) > 0)) {

			//m_value.second.ExportToPropertyTree(propertyTree.add(L"Difference", L""));
			propertyTree.put<GlyphColor>(L"Difference", m_value.second);
		}
	}

	template <typename DataType>
	void MinDiffDataMappingProperty<DataType>::ImportValueToPropertyTree(const boost::property_tree::wptree& propertyTree) {

		m_value.first = propertyTree.get<DataType>(L"Min");
		m_value.second = propertyTree.get_optional<DataType>(L"Difference").get_value_or(DataType());
	}

	template <>
	void MinDiffDataMappingProperty<GlyphColor>::ImportValueToPropertyTree(const boost::property_tree::wptree& propertyTree) {

		//For color check if property tree has it in the old way.  If not read in using the new way
		boost::optional<short> rgbMinRed = propertyTree.get_optional<short>(L"Min.R");
		if (rgbMinRed.is_initialized()) {

			m_value.first.Set(0, rgbMinRed.get());
			m_value.first.Set(1, propertyTree.get<short>(L"Min.G"));
			m_value.first.Set(2, propertyTree.get<short>(L"Min.B"));
		}
		else {

			m_value.first = propertyTree.get<GlyphColor>(L"Min");
		}

		boost::optional<short> rgbDiffRed = propertyTree.get_optional<short>(L"Difference.R");
		if (rgbDiffRed.is_initialized()) {

			m_value.second.Set(0, rgbDiffRed.get());
			m_value.second.Set(1, propertyTree.get<short>(L"Difference.G"));
			m_value.second.Set(2, propertyTree.get<short>(L"Difference.B"));
		}
		else {

			m_value.second = propertyTree.get_optional<GlyphColor>(L"Difference").get_value_or(GlyphColor({ { 0, 0, 0 } }));
		}
	}

	template class MinDiffDataMappingProperty<double>;
	template class MinDiffDataMappingProperty<GlyphColor>;

} //namespace SynGlyphX