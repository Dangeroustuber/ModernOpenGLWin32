@ECHO OFF
IF "glslangValidator"=="" ECHO glslangValidator is NOT defined in PATH
glslangValidator -o spirv/triangle_shader.vert.spv -V --aml glsl/triangle_shader.vert || ECHO: && ECHO UNABLE TO COMPILE VERTEX SHADER TO SPIRV   && ECHO:
glslangValidator -o spirv/triangle_shader.frag.spv -V --aml glsl/triangle_shader.frag || ECHO: && ECHO UNABLE TO COMPILE FRAGMENT SHADER TO SPIRV && ECHO: