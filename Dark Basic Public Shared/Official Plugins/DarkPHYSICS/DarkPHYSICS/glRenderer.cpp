#define NOMINMAX
#include "glRenderer.h"
#include <math.h>
#include <stdio.h>

#if defined(WIN32) || defined(_XBOX)
#define strcasecmp stricmp 
#endif

static Renderer *instance = 0;

Renderer* Renderer::getInstance() 
{
	if (instance == NULL) 
		instance = new Renderer();
	return instance;
}

void Renderer::destroyInstance() 
{
	if (instance != 0) {
		delete instance;
	}
	instance = 0;
}


// ------------------------------------------------------------------------------------
struct RendererTexRef {
	char     filename[256];
	//GLuint   texID;
	int		texID;
	int      width, height;
};

//-----------------------------------------------------------------------
Renderer::Renderer()
{
}


//-----------------------------------------------------------------------
Renderer::~Renderer()
{
	freeTextures();
}

//-----------------------------------------------------------------------
void Renderer::clear()
{
    //glClear(GL_COLOR_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
}

//-----------------------------------------------------------------------
void Renderer::clearDepth()
{
	//glClear(GL_DEPTH_BUFFER_BIT);
}

//-----------------------------------------------------------------------
void Renderer::clearColor()
{
    //glClear(GL_COLOR_BUFFER_BIT);
}

//-----------------------------------------------------------------------
void Renderer::maskColor(bool red, bool green, bool blue)
{
	//GLboolean r = red ? GL_TRUE : GL_FALSE;
	//GLboolean g = green ? GL_TRUE : GL_FALSE;
	//GLboolean b = blue ? GL_TRUE : GL_FALSE;
	//glColorMask(r,g,b, GL_TRUE);
}

//-----------------------------------------------------------------------
void Renderer::lighting(bool on)
{
	/*
	if (on)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
	*/
}

//-----------------------------------------------------------------------
void Renderer::backColor(float *color)
{
	//glClearColor(color[0], color[1], color[2], 1.0f);
}

//-----------------------------------------------------------------------
void Renderer::wireframeMode(bool on)
{
	/*
	if (on)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	*/
}

// -----------------------------------------------------------------
void Renderer::project(NxVec3 &v, int &xi, int &yi, float &depth)
{
	/*
// OpenGL view transformation
//We cannot do picking easily on the xbox/PS3 anyway
#if defined(_XBOX) || defined (__CELLOS_LV2__)
	xi=yi=0;
	depth=0.0f;
#else
	GLint viewPort[4];
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

	GLdouble winX, winY, winZ;
	gluProject((GLdouble) v.x, (GLdouble) v.y, (GLdouble) v.z,
		modelMatrix, projMatrix, viewPort, &winX, &winY, &winZ);

	xi = (int)winX; yi = viewPort[3] - (int)winY - 1; depth = (float)winZ;
#endif
	*/
}


// ---------------------------------------------------------------------
void Renderer::unProject(int xi, int yi, float depth, NxVec3 &v)
{
	/*
	// OpenGL view transformation
//We cannot do picking easily on the xbox/PS3 anyway
#if defined(_XBOX) || defined (__CELLOS_LV2__)
	v=NxVec3(0.0f,0.0f,0.0f);
#else
	GLint viewPort[4];
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

	yi = viewPort[3] - yi - 1;
	GLdouble wx, wy, wz;
	gluUnProject((GLdouble) xi, (GLdouble) yi, (GLdouble) depth,
	modelMatrix, projMatrix, viewPort, &wx, &wy, &wz);
	v.set((float)wx, (float)wy, (float)wz);
#endif
	*/
}


// ---------------------------------------------------------------------
float Renderer::depthAt(int xi, int yi)
{
	/*
#if defined(_XBOX) || defined (__CELLOS_LV2__)
	return 0.0f;
#else
	// depth is taken from depth buffer
	GLint viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);
	yi = viewPort[3] - yi - 1;
	GLfloat depth;
	glReadPixels(xi, yi, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	return depth;
#endif
	*/

	return 0.0f;
}

// ---------------------------------------------------------------------
void Renderer::pushModelTransformation(const NxMat34 &affine)
{
	/*
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	GLfloat m[16];
	affine.getColumnMajor44(m);
	glMultMatrixf(m);
	*/
}

