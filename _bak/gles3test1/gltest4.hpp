#pragma once


#include "glmanager.hpp"
#include "esMatrix.h"
#include <DirectXMath.h>
using namespace DirectX;
#include <memory>
#ifdef NDEBUG
#include <omp.h>
#endif

inline int esGenCube(float scale, GLfloat** vertices, GLfloat** normals,
	GLfloat** texCoords, GLuint** indices)
{
	int i;
	int numVertices = 24;
	int numIndices = 36;

	GLfloat cubeVerts[] =
	{
	   -0.5f, -0.5f, -0.5f,
	   -0.5f, -0.5f,  0.5f,
	   0.5f, -0.5f,  0.5f,
	   0.5f, -0.5f, -0.5f,
	   -0.5f,  0.5f, -0.5f,
	   -0.5f,  0.5f,  0.5f,
	   0.5f,  0.5f,  0.5f,
	   0.5f,  0.5f, -0.5f,
	   -0.5f, -0.5f, -0.5f,
	   -0.5f,  0.5f, -0.5f,
	   0.5f,  0.5f, -0.5f,
	   0.5f, -0.5f, -0.5f,
	   -0.5f, -0.5f, 0.5f,
	   -0.5f,  0.5f, 0.5f,
	   0.5f,  0.5f, 0.5f,
	   0.5f, -0.5f, 0.5f,
	   -0.5f, -0.5f, -0.5f,
	   -0.5f, -0.5f,  0.5f,
	   -0.5f,  0.5f,  0.5f,
	   -0.5f,  0.5f, -0.5f,
	   0.5f, -0.5f, -0.5f,
	   0.5f, -0.5f,  0.5f,
	   0.5f,  0.5f,  0.5f,
	   0.5f,  0.5f, -0.5f,
	};

	GLfloat cubeNormals[] =
	{
	   0.0f, -1.0f, 0.0f,
	   0.0f, -1.0f, 0.0f,
	   0.0f, -1.0f, 0.0f,
	   0.0f, -1.0f, 0.0f,
	   0.0f, 1.0f, 0.0f,
	   0.0f, 1.0f, 0.0f,
	   0.0f, 1.0f, 0.0f,
	   0.0f, 1.0f, 0.0f,
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, 1.0f,
	   0.0f, 0.0f, 1.0f,
	   0.0f, 0.0f, 1.0f,
	   0.0f, 0.0f, 1.0f,
	   -1.0f, 0.0f, 0.0f,
	   -1.0f, 0.0f, 0.0f,
	   -1.0f, 0.0f, 0.0f,
	   -1.0f, 0.0f, 0.0f,
	   1.0f, 0.0f, 0.0f,
	   1.0f, 0.0f, 0.0f,
	   1.0f, 0.0f, 0.0f,
	   1.0f, 0.0f, 0.0f,
	};

	GLfloat cubeTex[] =
	{
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   1.0f, 0.0f,
	   1.0f, 1.0f,
	   0.0f, 1.0f,
	   0.0f, 0.0f,
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	   0.0f, 0.0f,
	   0.0f, 1.0f,
	   1.0f, 1.0f,
	   1.0f, 0.0f,
	};

	// Allocate memory for buffers
	if (vertices != nullptr)
	{
		*vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * numVertices);
		memcpy(*vertices, cubeVerts, sizeof(cubeVerts));

		for (i = 0; i < numVertices * 3; i++)
		{
			(*vertices)[i] *= scale;
		}
	}

	if (normals != nullptr)
	{
		*normals = (GLfloat*)malloc(sizeof(GLfloat) * 3 * numVertices);
		memcpy(*normals, cubeNormals, sizeof(cubeNormals));
	}

	if (texCoords != nullptr)
	{
		*texCoords = (GLfloat*)malloc(sizeof(GLfloat) * 2 * numVertices);
		memcpy(*texCoords, cubeTex, sizeof(cubeTex));
	}


	// Generate the indices
	if (indices != nullptr)
	{
		GLuint cubeIndices[] =
		{
		   0, 2, 1,
		   0, 3, 2,
		   4, 5, 6,
		   4, 6, 7,
		   8, 9, 10,
		   8, 10, 11,
		   12, 15, 14,
		   12, 14, 13,
		   16, 17, 18,
		   16, 18, 19,
		   20, 23, 22,
		   20, 22, 21
		};

		*indices = (GLuint*)malloc(sizeof(GLuint) * numIndices);
		memcpy(*indices, cubeIndices, sizeof(cubeIndices));
	}

	return numIndices;
}

#ifdef _WIN32
#define srandom srand
#define random rand
#endif

