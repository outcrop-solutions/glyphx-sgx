#include "nonmappablegeometryproperties.h"

namespace SynGlyphX {

	NonMappableGeometryProperties::NonMappableGeometryProperties(GlyphGeometryInfo::Surface surface , double torusRatio ) :
		m_surface(surface),
		m_torusRatio(torusRatio)
	{
	}

	NonMappableGeometryProperties::NonMappableGeometryProperties(const NonMappableGeometryProperties& properties) :
		m_surface(properties.m_surface),
		m_torusRatio(properties.m_torusRatio) {


	}

	NonMappableGeometryProperties::~NonMappableGeometryProperties()
	{
	}

	NonMappableGeometryProperties& NonMappableGeometryProperties::operator=(const NonMappableGeometryProperties& properties) {

		m_surface = properties.m_surface;
		m_torusRatio = properties.m_torusRatio;

		return *this;
	}

	bool NonMappableGeometryProperties::operator==(const NonMappableGeometryProperties& properties) const {

		if (m_surface != properties.m_surface) {

			return false;
		}

		if (m_torusRatio != properties.m_torusRatio) {

			return false;
		}

		return true;
	}

	bool NonMappableGeometryProperties::operator!=(const NonMappableGeometryProperties& properties) const {

		return !operator==(properties);
	}

	GlyphGeometryInfo::Surface NonMappableGeometryProperties::GetSurface() const {

		return m_surface;
	}

	void NonMappableGeometryProperties::SetSurface(GlyphGeometryInfo::Surface surface) {

		m_surface = surface;
	}

	double NonMappableGeometryProperties::GetTorusRatio() const {

		return m_torusRatio;
	}

	void NonMappableGeometryProperties::SetTorusRatio(double torusRatio) {

		m_torusRatio = torusRatio;
	}

} //namespace SynGlyphX