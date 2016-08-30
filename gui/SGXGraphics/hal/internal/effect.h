
#pragma once

#include "types_internal.h"
#include "ref_counted.h"
#include <string>
#include <hash_map>
#include <hash_set>

namespace SynGlyphX
{
	namespace hal
	{
		class effect : public ref_counted
		{
		public:
			static effect* create( const char* vs_src, const char* gs_src, const char* ps_src, const std::vector<std::string>& forced_header_includes );
			~effect();

			void recompile();
			bool compiled() { return compile_success; }

			GLuint program() const { return _program; }

			void set_uniform( const char* block_name, const char* uniform_name, const void* data, size_t size );
			void set_uniform_block( const char* block_name, const void* data, size_t size );
			void update_uniforms();

			void set_uniform_block_external( const char* block_name );
			unsigned int get_uniform_block_index( const char* block_name );

			void bind();

		private:
			effect();
			effect( const effect& ) = delete;
			void release_internals();

			std::string vs_src, gs_src, ps_src;
			GLuint _program;
			GLuint vs, gs, ps;

			struct uniform_block
			{
				GLuint index;
				GLint size;
				GLuint buffer;
				GLuint binding_pt;
				char* backing_buffer;
				bool dirty, external;
			};

			std::hash_map<std::string, uniform_block> uniform_blocks;
			std::vector<std::string> forced_includes;
			std::hash_set<std::string> external_uniform_block_names;

			void setup_external_uniform_blocks();

			static void compile( effect* e );
			bool compile_success;

			void setup_uniform_blocks();
		};
	}
}

