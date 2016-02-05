#define _CRT_SECURE_NO_WARNINGS

#include "ObjMesh.h"
//#include <GL/glut.h>
#include <stdio.h>

#if defined(WIN32) || defined(_XBOX)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

//-----------------------------------------------------------------------------
void ObjMeshMaterial::init()
{
	strcpy(name, "");
	texNr = -1;
	strcpy(texFilename, "");
	ambient[0] = 0.0f; ambient[1] = 0.0f; ambient[2] = 0.0f;
	diffuse[0] = 0.0f; diffuse[1] = 0.0f; diffuse[2] = 0.0f;
	specular[0] = 0.0f; specular[1] = 0.0f; specular[2] = 0.0f;
	alpha = 0.0f;
	shininess = 0.0f;
};

// ----------------------------------------------------------------------
void ObjMesh::clear()
{
	mTriangles.clear();
	mNormals.clear();
	mTexCoords.clear();
	mVertices.clear();
	mMaterials.clear();
	mBounds.setEmpty();
	mHasTextureCoords = false;
	mHasNormals = false;

	strcpy(mPath, "");
	strcpy(mName, "");
}

// ----------------------------------------------------------------------
void ObjMesh::draw(unsigned int baseColor, bool wireframe, bool shadows)
{
	if (mVertices.size() == 0) 
		return;

	Renderer *renderer = Renderer::getInstance();
	renderer->setColor(baseColor);

	if (wireframe)
		renderer->wireframeMode(true);
		

	if (!mHasTextureCoords)
		renderer->deactivateTexture();

	int i;
	int materialNr = -1;
	TexCoord *texPointer = NULL;
	float textured = false;
	ObjMeshString fname;

	mDrawIndices.clear();

	if (!mHasTextureCoords) {
		for (i = 0; i < (int)mTriangles.size(); i++) {
			ObjMeshTriangle &t = mTriangles[i];
			mDrawIndices.push_back(t.vertexNr[0]);
			mDrawIndices.push_back(t.vertexNr[1]);
			mDrawIndices.push_back(t.vertexNr[2]);
		}


		renderer->drawTriangles(mTriangles.size(), &mDrawIndices[0], &mVertices[0], &mNormals[0], NULL);

		/*
		if (shadows) {
			const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
			glPushMatrix();
			glMultMatrixf(ShadowMat);
			glDisable(GL_LIGHTING);

			glColor4f(0.05f, 0.1f, 0.15f,1.0f);
			renderer->drawTriangles(mTriangles.size(), &mDrawIndices[0], &mVertices[0], &mNormals[0], NULL);
			
			renderer->setColor(baseColor);
			glEnable(GL_LIGHTING);
			glPopMatrix();
		}
		*/
		
		if (wireframe)
			renderer->wireframeMode(false);

		return;
	}

	for (i = 0; i < (int)mMaterials.size(); i++) {
		ObjMeshMaterial &m = mMaterials[i];
		if (strlen(m.texFilename) == 0)
			m.texNr = -1;
		else {
			sprintf(fname, "%s\\%s", mPath, m.texFilename);
			m.texNr = renderer->findTexture(fname);
			if (m.texNr < 0)
				m.texNr = renderer->addBmpTexture(fname);
		}
	}

	for (i = 0; i < (int)mTriangles.size(); i++) {
		ObjMeshTriangle &t = mTriangles[i];
		if (t.materialNr != materialNr) {
			if ((int)mDrawIndices.size() > 0) {
				renderer->drawTriangles(mDrawIndices.size()/3, &mDrawIndices[0], 
					&mVertices[0], &mNormals[0], textured ? &mTexCoords[0] : NULL);
			}

			mDrawIndices.clear();
			textured = false;

			materialNr = t.materialNr;
			if (materialNr >= 0) {
				int texNr = mMaterials[materialNr].texNr;
				if (texNr >= 0) {
					renderer->activateTexture(texNr);
					renderer->setColor(baseColor);
					textured = true;
				}
				else {
					renderer->deactivateTexture();
					renderer->setColor3r(mMaterials[materialNr].ambient);
				}
			}
		}

		mDrawIndices.push_back(t.vertexNr[0]);
		mDrawIndices.push_back(t.vertexNr[1]);
		mDrawIndices.push_back(t.vertexNr[2]);
	}


	if ((int)mDrawIndices.size() > 0) { 
		renderer->drawTriangles(mDrawIndices.size()/3, &mDrawIndices[0], 
			&mVertices[0], &mNormals[0], textured ? &mTexCoords[0] : NULL);
	}

	/*
	if (shadows) {
		const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
		glPushMatrix();
		glMultMatrixf(ShadowMat);
		glDisable(GL_LIGHTING);

		glColor4f(0.05f, 0.1f, 0.15f,1.0f);
		renderer->drawTriangles(mDrawIndices.size()/3, &mDrawIndices[0], 
			&mVertices[0], &mNormals[0], textured ? &mTexCoords[0] : NULL);
		
		renderer->setColor(baseColor);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
	*/

	if (wireframe)
		renderer->wireframeMode(false);
}

