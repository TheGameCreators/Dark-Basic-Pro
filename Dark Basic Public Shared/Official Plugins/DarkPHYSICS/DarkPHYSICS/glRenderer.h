#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#define NOMINMAX
#include <vector>
//#include <GL/glut.h>
#include "NxPhysics.h"
//#include "BmpLoader.h"

typedef unsigned int RendererColor;
struct RendererTexRef;

// ------------------------------------------------------------------------------------
struct TexCoord {
	TexCoord() {}
	TexCoord(NxReal u, NxReal v) { this->u = u; this->v = v; }
	void zero() { u = 0.0f; v = 0.0f; }
	TexCoord operator + (const TexCoord &tc) const {
		TexCoord r; r.u = u + tc.u; r.v = v + tc.v;
		return tc;
	}
	void operator += (const TexCoord &tc) {
		u += tc.u; v += tc.v;
	}
	void operator *= (NxReal r) {
		u *= r; v *= r;
	}
	void operator /= (NxReal r) {
		u /= r; v /= r;
	}
	NxReal u,v;
};

// ------------------------------------------------------------------------------------
class Renderer {
private:
	Renderer();
	~Renderer();

public:
	// singleton pattern
	static Renderer* getInstance();
	static void destroyInstance();

	static const RendererColor noColor = 0xFFFFFFFF;

	// general
	void clear();
	void clearDepth();
	void clearColor();
	void maskColor(bool red, bool green, bool blue);
	void lighting(bool on);
	void backColor(float *color);
	void wireframeMode(bool on);

	// view transformation
	void  project(NxVec3 &v, int &xi, int &yi, float &depth);
	void  unProject(int xi, int yi, float depth, NxVec3 &v);
	float depthAt(int xi, int yi);
	void  pushModelTransformation(const NxMat34 &affine);
	void  popModelTransformation();

	// textures
	int  findTexture(char *filename);
	int  addBmpTexture(char *filename);
	void freeTextures();
	void activateTexture(int texNr);
	void deactivateTexture();

	// drawing procedures
	void setColor(RendererColor color);
	void setColor3r(NxReal color[3]);
	void setColor4r(NxReal color[4]);

	void drawPoint(const NxVec3 &center, RendererColor color = noColor);
	void drawLine(const NxVec3 &p0, const NxVec3 &p1, RendererColor color = noColor);
	void drawTriangles(int numTriangles, const int *indices,
			const NxVec3 *vertices, const NxVec3 *normals, const TexCoord *texCoords);
	void drawTetrahedron(const NxVec3 &p0, const NxVec3 &p1,  const NxVec3 &p2,  const NxVec3 &p3,
			NxReal scale, RendererColor color = noColor);
	void drawBounds(const NxBounds3 &bounds, RendererColor color = noColor);
	void drawBox(const NxBounds3 &bounds, RendererColor color);
	void drawWireTriangle(const NxVec3 &p0, const NxVec3 &p1, const NxVec3 &p2, RendererColor color = noColor);
	void drawWireSphere(const NxVec3 &center, NxReal radius, RendererColor color = noColor, int level = 0);

	static const int colorWhite      = 0xFFFFFF;
	static const int colorBrown      = 0x502B27;
	static const int colorPeach      = 0xB85248;
	static const int colorRed        = 0xFF0000;
	static const int colorGreen      = 0x00FF00;
	static const int colorBlue       = 0x0000FF;
	static const int colorLightBlue  = 0x8080FF;
	static const int colorBlack      = 0x000000;
	static const int colorLightGrey  = 0xB2B2B2;
	static const int colorMediumGrey = 0x808080;
	static const int colorDarkGrey   = 0x333333;
	static const int colorYellow     = 0xFFFF00;
	static const int colorMagenta    = 0xFF00FF;
	static const int colorCyan       = 0x00FFFF;

private:
	//GLfloat* getFvColor(RendererColor color);
	//GLfloat mFvColor[4];

	float* getFvColor(RendererColor color);
	float mFvColor[4];

//	BmpLoader mBmpLoader;

	void subdivide(const NxVec3 &p0, const NxVec3 &p1, const NxVec3 &p2, int level);
	std::vector<RendererTexRef *> textureRefs;
};

#endif
