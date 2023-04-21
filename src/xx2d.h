#pragma once
#pragma execution_character_set( "utf-8" )

#include <xx_helpers.h>
#include <xx_file.h>
#include <xx_ptr.h>
#include <xx_coro_simple.h>	// xx_listlink.h
#include <xx_listdoublelink.h>
#ifndef __EMSCRIPTEN__
#include <xx_threadpool.h>
#endif

#include "xx2d_rnd.h"

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#include <emscripten/html5.h>
	#include <GLES3/gl3.h>
#else
#  ifdef __ANDROID__
	#include <EGL/egl.h>
	#include <GLES3/gl3.h>
#  else
	#include <glad.h>
#  endif
#endif
#include "xx2d_gl.h"

#include "xx2d_enums.h"
#include "xx2d_prims.h"
#include "xx2d_animframe.h"
#include "xx2d_utils.h"
#include "xx2d_calc.h"
#include "xx2d_shaders.h"
#include "xx2d_tp.h"
#include "xx2d_tmx.h"
#include "xx2d_bmfont.h"
#include "xx2d_engine.h"
#include "xx2d_event_listeners.h"
#include "xx2d_sprite.h"
#include "xx2d_label.h"
#include "xx2d_linestrip.h"
#include "xx2d_tmx_ex.h"
#include "xx2d_spacegrid.h"
#include "xx2d_spacegridab.h"
#include "xx2d_nodes.h"
#include "xx2d_quad.h"
#include "xx2d_movepath.h"
#include "xx2d_polygon_sprite.h"
#include "xx2d_simple_label.h"
#include "xx2d_gamelooper.h"
#include "xx2d_framebuffer.h"
#include "xx2d_particle.h"
#include "xx2d_mv.h"

#ifndef __EMSCRIPTEN__
#include "xx2d_audio.h"

#include "xx2d_imgui.h"
#endif
// ...
