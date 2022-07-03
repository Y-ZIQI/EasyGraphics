#pragma once

namespace Eagle
{
	class ShaderManager
	{
	public:
		const char* vert_shader_path = "../Engine/shaders/compiled/vert.spv";
		const char* frag_shader_path = "../Engine/shaders/compiled/frag.spv";

		const char* draw_gbuffer_vert_path = "../Engine/shaders/compiled/draw_gbuffer_vert.spv";
		const char* draw_gbuffer_frag_path = "../Engine/shaders/compiled/draw_gbuffer_frag.spv";
	};
}