// ---------------------------------------------------------------------
void Renderer::popModelTransformation()
{
    //glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();
}

// ------------------ texture handling --------------------------------------


//---------------------------------------------------------------------------
int Renderer::findTexture(char *filename)
{
	int n = (int)textureRefs.size();
	int i = 0;
	while (i < n && _stricmp(textureRefs[i]->filename, filename))
		i++;
	if (i >= n) return -1;
	return i;
}


//---------------------------------------------------------------------------
int Renderer::addBmpTexture(char *filename)
{
	return 0;

	/*
	mBmpLoader.loadBmp(filename);

	// ------ read out RGB --------------------------------

	int w = mBmpLoader.mWidth;
	int h = mBmpLoader.mHeight;
	int lineLen = 3*w;

	// ------ generate texture --------------------------------

	RendererTexRef *ref = new RendererTexRef();
	glGenTextures(1, &ref->texID);

	glBindTexture(GL_TEXTURE_2D, ref->texID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, mBmpLoader.mRGB);

	strcpy(ref->filename, filename);
	textureRefs.push_back(ref);

	return (int)textureRefs.size()-1;
	*/
}



//---------------------------------------------------------------------------
void Renderer::freeTextures()
{
	for (int i = 0; i < (int)textureRefs.size(); i++) {
		//glDeleteTextures(1, &textureRefs[i]->texID);
		delete textureRefs[i];
	}
	textureRefs.clear();
}


//---------------------------------------------------------------------------
void Renderer::activateTexture(int texNr)
{
	if (texNr >= 0 && texNr < (int)textureRefs.size()) {
		//glEnable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, textureRefs[texNr]->texID);
	}
}


//---------------------------------------------------------------------------
void Renderer::deactivateTexture()
{
  //  glDisable(GL_TEXTURE_2D);
}


// Drawing procedures:

//---------------------------------------------------------------------------------------
void Renderer::drawPoint(const NxVec3 &center, RendererColor color)
{
	/*
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(1, GL_FLOAT, 0, &center);
	
	setColor(color);
	glDrawArrays(GL_POINTS, 0, 1); 

	glDisableClientState(GL_VERTEX_ARRAY);
	*/
}

//---------------------------------------------------------------------------------------
void Renderer::drawLine(const NxVec3 &p0, const NxVec3 &p1, RendererColor color)
{
/*
	NxVec3 points[2];
	points[0] = p0;
	points[1] = p1;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, points);

	setColor(color);
	glDrawArrays(GL_LINES, 0, 2);

	glDisableClientState(GL_VERTEX_ARRAY);
	*/
}

//---------------------------------------------------------------------------------------
void Renderer::drawTriangles(int numTriangles, const int *indices,
			const NxVec3 *vertices, const NxVec3 *normals, const TexCoord *texCoords)
{
	int d = 0;

	/*
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	NxU32 numVertices = numTriangles * 3; 

	if (normals) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, normals);
	}

	if (texCoords) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	}

#ifdef __CELLOS_LV2__	
	glDrawRangeElements(GL_TRIANGLES, 0, numVertices-1, 3*numTriangles, GL_UNSIGNED_INT, indices);
#else
	glDrawElements(GL_TRIANGLES, 3*numTriangles, GL_UNSIGNED_INT, indices);
#endif

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	*/
}

//---------------------------------------------------------------------------------------
void Renderer::drawTetrahedron(const NxVec3 &p0, const NxVec3 &p1, const NxVec3 &p2, const NxVec3 &p3,
			NxReal scale, RendererColor color)
{
	/*
	setColor(color);
	NxVec3 normal, center, v[4];
	center = p0;  v[0] = p0;
	center += p1; v[1] = p1;
	center += p2; v[2] = p2;
	center += p3; v[3] = p3;
	center *= 0.25f;
	v[0] = center + (v[0] - center)*scale;
	v[1] = center + (v[1] - center)*scale;
	v[2] = center + (v[2] - center)*scale;
	v[3] = center + (v[3] - center)*scale;
	int sides[4][3] = {{2,1,0}, {0,1,3}, {1,2,3}, {2,0,3}};

	NxVec3 renderVerts[3 * 4];
	NxVec3 renderNormals[3 * 4];

	NxU32 renderVertIdx = 0;
	NxU32 renderNormalIdx = 0;

	for (int side = 0; side < 4; side ++) {
		NxVec3 &v0 = v[sides[side][0]];
		NxVec3 &v1 = v[sides[side][1]];
		NxVec3 &v2 = v[sides[side][2]];
		normal.cross(v1-v0, v2-v0);
		normal.normalize();
		
		renderVerts[renderVertIdx++] = v0;
		renderVerts[renderVertIdx++] = v1;
		renderVerts[renderVertIdx++] = v2;

		renderNormals[renderNormalIdx++] = normal;
		renderNormals[renderNormalIdx++] = normal;
		renderNormals[renderNormalIdx++] = normal;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, renderVerts);
	glNormalPointer(GL_FLOAT, 0, renderNormals);

	glDrawArrays(GL_TRIANGLES, 0, 3*4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
*/
}

//---------------------------------------------------------------------------------------
void Renderer::drawBounds(const NxBounds3 &bounds, RendererColor color)
{
	/*
	setColor(color);
	NxVec3 v[8];
	v[0].set(bounds.min.x, bounds.min.y, bounds.min.z);
	v[1].set(bounds.max.x, bounds.min.y, bounds.min.z);
	v[2].set(bounds.max.x, bounds.max.y, bounds.min.z);
	v[3].set(bounds.min.x, bounds.max.y, bounds.min.z);
	v[4].set(bounds.min.x, bounds.min.y, bounds.max.z);
	v[5].set(bounds.max.x, bounds.min.y, bounds.max.z);
	v[6].set(bounds.max.x, bounds.max.y, bounds.max.z);
	v[7].set(bounds.min.x, bounds.max.y, bounds.max.z);
	int edges[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,0},{0,4},{1,5},{2,6},{3,7}};

	for (int edge = 0; edge < 12; edge ++) {
		NxVec3 &v0 = v[edges[edge][0]];
		NxVec3 &v1 = v[edges[edge][1]];

		drawLine(v0, v1, color);
	}
	*/
}

//---------------------------------------------------------------------------------------
void Renderer::drawBox(const NxBounds3 &bounds, RendererColor color)
{
	/*
	setColor(color);
	NxVec3 v[8];
	v[0].set(bounds.min.x, bounds.min.y, bounds.min.z);
	v[1].set(bounds.max.x, bounds.min.y, bounds.min.z);
	v[2].set(bounds.max.x, bounds.max.y, bounds.min.z);
	v[3].set(bounds.min.x, bounds.max.y, bounds.min.z);
	v[4].set(bounds.min.x, bounds.min.y, bounds.max.z);
	v[5].set(bounds.max.x, bounds.min.y, bounds.max.z);
	v[6].set(bounds.max.x, bounds.max.y, bounds.max.z);
	v[7].set(bounds.min.x, bounds.max.y, bounds.max.z);
	int sides[6][4] = {{0,3,2,1}, {4,5,6,7}, {0,1,5,4}, {1,2,6,5}, {2,3,7,6}, {3,0,4,7}};
	NxReal normals[6][3] = {{0,0,-1},{0,0,1},{0,-1,0},{1,0,0},{0,1,0},{-1,0,0}};

	NxVec3 renderVerts[6*6];
	NxVec3 renderNormals[6*6];

	NxU32 renderVertIdx = 0;
	NxU32 renderNormalIdx = 0;

	for (int side = 0; side < 6; side ++) {
		NxVec3 &v0 = v[sides[side][0]];
		NxVec3 &v1 = v[sides[side][1]];
		NxVec3 &v2 = v[sides[side][2]];
		NxVec3 &v3 = v[sides[side][3]];
		


		renderVerts[renderVertIdx++] = v0;
		renderVerts[renderVertIdx++] = v1;
		renderVerts[renderVertIdx++] = v2;

		renderVerts[renderVertIdx++] = v0;
		renderVerts[renderVertIdx++] = v2;
		renderVerts[renderVertIdx++] = v3;

		renderNormals[renderNormalIdx++] = normals[side];
		renderNormals[renderNormalIdx++] = normals[side];
		renderNormals[renderNormalIdx++] = normals[side];

		renderNormals[renderNormalIdx++] = normals[side];
		renderNormals[renderNormalIdx++] = normals[side];
		renderNormals[renderNormalIdx++] = normals[side];

	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, renderVerts);
	glNormalPointer(GL_FLOAT, 0, renderNormals);

	glDrawArrays(GL_TRIANGLES, 0, 6*6);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	*/
}