// ----------------------------------------------------------------------
void ObjMesh::setDiagonal(NxReal diagonal)
{
	NxReal d = mBounds.min.distance(mBounds.max);
	if (d == 0.0f) return;
	NxReal s = diagonal / d;
	for (int i = 0; i < (int)mVertices.size(); i++) 
		mVertices[i] *= s;
	updateBounds();
}

// ----------------------------------------------------------------------
void ObjMesh::setCenter(const NxVec3 &center)
{
	NxVec3 d = center - (mBounds.min + mBounds.max) * 0.5f;
	for (int i = 0; i < (int)mVertices.size(); i++) 
		mVertices[i] += d;
	updateBounds();
}

// ----------------------------------------------------------------------
void ObjMesh::transform(const NxMat34 &a)
{
	for (int i = 0; i < (int)mVertices.size(); i++) {
		NxVec3 &v = mVertices[i];
		v = a * v;
	}
	updateBounds();
	updateNormals();
}

// ----------------------------------------------------------------------
void ObjMesh::getTriangleBounds(int i, NxBounds3 &bounds) const
{
	const ObjMeshTriangle &mt = mTriangles[i];
	bounds.setEmpty();
	bounds.include(mVertices[mt.vertexNr[0]]);
	bounds.include(mVertices[mt.vertexNr[1]]);
	bounds.include(mVertices[mt.vertexNr[2]]);
}

// ----------------------------------------------------------------------
void ObjMesh::updateNormals()
{
	mNormals.resize(mVertices.size());
	int i;
	for (i = 0; i < (int)mNormals.size(); i++) 
		mNormals[i].zero();
	NxVec3 n;
	for (i = 0; i < (int)mTriangles.size(); i++) {
		ObjMeshTriangle &mt = mTriangles[i];
		mt.normalNr[0] = mt.vertexNr[0];
		mt.normalNr[1] = mt.vertexNr[1];
		mt.normalNr[2] = mt.vertexNr[2];
		n.cross(mVertices[mt.vertexNr[1]] - mVertices[mt.vertexNr[0]], 
			mVertices[mt.vertexNr[2]] - mVertices[mt.vertexNr[0]]);
		mNormals[mt.normalNr[0]] += n;
		mNormals[mt.normalNr[1]] += n;
		mNormals[mt.normalNr[2]] += n;
	}
	for (i = 0; i < (int)mNormals.size(); i++) 
		mNormals[i].normalize();
}

// ----------------------------------------------------------------------
void ObjMesh::updateBounds()
{
	mBounds.setEmpty();
	for (int i = 0; i < (int)mVertices.size(); i++)
		mBounds.include(mVertices[i]);
}

// -----------------------------------------------------------------------------------
bool ObjMesh::rayTriangleIntersection(const NxVec3 &orig, const NxVec3 &dir,
	 									 const ObjMeshTriangle &triangle,
	 									 NxReal &t, NxReal &u, NxReal &v) const
{
	const NxVec3 &a = mVertices[triangle.vertexNr[0]];
	const NxVec3 &b = mVertices[triangle.vertexNr[0]];
	const NxVec3 &c = mVertices[triangle.vertexNr[0]];

	NxVec3 edge1, edge2, tvec, pvec, qvec;
	NxReal det,inv_det;

	edge1 = b - a;
	edge2 = c - a;
	pvec.cross(dir, edge2);

	/* if determinant is near zero, ray lies in plane of triangle */
	det = edge1.dot(pvec);

	if (det == 0.0f)
		return false;
	inv_det = 1.0f / det;

	/* calculate distance from vert0 to ray origin */
	tvec = orig - a;

	/* calculate U parameter and test bounds */
	u = tvec.dot(pvec) * inv_det;
	if (u < 0.0f || u > 1.0f)
	return false;

	/* prepare to test V parameter */
	qvec.cross(tvec, edge1);

	/* calculate V parameter and test bounds */
	v = dir.dot(qvec) * inv_det;
	if (v < 0.0f || u + v > 1.0f)
	return false;

	/* calculate t, ray intersects triangle */
	t = edge2.dot(qvec) * inv_det;

	return true;
}

