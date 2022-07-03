set glslc=D:/include/VulkanSDK/1.3.211.0/Bin/glslc.exe

call %glslc% triangle.vert -o ../compiled/vert.spv
call %glslc% triangle.frag -o ../compiled/frag.spv

call %glslc% draw_gbuffer.vert -o ../compiled/draw_gbuffer_vert.spv
call %glslc% draw_gbuffer.frag -o ../compiled/draw_gbuffer_frag.spv