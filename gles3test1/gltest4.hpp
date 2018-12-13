#pragma once

#include "glmanager.hpp"

typedef struct
{
	GLfloat   m[4][4];
} ESMatrix;

inline int esGenCube(float scale, GLfloat **vertices, GLfloat **normals,
	GLfloat **texCoords, GLuint **indices)
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

inline void esMatrixLoadIdentity(ESMatrix *result)
{
	memset(result, 0x0, sizeof(ESMatrix));
	result->m[0][0] = 1.0f;
	result->m[1][1] = 1.0f;
	result->m[2][2] = 1.0f;
	result->m[3][3] = 1.0f;
}

#define PI 3.1415926535897932384626433832795f

inline void esMatrixMultiply(ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB)
{
	ESMatrix    tmp;
	int         i;

	for (i = 0; i < 4; i++)
	{
		tmp.m[i][0] = (srcA->m[i][0] * srcB->m[0][0]) +
			(srcA->m[i][1] * srcB->m[1][0]) +
			(srcA->m[i][2] * srcB->m[2][0]) +
			(srcA->m[i][3] * srcB->m[3][0]);

		tmp.m[i][1] = (srcA->m[i][0] * srcB->m[0][1]) +
			(srcA->m[i][1] * srcB->m[1][1]) +
			(srcA->m[i][2] * srcB->m[2][1]) +
			(srcA->m[i][3] * srcB->m[3][1]);

		tmp.m[i][2] = (srcA->m[i][0] * srcB->m[0][2]) +
			(srcA->m[i][1] * srcB->m[1][2]) +
			(srcA->m[i][2] * srcB->m[2][2]) +
			(srcA->m[i][3] * srcB->m[3][2]);

		tmp.m[i][3] = (srcA->m[i][0] * srcB->m[0][3]) +
			(srcA->m[i][1] * srcB->m[1][3]) +
			(srcA->m[i][2] * srcB->m[2][3]) +
			(srcA->m[i][3] * srcB->m[3][3]);
	}

	memcpy(result, &tmp, sizeof(ESMatrix));
}

inline void esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
	float       deltaX = right - left;
	float       deltaY = top - bottom;
	float       deltaZ = farZ - nearZ;
	ESMatrix    frust;

	if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
		(deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
	{
		return;
	}

	frust.m[0][0] = 2.0f * nearZ / deltaX;
	frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

	frust.m[1][1] = 2.0f * nearZ / deltaY;
	frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

	frust.m[2][0] = (right + left) / deltaX;
	frust.m[2][1] = (top + bottom) / deltaY;
	frust.m[2][2] = -(nearZ + farZ) / deltaZ;
	frust.m[2][3] = -1.0f;

	frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
	frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

	esMatrixMultiply(result, &frust, result);
}