//---------------------------------------------------------------------------------------
void Renderer::drawWireTriangle(const NxVec3 &p0, const NxVec3 &p1, const NxVec3 &p2, RendererColor color)
{
	if (color != noColor) setColor(color);

	drawLine(p0, p1);
	drawLine(p1, p2);
	drawLine(p2, p0);
}

//---------------------------------------------------------------------------------------
void Renderer::subdivide(const NxVec3 &p0, const NxVec3 &p1, const NxVec3 &p2, int level) {

	if (level == 0) { drawWireTriangle(p0, p1, p2); return; }
	NxVec3 p01 = p0 + p1; p01.normalize();
	NxVec3 p12 = p1 + p2; p12.normalize();
	NxVec3 p20 = p2 + p0; p20.normalize();
	subdivide(p0, p01, p20, level-1);
	subdivide(p1, p12, p01, level-1);
	subdivide(p2, p20, p12, level-1);
	subdivide(p01, p12, p20, level-1);
}

#define X 0.525731112119133606f
#define Z 0.850650808352039932f

//---------------------------------------------------------------------------------------
void Renderer::drawWireSphere(const NxVec3 &center, NxReal radius, RendererColor color,  int level)
{
	/*
	static NxReal coords[12][3] = {
		{-X, 0.0f, Z}, {X, 0.0f, Z}, {-X, 0.0f, -Z}, {X, 0.0f, -Z},
		{0.0f, Z, X}, {0.0f, Z, -X}, {0.0f, -Z, X}, {0.0f, -Z, -X},
		{Z, X, 0.0f}, {-Z, X, 0.0f}, {Z, -X, 0.0f}, {-Z, -X, 0.0f}
	};

	static int indices[20][3] = {
		{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
		{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
		{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
	};

	setColor(color);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(center.x, center.y, center.z);
	glScalef(radius, radius, radius);
	NxVec3 p0,p1,p2;
	for (int i = 0; i < 20; i++) {
		int i0 = indices[i][0];
		int i1 = indices[i][1];
		int i2 = indices[i][2];
		p0.set(coords[i0][0], coords[i0][1], coords[i0][2]);
		p1.set(coords[i1][0], coords[i1][1], coords[i1][2]);
		p2.set(coords[i2][0], coords[i2][1], coords[i2][2]);
		subdivide(p0,p1,p2, level);
	}
	glPopMatrix();
	*/
}

//---------------------------------------------------------------------------------------
void Renderer::setColor(RendererColor color)
{
	/*
	if (color == noColor) return;
	const GLfloat s = 1.0f / 256.0f;
	glColor4f(
		(GLfloat)((color >> 16) & 0xFF)*s,
		(GLfloat)((color >>  8) & 0xFF)*s,
		(GLfloat)((color >>  0) & 0xFF)*s,
		1.0f
	);
	*/
}

//---------------------------------------------------------------------------------------
void Renderer::setColor3r(NxReal color[3])
{
	//glColor4f(color[0], color[1], color[2], 1.0f);
}

//---------------------------------------------------------------------------------------
void Renderer::setColor4r(NxReal color[4])
{
	//glColor4f(color[0], color[1], color[2], color[3]);
}

//---------------------------------------------------------------------------------------
float* Renderer::getFvColor(RendererColor color)
{
	const float s = 1.0f / 256.0f;
	mFvColor[0] = (float)((color >> 16) & 0xFF)*s;
	mFvColor[1] = (float)((color >>  8) & 0xFF)*s;
	mFvColor[2] = (float)((color >>  0) & 0xFF)*s;
	mFvColor[3] = 1.0f;
	return mFvColor;

	/*
	const GLfloat s = 1.0f / 256.0f;
	mFvColor[0] = (GLfloat)((color >> 16) & 0xFF)*s;
	mFvColor[1] = (GLfloat)((color >>  8) & 0xFF)*s;
	mFvColor[2] = (GLfloat)((color >>  0) & 0xFF)*s;
	mFvColor[3] = 1.0f;
	return mFvColor;
	*/
}

