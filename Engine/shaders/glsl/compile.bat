set glslc=D:/include/VulkanSDK/1.3.211.0/Bin/glslc.exe
::set glslc=C:\VulkanSDK\1.3.216.0\Bin\glslc.exe

::call %glslc% triangle.vert -o ../compiled/vert.spv
::call %glslc% triangle.frag -o ../compiled/frag.spv

call %glslc% draw_gbuffer.vert -o ../compiled/draw_gbuffer_vert.spv
call %glslc% draw_gbuffer.frag -o ../compiled/draw_gbuffer_frag.spv

call %glslc% shading.vert -o ../compiled/shading_vert.spv
call %glslc% shading.frag -o ../compiled/shading_frag.spv

call %glslc% tonemapping.vert -o ../compiled/tonemapping_vert.spv
call %glslc% tonemapping.frag -o ../compiled/tonemapping_frag.spv

call %glslc% shadow.vert -o ../compiled/shadow_vert.spv
call %glslc% shadow.frag -o ../compiled/shadow_frag.spv