// ----------------------------------------------------------------------
bool ObjMesh::rayCast(const NxVec3 &orig, const NxVec3 &dir, NxReal &t)
{
	t = -1.0f;
	for (int i = 0; i < (int)mTriangles.size(); i++) {
		ObjMeshTriangle &mt = mTriangles[i];
		NxReal ti, u,v;
		if (!rayTriangleIntersection(orig, dir, mt, ti, u,v))
			continue;
		if (ti < 0.0f) continue;
		if (t < 0.0f || ti < t) t = ti;
	}
	return t >= 0.0f;
}

// --------------------------------------------------------------------------
void ObjMesh::extractPath(char *filename)
{
	strcpy(mPath, filename);
	int i = strlen(mPath)-1;
	while (i >= 0 && mPath[i] != '\\' && mPath[i] != ':') i--;
	if (i >= 0) mPath[i] = '\0';
	else strcpy(mPath, ".");

	int j = 0;
	i++;
	while (i < (int)strlen(filename))
		mName[j++] = filename[i++];
	mName[j] = '\0';
}

//-----------------------------------------------------------------------------
bool ObjMesh::importMtlFile(char *mtllib)
{
	ObjMeshString fname;
	sprintf(fname, "%s\\%s", mPath, mtllib);
	FILE *f = fopen(fname, "r");
	if (!f) return false;
	mMaterials.clear();
	ObjMeshString s;
	ObjMeshMaterial mat;
	mat.init();
	while (!feof(f)) {
		fgets(s, OBJ_MESH_STRING_LEN, f);
		if (strncasecmp(s, "newmtl", 6) == 0) {
			if (strlen(mat.name) > 0)
				mMaterials.push_back(mat);
			mat.init();
			sscanf(&s[7], "%s", mat.name);
		}
		else if (strncasecmp(s, "Ka", 2) == 0) {
			sscanf(&s[3], "%f %f %f", &mat.ambient[0], &mat.ambient[1], &mat.ambient[2]);
		}
		else if (strncasecmp(s, "Kd", 2) == 0) {
			sscanf(&s[3], "%f %f %f", &mat.diffuse[0], &mat.diffuse[1], &mat.diffuse[2]);
		}
		else if (strncasecmp(s, "Ks", 2) == 0) {
			sscanf(&s[3], "%f %f %f", &mat.specular[0], &mat.specular[1], &mat.specular[2]);
		}
		else if (strncasecmp(s, "Ns", 2) == 0) {
			sscanf(&s[3], "%f", &mat.shininess);
		}
		else if (strncasecmp(s, "Tr", 2) == 0) {
			sscanf(&s[3], "%f", &mat.alpha);
		}
		else if (strncasecmp(s, "d ", 2) == 0) {
			sscanf(&s[2], "%f", &mat.alpha);
		}
		else if (strncasecmp(s, "map_K ", 5) == 0) {
			sscanf(&s[7], "%s", mat.texFilename);
		}
	}
	if (strlen(mat.name) > 0)
		mMaterials.push_back(mat);
	fclose(f);
	return true;
}


//-----------------------------------------------------------------------------
void parseRef(char *s, int nr[])
{
	int i,j,k;
	for (k = 0; k < 3; k++)
		nr[k] = -1;

	int len = strlen(s);
	char is[256]; i = 0;

	for (k = 0; k < 3; k++) {
		j = 0;
		while (i < len && s[i] != '/') {
			is[j] = s[i]; i++; j++;
		}
		i++;
		if (j > 0) {
			is[j] = 0;
			sscanf(is, "%i", &nr[k]);
		}
	}
}

