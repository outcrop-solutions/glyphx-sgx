#include "virtualtopologyinfo.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const VirtualTopologyInfo::TypeStringBimap VirtualTopologyInfo::s_virtualTopologyNames = boost::assign::list_of < VirtualTopologyInfo::TypeStringBimap::relation >
		//(VirtualTopologyInfo::Type::Null, L"Euclidean")
		(VirtualTopologyInfo::Type::CubePlane, L"Cube")
		(VirtualTopologyInfo::Type::SphereNonZeroRadius, L"Sphere")
		(VirtualTopologyInfo::Type::Circle, L"Torus")
		(VirtualTopologyInfo::Type::CylinderSide, L"Cylinder")
		(VirtualTopologyInfo::Type::LinePin, L"Pin")
		(VirtualTopologyInfo::Type::LineRod, L"Rod")
		(VirtualTopologyInfo::Type::SphereZeroRadius, L"Point");

	VirtualTopologyInfo::VirtualTopologyInfo()
	{
	}


	VirtualTopologyInfo::~VirtualTopologyInfo()
	{
	}

	VirtualTopologyTranslator::VirtualTopologyTranslator() {


	}

	boost::optional<VirtualTopologyInfo::Type> VirtualTopologyTranslator::get_value(std::wstring const &v) {

		VirtualTopologyInfo::TypeStringBimap::right_map::const_iterator iT = VirtualTopologyInfo::s_virtualTopologyNames.right.find(v);

		if (iT == VirtualTopologyInfo::s_virtualTopologyNames.right.end()) {

			return boost::none;
		}
		else {

			return iT->second;
		}
	}

	boost::optional<std::wstring> VirtualTopologyTranslator::put_value(VirtualTopologyInfo::Type const& v) {

		return VirtualTopologyInfo::s_virtualTopologyNames.left.at(v);
	}

} //namespace SynGlyphX