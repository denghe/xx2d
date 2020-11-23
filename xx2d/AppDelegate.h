#pragma once
#include "xx_ptr.h"
//#define NOMINMAX
//#include <Windows.h>
#include "glew/glew.h"
#include "glfw/glfw3.h"
//#include "glad/glad.h"
#include "Structs.h"
#include <stack>

struct AppDelegate {
	AppDelegate(AppDelegate const&) = delete;
	AppDelegate& operator=(AppDelegate const&) = delete;
	AppDelegate();
	~AppDelegate();
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

	int lastErrorNumber = 0;
	std::string lastErrorMessage;

	int width = 0, height = 0;
	bool fullScreen = false;
	double beginTime = 0;
	double lastTime = 0;
	double delta = 0;
	uint64_t numFrames = 0;
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

	int64_t _drawnBatches;
	int64_t _drawnVertices;

	GLFWmonitor* _monitor = nullptr;
	GLFWwindow* _window = nullptr;
};

inline xx::Shared<AppDelegate> gAppDelegate;