// ----------------------------------------------------------------------
bool ObjMesh::loadFromObjFile(char *filename)
{
	FILE *f = fopen(filename, "r");
	if (!f) return false;
	clear();
	ObjMeshString s, subs[maxVerticesPerFace];
	ObjMeshString mtllib, matName;

	mHasTextureCoords = false;
	mHasNormals = false;

	strcpy(mtllib, "");
	int materialNr = -1;
	int i,j;
	NxVec3 v;
	ObjMeshTriangle t;
	TexCoord tc;

	std::vector<NxVec3> centermVertices;
	std::vector<TexCoord> centermTexCoords;
	std::vector<NxVec3> centerNormals;

	extractPath(filename);

	while (!feof(f)) {
		if (fgets(s, OBJ_MESH_STRING_LEN, f) == NULL) break;

		if (strncmp(s, "mtllib", 6) == 0) {  // material library
			sscanf(&s[7], "%s", mtllib);
			importMtlFile(mtllib);
		}
		else if (strncmp(s, "usemtl", 6) == 0) {  // use material
			sscanf(&s[7], "%s", matName);
			materialNr = 0;
			int numMaterials = (int)mMaterials.size();
			while (materialNr < numMaterials &&
				   strcasecmp(mMaterials[materialNr].name, matName) != 0)
				materialNr++;
			if (materialNr >= numMaterials) 
				materialNr = -1;
		}
		else if (strncmp(s, "v ", 2) == 0) {	// vertex
			sscanf(s, "v %f %f %f", &v.x, &v.y, &v.z);
			mVertices.push_back(v);
		}
		else if (strncmp(s, "vn ", 3) == 0) {	// normal
			sscanf(s, "vn %f %f %f", &v.x, &v.y, &v.z);
			mNormals.push_back(v);
		}
		else if (strncmp(s, "vt ", 3) == 0) {	// texture coords
			sscanf(s, "vt %f %f", &tc.u, &tc.v);
			mTexCoords.push_back(tc);
		}
		else if (strncmp(s, "f ", 2) == 0) {	// face
			int nr;
			nr = sscanf(s, "f %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
			subs[0], subs[1], subs[2], subs[3], subs[4],
			subs[5], subs[6], subs[7], subs[8], subs[9],
			subs[10], subs[11], subs[12],subs[13], subs[14]);
			int vertNr[maxVerticesPerFace], texNr[maxVerticesPerFace];
			int normalNr[maxVerticesPerFace];
			for (i = 0; i < nr; i++) {
				int refs[3];
				parseRef(subs[i], refs);
				vertNr[i] = refs[0]-1;
				texNr[i] = refs[1]-1;
				normalNr[i] = refs[2]-1;
			}
			if (nr <= 4) {	// simple non-singular triangle or quad
				if (vertNr[0] != vertNr[1] && vertNr[1] != vertNr[2] && vertNr[2] != vertNr[0]) {
					t.init();
					t.vertexNr[0] = vertNr[0];
					t.vertexNr[1] = vertNr[1];
					t.vertexNr[2] = vertNr[2];
					t.normalNr[0] = normalNr[0];
					t.normalNr[1] = normalNr[1];
					t.normalNr[2] = normalNr[2];
					t.texCoordNr[0] = texNr[0];
					t.texCoordNr[1] = texNr[1];
					t.texCoordNr[2] = texNr[2];
					t.materialNr = materialNr;
					mTriangles.push_back(t);
				}
				if (nr == 4) {	// non-singular quad -> generate a second triangle
					if (vertNr[2] != vertNr[3] && vertNr[3] != vertNr[0] && vertNr[0] != vertNr[2]) {
						t.init();
						t.vertexNr[0] = vertNr[2];
						t.vertexNr[1] = vertNr[3];
						t.vertexNr[2] = vertNr[0];
						t.normalNr[0] = normalNr[2];
						t.normalNr[1] = normalNr[3];
						t.normalNr[2] = normalNr[0];
						t.texCoordNr[0] = texNr[0];
						t.texCoordNr[1] = texNr[1];
						t.texCoordNr[2] = texNr[2];
						t.materialNr = materialNr;
						mTriangles.push_back(t);
					}
				}
			}
			else {	// polygonal face

				// compute center properties
				NxVec3 centerPos(0.0f, 0.0f, 0.0f);
				TexCoord centerTex; centerTex.zero();
				for (i = 0; i < nr; i++) {
					centerPos += mVertices[vertNr[i]];
					if (texNr[i] >= 0) centerTex += mTexCoords[texNr[i]];
				}
				centerPos /= (float)nr;
				centerTex /= (float)nr;
				NxVec3 d1 = centerPos - mVertices[vertNr[0]];
				NxVec3 d2 = centerPos - mVertices[vertNr[1]];
				NxVec3 centerNormal = d1.cross(d2); centerNormal.normalize();

				// add center vertex
				centermVertices.push_back(centerPos);
				centermTexCoords.push_back(centerTex);
				centerNormals.push_back(centerNormal);

				// add surrounding elements
				for (i = 0; i < nr; i++) {
					j = i+1; if (j >= nr) j = 0;
					t.init();
					t.vertexNr[0] = mVertices.size() + centermVertices.size()-1;
					t.vertexNr[1] = vertNr[i];
					t.vertexNr[2] = vertNr[j];

					t.normalNr[0] = mNormals.size() + centerNormals.size()-1;
					t.normalNr[1] = normalNr[i];
					t.normalNr[2] = normalNr[j];

					t.texCoordNr[0] = mTexCoords.size() + centermTexCoords.size()-1;
					t.texCoordNr[1] = texNr[i];
					t.texCoordNr[2] = texNr[j];
					t.materialNr = materialNr;
					mTriangles.push_back(t);
				}
			}
		}
	}
	fclose(f);

	// new center mVertices are inserted here.
	// If they were inserted when generated, the vertex numbering would be corrupted
	for (i = 0; i < (int)centermVertices.size(); i++)
		mVertices.push_back(centermVertices[i]);
	for (i = 0; i < (int)centerNormals.size(); i++)
		mNormals.push_back(centerNormals[i]);
	for (i = 0; i < (int)centermTexCoords.size(); i++)
		mTexCoords.push_back(centermTexCoords[i]);

	if (mTexCoords.size() > 0) mHasTextureCoords = true;
	if (mNormals.size() > 0) 
		mHasNormals = true;
	else
		updateNormals();

	updateBounds();
	return true;
}

