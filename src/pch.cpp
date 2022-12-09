#include "pch.h"

// todo: search path
// todo: texture cache
// todo: texture packer plist support
// todo: font & label
// todo: node
// todo: spine? webassembly? lua? 
// 
// todo: mouse, touch, keyboard, joystick support
// 
// todo: Program manager? ready for shader switch
//
// todo: draw instance for particle system?
// todo: texture array support?




// some opengl documents put here
/*
* glEnable
* glDisable
* gIisEnabled
*
GL_BLEND
GL_CULL_FACE
GL_DEPTH_TEST
GL_DITHER
GL_POLYGON_OFFSET_FILL
GL_PRIMITIVE_RESTART_FIXED_INDEX
GL_RASTERIZER_DISCARD
GL_SAMPLE_ALPHA_TO_COVERAGE
GL_SAMPLE_COVERAGE
GL_SCISSOR_TEST
GL_STENCIL_TEST

known issue：if enable depth test，will be alpha bug( can be fix by discard )
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_ALWAYS);//glDepthFunc(GL_LEQUAL);
glDepthMask(GL_TRUE);

// showly than glBufferData
//glBufferData(GL_ARRAY_BUFFER, sizeof(XYUVIJRGBA8) * maxVertNums, nullptr, GL_DYNAMIC_DRAW);
//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(XYUVIJRGBA8) * 4 * autoBatchNumQuads, verts);

*/