inline void esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ)
{
	GLfloat frustumW, frustumH;

	frustumH = tanf(fovy / 360.0f * PI) * nearZ;
	frustumW = frustumH * aspect;

	esFrustum(result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

inline void esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz)
{
	result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
	result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
	result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
	result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

inline void esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat sinAngle, cosAngle;
	GLfloat mag = sqrtf(x * x + y * y + z * z);

	sinAngle = sinf(angle * PI / 180.0f);
	cosAngle = cosf(angle * PI / 180.0f);

	if (mag > 0.0f)
	{
		GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
		GLfloat oneMinusCos;
		ESMatrix rotMat;

		x /= mag;
		y /= mag;
		z /= mag;

		xx = x * x;
		yy = y * y;
		zz = z * z;
		xy = x * y;
		yz = y * z;
		zx = z * x;
		xs = x * sinAngle;
		ys = y * sinAngle;
		zs = z * sinAngle;
		oneMinusCos = 1.0f - cosAngle;

		rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
		rotMat.m[0][1] = (oneMinusCos * xy) - zs;
		rotMat.m[0][2] = (oneMinusCos * zx) + ys;
		rotMat.m[0][3] = 0.0F;

		rotMat.m[1][0] = (oneMinusCos * xy) + zs;
		rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
		rotMat.m[1][2] = (oneMinusCos * yz) - xs;
		rotMat.m[1][3] = 0.0F;

		rotMat.m[2][0] = (oneMinusCos * zx) - ys;
		rotMat.m[2][1] = (oneMinusCos * yz) + xs;
		rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
		rotMat.m[2][3] = 0.0F;

		rotMat.m[3][0] = 0.0F;
		rotMat.m[3][1] = 0.0F;
		rotMat.m[3][2] = 0.0F;
		rotMat.m[3][3] = 1.0F;

		esMatrixMultiply(result, &rotMat, result);
	}
}


#ifdef _WIN32
#define srandom srand
#define random rand
#endif

#define NUM_INSTANCES   100
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

	GLfloat   angle[NUM_INSTANCES];


	GLTest4()
	{
		var vs = sm.LoadShader(GL_VERTEX_SHADER, vsSrc);
		assert(vs);
		var fs = sm.LoadShader(GL_FRAGMENT_SHADER, fsSrc);
		assert(fs);
		program = sm.LinkProgram(vs, fs);
		assert(program);

		GLfloat *positions;
		GLuint *indices;

		numIndices = esGenCube(0.1f, &positions, NULL, NULL, &indices);

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
			GLubyte colors[NUM_INSTANCES][4];
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
			glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * 4, colors, GL_STATIC_DRAW);
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
			glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * sizeof(ESMatrix), NULL, GL_DYNAMIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	}

	inline void Update(int const& width, int const& height, float deltaTime) noexcept
	{
		ESMatrix *matrixBuf;
		ESMatrix perspective;
		float    aspect;
		int      instance = 0;
		int      numRows;
		int      numColumns;


		// Compute the window aspect ratio
		aspect = (GLfloat)width / (GLfloat)height;

		// Generate a perspective matrix with a 60 degree FOV
		esMatrixLoadIdentity(&perspective);
		esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);

		glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);
		matrixBuf = (ESMatrix *)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(ESMatrix) * NUM_INSTANCES, GL_MAP_WRITE_BIT);

		// Compute a per-instance MVP that translates and rotates each instance differnetly
		numRows = (int)sqrtf(NUM_INSTANCES);
		numColumns = numRows;

		for (instance = 0; instance < NUM_INSTANCES; instance++)
		{
			ESMatrix modelview;
			float translateX = ((float)(instance % numRows) / (float)numRows) * 2.0f - 1.0f;
			float translateY = ((float)(instance / numColumns) / (float)numColumns) * 2.0f - 1.0f;

			// Generate a model view matrix to rotate/translate the cube
			esMatrixLoadIdentity(&modelview);

			// Per-instance translation
			esTranslate(&modelview, translateX, translateY, -2.0f);

			// Compute a rotation angle based on time to rotate the cube
			angle[instance] += (deltaTime * 40.0f);

			if (angle[instance] >= 360.0f)
			{
				angle[instance] -= 360.0f;
			}

			// Rotate the cube
			esRotate(&modelview, angle[instance], 1.0, 0.0, 1.0);

			// Compute the final MVP by multiplying the
			// modevleiw and perspective matrices together
			esMatrixMultiply(&matrixBuf[instance], &modelview, &perspective);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);



		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);

		// Load the vertex position
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT,
			GL_FALSE, 3 * sizeof(GLfloat), (const void *)NULL);
		glEnableVertexAttribArray(POSITION_LOC);

		// Load the instance color buffer
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glVertexAttribPointer(COLOR_LOC, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(GLubyte), (const void *)NULL);
		glEnableVertexAttribArray(COLOR_LOC);
		glVertexAttribDivisor(COLOR_LOC, 1); // One color per instance


		// Load the instance MVP buffer
		glBindBuffer(GL_ARRAY_BUFFER, mvpVBO);

		// Load each matrix row of the MVP.  Each row gets an increasing attribute location.
		glVertexAttribPointer(MVP_LOC + 0, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)NULL);
		glVertexAttribPointer(MVP_LOC + 1, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 4));
		glVertexAttribPointer(MVP_LOC + 2, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 8));
		glVertexAttribPointer(MVP_LOC + 3, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 12));
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
		glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (const void *)NULL, NUM_INSTANCES);
	}
};