// ----------------------------------------------------------------------
bool ObjMesh::saveToObjFile(char *filename) const
{
	// extract name and path
	ObjMeshString name, path;
	strcpy(path, filename);
	int i = strlen(path)-1;
	while (i >= 0 && path[i] != '\\' && path[i] != ':') i--;
	if (i >= 0) path[i] = '\0';
	else strcpy(path, ".");
	strcpy(name, &filename[i+1]);
	i = strlen(name);
	while (i >= 0 && name[i] != '.') i--;
	if (name[i] == '.') name[i] = '\0';

	ObjMeshString objName, mtlName;
	sprintf(objName, "%s\\%s.obj", path, name);
	sprintf(mtlName, "%s\\%s.mtl", path, name);

	// export material file
	FILE *f = fopen(mtlName, "w");
	if (!f) return false;
	for (i = 0; i < (int)mMaterials.size(); i++) {
		const ObjMeshMaterial &mat = mMaterials[i];
		fprintf(f, "\n"); 
		fprintf(f, "newmtl %s\n", mat.name); 
		fprintf(f, "Ka %f %f %f\n", mat.ambient[0], mat.ambient[1], mat.ambient[2]); 
		fprintf(f, "Kd %f %f %f\n", mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]); 
		fprintf(f, "Ks %f %f %f\n", mat.specular[0], mat.specular[1], mat.specular[2]); 
		fprintf(f, "Ns %f\n", mat.shininess); 
		fprintf(f, "Tr %f\n", mat.alpha); 
		if (strlen(mat.texFilename) > 0)
			fprintf(f, "map_Kd %s\n", mat.texFilename);
	}
	fclose(f);

	// export obj file
	f = fopen(objName, "w");
	if (!f) return false;

	fprintf(f, "# Wavefront OBJ\n");
	fprintf(f, "\n");
	fprintf(f, "mtllib %s.mtl\n", name);
	fprintf(f, "\n");

	fprintf(f, "\n");
	fprintf(f, "# %i vertices:\n", mVertices.size());
	for (i = 0; i < (int)mVertices.size(); i++) {
		const NxVec3 &v = mVertices[i];
		fprintf(f,"v %f %f %f\n", v.x, v.y, v.z);
	}

	fprintf(f, "\n");
	fprintf(f, "# %i texture coordinates:\n", mTexCoords.size());
	for (i = 0; i < (int)mTexCoords.size(); i++) {
		const TexCoord &tc = mTexCoords[i];
		fprintf(f,"vt %f %f\n", tc.u, tc.v);
	}

	if (mHasNormals) {	// make sure we don't save our own ones
		fprintf(f, "\n");
		fprintf(f, "# %i normals:\n", mNormals.size());
		for (i = 0; i < (int)mNormals.size(); i++) {
			const NxVec3 &v = mNormals[i];
			fprintf(f,"vn %f %f %f\n", v.x, v.y, v.z);
		}
	}

	int matNr = -1;
	int groupNr = 0;
	fprintf(f, "\n");
	fprintf(f, "# %i triangles:\n", mTriangles.size());
	for (i = 0; i < (int)mTriangles.size(); i++) {
		const ObjMeshTriangle &t = mTriangles[i];
		if (t.materialNr != matNr) {
			matNr = t.materialNr;
			if (matNr >= 0) {
				fprintf(f, "\n");
				fprintf(f, "g group%i\n", groupNr++);
				fprintf(f, "usemtl %s\n", mMaterials[matNr].name);
			}
		}
		fprintf(f, "f");
		for (int j = 0; j < 3; j++) {
			fprintf(f, " ");
			fprintf(f, "%i/", t.vertexNr[j]+1);
			if (t.texCoordNr[j] >= 0) fprintf(f, "%i", t.texCoordNr[j]+1);
			fprintf(f, "/");
			if (mHasNormals && t.normalNr[j] >= 0) fprintf(f, "%i", t.normalNr[j]+1);
		}
		fprintf(f, "\n");
	}
	fclose(f);

	return true;
}

