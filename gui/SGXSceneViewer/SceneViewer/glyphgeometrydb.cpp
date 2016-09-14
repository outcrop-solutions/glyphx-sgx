
#include "glyphgeometrydb.h"
#include <render/model_loader.h>
#include <render/torus_generator.h>
#include "placementpolicy.h"

namespace SynGlyphX
{
	std::unordered_map<GlyphShape, std::vector<GlyphGeometryDB::model_entry>> GlyphGeometryDB::models;
	std::unordered_map<float, std::vector<GlyphGeometryDB::model_entry>> GlyphGeometryDB::torus_models;

	void GlyphGeometryDB::init()
	{
		clear();
		//add_model( GlyphShape::Sphere, "meshes/sphere_2.dae", FLT_MAX, glm::scale( glm::mat4(), glm::vec3( 0.5f ) ) );
		add_model( GlyphShape::Sphere, "meshes/sphere_2.dae", FLT_MAX);
		add_model( GlyphShape::Cube, "meshes/cube_0.dae", FLT_MAX, glm::scale(glm::mat4(), glm::vec3(std::sqrt(2.f) / 2.f )));
		add_model( GlyphShape::Cone, "meshes/cone_0.dae", FLT_MAX, glm::translate(glm::rotate(glm::mat4(), glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)), glm::vec3(0.f, 1.f, 0.f)) );
		add_model( GlyphShape::Icosahedron, "meshes/icosahedron_0.dae", FLT_MAX );
		add_model( GlyphShape::Cylinder, "meshes/cylinder_0.dae", FLT_MAX, glm::translate(glm::rotate( glm::mat4(), glm::half_pi<float>(), glm::vec3( 1.f, 0.f, 0.f ) ), glm::vec3(0.f, 1.f, 0.f) ) );
		add_model( GlyphShape::Octahedron, "meshes/octahedron_0.dae", FLT_MAX );
		add_model( GlyphShape::Dodecahedron, "meshes/dodecahedron_0.dae", FLT_MAX );
		add_model( GlyphShape::Tetrahedron, "meshes/tetrahedron_0.dae", FLT_MAX );
		add_model( GlyphShape::Pin, create_pin(), FLT_MAX);
		add_model( GlyphShape::Link_Cylinder, "meshes/link_cylinder.dae", FLT_MAX, glm::rotate( glm::mat4(), glm::half_pi<float>(), glm::vec3( 1.f, 0.f, 0.f ) ) );
		add_model( GlyphShape::Link_Cube, "meshes/link_cube.dae", FLT_MAX );
		add_model( GlyphShape::ErrorModel, "meshes/cube_0.dae", FLT_MAX );
	}

	void GlyphGeometryDB::reset()
	{
		// clear torus cache (next vis might not want the same ratios)
		for ( auto ml : torus_models )
			for ( auto m : ml.second )
				delete m.model;
		torus_models.clear();
	}

	void GlyphGeometryDB::clear()
	{
		reset();

		for ( auto ml : models )
			for ( auto m : ml.second )
				delete m.model;

		models.clear();
	}

	render::model* GlyphGeometryDB::get( const GlyphShape type, float torus_ratio, float dist )
	{
		if ( type == GlyphShape::Torus )
			return get_torus( torus_ratio, dist );

		for ( auto& entry : models[type] )
		{
			if ( dist < entry.lod )
				return entry.model;
		}

		// No model of appropriate LOD, so just return the lowest LOD available.
		if ( !models[type].empty() )
		{
			return models[type].rbegin()->model;
		}
		else
		{
			hal::debug::print( "Missing model type %i", int( type ) );
			return models[GlyphShape::ErrorModel].begin()->model;
		}
	}

	render::model* GlyphGeometryDB::get_torus( float ratio, float dist )
	{
		if ( torus_models.find( ratio ) == torus_models.end() )
		{
			// todo: handle LOD instead of just creating one fixed LOD per torus ratio
			add_torus_model( ratio, render::generate_torus( 1.5f, ratio * 1.5f, 8u, 16u, true ), FLT_MAX );

			// if this assert hits, we have a LOT of different torus ratios in this vis; should be avoided as it
			// can severely negatively impact performance
			const unsigned int torus_error_threshold = 32u;
			assert( torus_models.size() < torus_error_threshold );
		}

		for ( auto& entry : torus_models[ratio] )
		{
			if ( dist < entry.lod )
				return entry.model;
		}

		// No model of appropriate LOD, so just return the lowest LOD available.
		if ( !torus_models[ratio].empty() )
		{
			return torus_models[ratio].rbegin()->model;
		}
		else
		{
			// should be impossible to get here, since we'll have created a new model for ratios
			// we don't have already
			assert( false );
			return nullptr;
		}
	}

	void GlyphGeometryDB::add_model( GlyphShape type, const char* filename, float lod, const glm::mat4& base_transform )
	{
		render::model_loader_settings settings = render::model_loader_defaults();
		settings.support_readback = true;
		render::model* m = render::load_model( filename, settings );
		m->set_transform( base_transform );

		add_model( type, m, lod );
	}

	void GlyphGeometryDB::add_model( GlyphShape type, render::model* model, float lod )
	{
		model_entry e;
		e.model = model;
		e.lod = lod;
		assert( e.model );

		if ( models.find( type ) == models.end() )
			models.insert( std::make_pair( type, std::vector<model_entry>() ) );

		auto entry = models.find( type );
		entry->second.push_back( e );
	}

	void GlyphGeometryDB::add_torus_model( float ratio, render::model* model, float lod )
	{
		model_entry e;
		e.model = model;
		e.lod = lod;
		assert( e.model );

		if ( torus_models.find( ratio ) == torus_models.end() )
			torus_models.insert( std::make_pair( ratio, std::vector<model_entry>() ) );

		auto entry = torus_models.find( ratio );
		entry->second.push_back( e );
	}

	render::model* GlyphGeometryDB::create_pin()
	{
		// Load the sphere and cone models and grab their first (and only) meshes.
		render::model_loader_settings settings = render::model_loader_defaults();
		settings.support_readback = true;
		render::model* sphere = render::load_model( "meshes/sphere_2.dae", settings );
		render::model* cone = render::load_model( "meshes/cone_0.dae", settings );
		render::model_part* sphere_part = sphere->get_parts()[0];
		render::model_part* cone_part = cone->get_parts()[0];
		
		// Transform the sphere and cone here.
		//glm::mat4 sphere_transform = glm::translate(glm::rotate(glm::mat4(), glm::half_pi<float>(), glm::vec3(-1.f, 0.f, 0.f)), glm::vec3(0.f, 0.f, 5.f));
		glm::mat4 sphere_transform = glm::scale(glm::translate(glm::mat4(), glm::vec3(0.f, 0.f, 5.f)), glm::vec3(0.5));
		glm::mat4 cone_transform = glm::scale(glm::translate(glm::rotate(glm::mat4(), glm::half_pi<float>(), glm::vec3(-1.f, 0.f, 0.f)), glm::vec3(0.f, -2.5f, 0.f)), glm::vec3(0.5f, 2.5f, 0.5f));
		
		// Create a new pin model that combines them.
		render::model* pin = new render::model();
		render::model_part* part0 = new render::model_part( sphere_part->get_mesh(), sphere_transform * sphere_part->get_transform(), sphere_part->get_local_bound(), "pin-sphere" );
		render::model_part* part1 = new render::model_part( cone_part->get_mesh(), cone_transform * cone_part->get_transform(), cone_part->get_local_bound(), "pin-cone" );
		pin->add_part( part1 );
		pin->add_part( part0 );

		delete sphere;
		delete cone;
		return pin;
	}
}
