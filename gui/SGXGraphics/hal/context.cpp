
#include "context.h"

namespace SynGlyphX
{
	namespace hal
	{
		void context::reset_defaults()
		{
			reset_defaults_internal();
			unbind_all_textures();
			bind( ( render_target_set* )nullptr );
			set_depth_state( depth_state::disabled );
			set_blend_state( blend_state::disabled );

			rasterizer_state rast;
			rast.cull_backfaces = false;
			rast.multisample = false;
			rast.wireframe = false;
			set_rasterizer_state( rast );

		}
	}
}