// -----------------------------------------------------------------------
// compute the links between the surface mesh and tetrahedras
void ObjMesh::buildTetraLinks(const NxVec3 *vertices, const NxU32 *indices, const NxU32 numTets) {
	mTetraLinks.clear();

	MeshHash* hash = new MeshHash();

	// hash tetrahedra for faster search
	hash->setGridSpacing(mBounds.min.distance(mBounds.max) * 0.1f);

	for (NxU32 i = 0; i < numTets; i++) {
		const NxU32 *ix = &indices[4*i];
		NxBounds3 tetraBounds;
		tetraBounds.setEmpty();
		tetraBounds.include(vertices[*ix++]);
		tetraBounds.include(vertices[*ix++]);
		tetraBounds.include(vertices[*ix++]);
		tetraBounds.include(vertices[*ix++]);
		hash->add(tetraBounds, i);
	}

	for (NxU32 i = 0; i < mVertices.size(); i++) {
		ObjMeshTetraLink tmpLink;

		NxVec3 triVert = mVertices[i];
		std::vector<int> itemIndices;
		hash->queryUnique(triVert, itemIndices);

		NxReal minDist = 0.0f;
		NxVec3 b;
		int num, isize;
		num = isize = itemIndices.size();
		if (num == 0) num = numTets;

		for (int i = 0; i < num; i++) {
			int j = i;
			if (isize > 0) j = itemIndices[i];

			const NxU32 *ix = &indices[j*4];
			const NxVec3 &p0 = vertices[*ix++];
			const NxVec3 &p1 = vertices[*ix++];
			const NxVec3 &p2 = vertices[*ix++];
			const NxVec3 &p3 = vertices[*ix++];

			NxVec3 b = computeBaryCoords(triVert, p0, p1, p2, p3);

			// is the vertex inside the tetrahedron? If yes we take it
			if (b.x >= 0.0f && b.y >= 0.0f && b.z >= 0.0f && (b.x + b.y + b.z) <= 1.0f) {
				tmpLink.barycentricCoords = b;
				tmpLink.tetraNr = j;
				break;
			}

			// otherwise, if we are not in any tetrahedron we take the closest one
			NxReal dist = 0.0f;
			if (b.x + b.y + b.z > 1.0f) dist = b.x + b.y + b.z - 1.0f;
			if (b.x < 0.0f) dist = (-b.x < dist) ? dist : -b.x;
			if (b.y < 0.0f) dist = (-b.y < dist) ? dist : -b.y;
			if (b.z < 0.0f) dist = (-b.z < dist) ? dist : -b.z;

			if (i == 0 || dist < minDist) {
				minDist = dist;
				tmpLink.barycentricCoords = b;
				tmpLink.tetraNr = j;
			}
		}

		mTetraLinks.push_back(tmpLink);
	}
	delete hash;
}