#define NUM_INSTANCES   1000000
#define POSITION_LOC    0
#define COLOR_LOC       1
#define MVP_LOC         2

struct GLTest4
{
	inline static var vsSrc = R"--(
#version 300 es
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in mat4 aMvpMatrix;
out vec4 vColor;
void main()
{
   vColor = aColor;
   gl_Position = aMvpMatrix * aPosition;
}
)--";

	inline static var fsSrc = R"--(
#version 300 es
precision mediump float;
in vec4 vColor;
layout(location = 0) out vec4 oColor;
void main()
{
   oColor = vColor;
}
)--";

	GLManager sm;
	std::string lastErrorMessage;
	GLuint program = 0;
	// VBOs
	GLuint positionVBO;
	GLuint colorVBO;
	GLuint mvpVBO;
	GLuint indicesIBO;

	int       numIndices;

	std::unique_ptr<GLfloat[]>   angle = std::unique_ptr<GLfloat[]>(new GLfloat[NUM_INSTANCES]);


	GLTest4()
	{
		var vs = sm.LoadShader(GL_VERTEX_SHADER, vsSrc);
		assert(vs);
		var fs = sm.LoadShader(GL_FRAGMENT_SHADER, fsSrc);
		assert(fs);
		program = sm.LinkProgram(vs, fs);
		assert(program);

		GLfloat* positions;
		GLuint* indices;

		numIndices = esGenCube(0.01f, &positions, NULL, NULL, &indices);

		// Index buffer object
		glGenBuffers(1, &indicesIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		free(indices);

		// Position VBO for cube model
		glGenBuffers(1, &positionVBO);
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat) * 3, positions, GL_STATIC_DRAW);
		free(positions);

		// Random color for each instance
		{
			std::unique_ptr<GLubyte[][4]> colors(new GLubyte[NUM_INSTANCES][4]);
			int instance;

			srandom(0);

			for (instance = 0; instance < NUM_INSTANCES; instance++)
			{
				colors[instance][0] = random() % 255;
				colors[instance][1] = random() % 255;
				colors[instance][2] = random() % 255;
				colors[instance][3] = 0;
			}

			glGenBuffers(1, &colorVBO);
			glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
			glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * 4, (void*)colors.get(), GL_STATIC_DRAW);
		}

		// Allocate storage to store MVP per instance
		{
			int instance;

			// Random angle for each instance, compute the MVP later
			for (instance = 0; instance < NUM_INSTANCES; instance++)
			{
				angle[instance] = (float)(random() % 32768) / 32767.0f * 360.0f;
			}

			glGenBuffers(1, &mvpVBO);
			glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
			glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * sizeof(XMMATRIX), NULL, GL_DYNAMIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	inline void Update(int const& width, int const& height, float deltaTime) noexcept
	{
		XMMATRIX* matrixBuf;
		XMMATRIX perspective;
		float    aspect;
		int      instance = 0;
		int      numRows;
		int      numColumns;


		// Compute the window aspect ratio
		aspect = (GLfloat)width / (GLfloat)height;

		// Generate a perspective matrix with a 60 degree FOV
		esMatrixLoadIdentity((ESMatrix*)&perspective);
		//esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);
		esOrtho((ESMatrix*)&perspective, -aspect, aspect, -1, 1, -10, 10);


		glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
		matrixBuf = (XMMATRIX*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(XMMATRIX) * NUM_INSTANCES, GL_MAP_WRITE_BIT);

		// Compute a per-instance MVP that translates and rotates each instance differnetly
		numRows = (int)sqrtf(NUM_INSTANCES);
		numColumns = numRows;

		#pragma omp parallel for
		for (instance = 0; instance < NUM_INSTANCES; instance++)
		{
			XMMATRIX modelview;
			float translateX = ((float)(instance % numRows) / (float)numRows) * 2.0f - 1.0f;
			float translateY = ((float)(instance / numColumns) / (float)numColumns) * 2.0f - 1.0f;

			// Generate a model view matrix to rotate/translate the cube
			//esMatrixLoadIdentity((ESMatrix*)&modelview);

			// Per-instance translation
			//esTranslate((ESMatrix*)&modelview, translateX, translateY, -2.0f);
			modelview = XMMatrixTranslation(translateX, translateY, -2.0f);

			// Compute a rotation angle based on time to rotate the cube
			angle[instance] += (deltaTime * 40.0f);

			if (angle[instance] >= 360.0f)
			{
				angle[instance] -= 360.0f;
			}

			// Rotate the cube
			//esRotate((ESMatrix*)&modelview, angle[instance], 1.0, 0.0, 1.0);

			//{
			//	GLfloat a = angle[instance], x = 1.0f, y = 0.0f, z = 1.0f;
			//	GLfloat sinAngle, cosAngle;
			//	GLfloat mag = sqrtf(x * x + y * y + z * z);

			//	//sinAngle = sinf(a * PI / 180.0f);
			//	//cosAngle = cosf(a * PI / 180.0f);
			//	XMScalarSinCos(&sinAngle, &cosAngle, a);

			//	if (mag > 0.0f) {
			//		GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
			//		GLfloat oneMinusCos;
			//		XMMATRIX rotMat_;
			//		ESMatrix& rotMat = *(ESMatrix*)&rotMat_;

			//		x /= mag;
			//		y /= mag;
			//		z /= mag;

			//		xx = x * x;
			//		yy = y * y;
			//		zz = z * z;
			//		xy = x * y;
			//		yz = y * z;
			//		zx = z * x;
			//		xs = x * sinAngle;
			//		ys = y * sinAngle;
			//		zs = z * sinAngle;
			//		oneMinusCos = 1.0f - cosAngle;

			//		rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
			//		rotMat.m[0][1] = (oneMinusCos * xy) - zs;
			//		rotMat.m[0][2] = (oneMinusCos * zx) + ys;
			//		rotMat.m[0][3] = 0.0F;
			//			   
			//		rotMat.m[1][0] = (oneMinusCos * xy) + zs;
			//		rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
			//		rotMat.m[1][2] = (oneMinusCos * yz) - xs;
			//		rotMat.m[1][3] = 0.0F;
			//			   
			//		rotMat.m[2][0] = (oneMinusCos * zx) - ys;
			//		rotMat.m[2][1] = (oneMinusCos * yz) + xs;
			//		rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
			//		rotMat.m[2][3] = 0.0F;
			//			   
			//		rotMat.m[3][0] = 0.0F;
			//		rotMat.m[3][1] = 0.0F;
			//		rotMat.m[3][2] = 0.0F;
			//		rotMat.m[3][3] = 1.0F;

			//		modelview = XMMatrixMultiply(rotMat_, modelview);
			//	}
			//}


			// Compute the final MVP by multiplying the
			// modevleiw and perspective matrices together
			//esMatrixMultiply((ESMatrix*)&matrixBuf[instance], (ESMatrix*)&modelview, (ESMatrix*)&perspective);
			matrixBuf[instance] = XMMatrixMultiply(modelview, perspective);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);



		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);

		// Load the vertex position
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT,
			GL_FALSE, 3 * sizeof(GLfloat), (const void*)NULL);
		glEnableVertexAttribArray(POSITION_LOC);

		// Load the instance color buffer
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glVertexAttribPointer(COLOR_LOC, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(GLubyte), (const void*)NULL);
		glEnableVertexAttribArray(COLOR_LOC);
		glVertexAttribDivisor(COLOR_LOC, 1); // One color per instance


		// Load the instance MVP buffer
		glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);

		// Load each matrix row of the MVP.  Each row gets an increasing attribute location.
		glVertexAttribPointer(MVP_LOC + 0, 4, GL_FLOAT, GL_FALSE, sizeof(XMMATRIX), (const void*)NULL);
		glVertexAttribPointer(MVP_LOC + 1, 4, GL_FLOAT, GL_FALSE, sizeof(XMMATRIX), (const void*)(sizeof(GLfloat) * 4));
		glVertexAttribPointer(MVP_LOC + 2, 4, GL_FLOAT, GL_FALSE, sizeof(XMMATRIX), (const void*)(sizeof(GLfloat) * 8));
		glVertexAttribPointer(MVP_LOC + 3, 4, GL_FLOAT, GL_FALSE, sizeof(XMMATRIX), (const void*)(sizeof(GLfloat) * 12));
		glEnableVertexAttribArray(MVP_LOC + 0);
		glEnableVertexAttribArray(MVP_LOC + 1);
		glEnableVertexAttribArray(MVP_LOC + 2);
		glEnableVertexAttribArray(MVP_LOC + 3);

		// One MVP per instance
		glVertexAttribDivisor(MVP_LOC + 0, 1);
		glVertexAttribDivisor(MVP_LOC + 1, 1);
		glVertexAttribDivisor(MVP_LOC + 2, 1);
		glVertexAttribDivisor(MVP_LOC + 3, 1);

		// Bind the index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesIBO);

		// Draw the cubes
		glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (const void*)NULL, NUM_INSTANCES);
	}
};
