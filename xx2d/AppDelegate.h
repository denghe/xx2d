#pragma once
#include "xx_ptr.h"
//#define NOMINMAX
//#include <Windows.h>
#include "glew/glew.h"
#include "glfw/glfw3.h"
//#include "glad/glad.h"
#include "Structs.h"
#include <stack>

/**Enum the preallocated vertex attribute. */
enum
{
	/**Index 0 will be used as Position.*/
	VERTEX_ATTRIB_POSITION,
	/**Index 1 will be used as Color.*/
	VERTEX_ATTRIB_COLOR,
	/**Index 2 will be used as Tex coord unit 0.*/
	VERTEX_ATTRIB_TEX_COORD,
	/**Index 3 will be used as Tex coord unit 1.*/
	VERTEX_ATTRIB_TEX_COORD1,
	/**Index 4 will be used as Tex coord unit 2.*/
	VERTEX_ATTRIB_TEX_COORD2,
	/**Index 5 will be used as Tex coord unit 3.*/
	VERTEX_ATTRIB_TEX_COORD3,
	/**Index 6 will be used as Normal.*/
	VERTEX_ATTRIB_NORMAL,
	/**Index 7 will be used as Blend weight for hardware skin.*/
	VERTEX_ATTRIB_BLEND_WEIGHT,
	/**Index 8 will be used as Blend index.*/
	VERTEX_ATTRIB_BLEND_INDEX,
	/**Index 9 will be used as tangent.*/
	VERTEX_ATTRIB_TANGENT,
	/**Index 10 will be used as Binormal.*/
	VERTEX_ATTRIB_BINORMAL,
	VERTEX_ATTRIB_MAX,

	// backward compatibility
	VERTEX_ATTRIB_TEX_COORDS = VERTEX_ATTRIB_TEX_COORD,
};

struct AppDelegate {
	AppDelegate();
	AppDelegate(AppDelegate const&) = delete;
	AppDelegate& operator=(AppDelegate const&) = delete;
	virtual ~AppDelegate();
	virtual int Init();
	int Run(int const& width, int const& height, bool const& fullScreen);

	void onGLFWError(int errorID, const char* errorDesc);
	void onGLFWMouseCallBack(int button, int action, int modify);
	void onGLFWMouseMoveCallBack(double x, double y);
	void onGLFWMouseScrollCallback(double x, double y);
	void onGLFWKeyCallback(int key, int scancode, int action, int mods);
	void onGLFWCharCallback(unsigned int character);
	void onGLFWWindowPosCallback(int x, int y);
	void onGLFWframebuffersize(int w, int h);
	void onGLFWWindowSizeFunCallback(int width, int height);
	void onGLFWWindowIconifyCallback(int iconified);
	void onGLFWWindowFocusCallback(int focused);

	GLFWmonitor* monitor = nullptr;
	GLFWwindow* window = nullptr;

	int lastErrorNumber = 0;
	std::string lastErrorMessage;

	int width = 0, height = 0;
	bool fullScreen = false;
	double delta = 0;
	std::filesystem::path resRootPath;

	static const int VBO_SIZE = 65536;
	static const int INDEX_VBO_SIZE = VBO_SIZE * 6 / 4;
	static const int BATCH_TRIAGCOMMAND_RESERVED_SIZE = 64;
	static const int MATERIAL_ID_DO_NOT_BATCH = 0;

	V3F_C4B_T2F _verts[VBO_SIZE];
	GLushort _indices[INDEX_VBO_SIZE];
	GLuint _buffersVAO;
	GLuint _buffersVBO[2]; //0: vertex  1: indices

	std::stack<Mat4> _modelViewMatrixStack;
	std::vector< std::stack<Mat4> > _projectionMatrixStackList;
	std::stack<Mat4> _textureMatrixStack;
};

inline xx::Shared<AppDelegate> gAppDelegate;