// ----------------------------------------------------------------------
bool ObjMesh::updateTetraLinks(const NxMeshData &tetraMeshData)
{
	if (mTetraLinks.size() != mVertices.size()) return false;

	NxU32 numVertices = *tetraMeshData.numVerticesPtr;
	NxU32 numTetrahedra = *tetraMeshData.numIndicesPtr / 4;
	const NxVec3 *vertices = (NxVec3*)tetraMeshData.verticesPosBegin;
	NxU32* indices = (NxU32*)tetraMeshData.indicesBegin;

	for (int i = 0; i < (int)mVertices.size(); i++) {
		ObjMeshTetraLink &link = mTetraLinks[i];
		const NxU32 *ix = &indices[4*link.tetraNr];
		const NxVec3 &p0 = vertices[*ix++];
		const NxVec3 &p1 = vertices[*ix++];
		const NxVec3 &p2 = vertices[*ix++];
		const NxVec3 &p3 = vertices[*ix++];

		NxVec3 &b = link.barycentricCoords;
		mVertices[i] = p0 * b.x + p1 * b.y + p2 * b.z + p3 * (1.0f - b.x - b.y - b.z);
	}
	updateNormals();

	return true;
}

// ----------------------------------------------------------------------
bool ObjMesh::simulateMesh(const NxMeshData &tetraMeshData)
{
	if (updateTetraLinks(tetraMeshData)) 
		return true;
	else
		return false;
}

// ----------------------------------------------------------------------
// computes barycentric coordinates
NxVec3 ObjMesh::computeBaryCoords(NxVec3 vertex, NxVec3 p0, NxVec3 p1, NxVec3 p2, NxVec3 p3) {
	NxVec3 baryCoords;

	NxVec3 q  = vertex-p3;
	NxVec3 q0 = p0-p3;
	NxVec3 q1 = p1-p3;
	NxVec3 q2 = p2-p3;

	NxMat33 m;
	m.setColumn(0,q0);
	m.setColumn(1,q1);
	m.setColumn(2,q2);

	NxReal det = m.determinant();

	m.setColumn(0, q);
	baryCoords.x = m.determinant();

	m.setColumn(0, q0); m.setColumn(1,q);
	baryCoords.y = m.determinant();

	m.setColumn(1, q1); m.setColumn(2,q);
	baryCoords.z = m.determinant();

	if (det != 0.0f)
		baryCoords /= det;

	return baryCoords;
}

// clone an object mesh (saves reloading it)
void ObjMesh::clone ( ObjMesh* pSrcObjMesh )
{
	mTriangles.clear();
	for ( int i = 0; i < (int)pSrcObjMesh->mTriangles.size ( ); i++ )
		mTriangles.push_back( pSrcObjMesh->mTriangles [ i ] );

	mVertices.clear();
	for ( int i = 0; i < (int)pSrcObjMesh->mVertices.size ( ); i++ )
		mVertices.push_back( pSrcObjMesh->mVertices [ i ] );

	mNormals.clear();
	for ( int i = 0; i < (int)pSrcObjMesh->mNormals.size ( ); i++ )
		mNormals.push_back( pSrcObjMesh->mNormals [ i ] );

	mTexCoords.clear();
	for ( int i = 0; i < (int)pSrcObjMesh->mTexCoords.size ( ); i++ )
		mTexCoords.push_back( pSrcObjMesh->mTexCoords [ i ] );

	mMaterials.clear();
	for ( int i = 0; i < (int)pSrcObjMesh->mMaterials.size ( ); i++ )
		mMaterials.push_back( pSrcObjMesh->mMaterials [ i ] );

	mDrawIndices.clear();
	for ( int i = 0; i < (int)pSrcObjMesh->mDrawIndices.size ( ); i++ )
		mDrawIndices.push_back( pSrcObjMesh->mDrawIndices [ i ] );

	mTetraLinks.clear();
	for ( int i = 0; i < (int)pSrcObjMesh->mTetraLinks.size ( ); i++ )
		mTetraLinks.push_back( pSrcObjMesh->mTetraLinks [ i ] );

	strcpy ( mName, pSrcObjMesh->mName );
	strcpy ( mPath, pSrcObjMesh->mPath );
	mBounds = pSrcObjMesh->mBounds;
	mHasTextureCoords = pSrcObjMesh->mHasTextureCoords;
	mHasNormals = pSrcObjMesh->mHasNormals;
}
