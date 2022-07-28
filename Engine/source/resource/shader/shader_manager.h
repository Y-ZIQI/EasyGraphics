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

		const char* shading_vert_path = "../Engine/shaders/compiled/shading_vert.spv";
		const char* shading_frag_path = "../Engine/shaders/compiled/shading_frag.spv";

		const char* tonemapping_vert_path = "../Engine/shaders/compiled/tonemapping_vert.spv";
		const char* tonemapping_frag_path = "../Engine/shaders/compiled/tonemapping_frag.spv";

		const char* shadow_vert_path = "../Engine/shaders/compiled/shadow_vert.spv";
		const char* shadow_frag_path = "../Engine/shaders/compiled/shadow_frag.spv";

		const char* blur_vert_path = "../Engine/shaders/compiled/blur_vert.spv";
		const char* blur_frag_path = "../Engine/shaders/compiled/blur_frag.spv";
	};
}