// Wavefront Loader
// PrakCG


#ifndef WAVEFRONT_H
#define WAVEFRONT_H


#ifndef __EXT
#define __EXT

#include "Include/glext.h"

#ifndef EXPORT_EXT
#define EXTERN_EXT extern
#define EXTERN_NULL
#else
#define EXTERN_EXT
#define EXTERN_NULL =NULL
#endif // EXPORT_EXT

EXTERN_EXT PFNGLGENBUFFERSPROC glGenBuffers EXTERN_NULL;
EXTERN_EXT PFNGLBINDBUFFERPROC glBindBuffer EXTERN_NULL;
EXTERN_EXT PFNGLBUFFERDATAPROC glBufferData EXTERN_NULL;
EXTERN_EXT PFNGLDELETEBUFFERSPROC glDeleteBuffers EXTERN_NULL;

bool initExtensions ();
inline float* normieren ( float v[3] );
inline void crossProduct( float *n, float *a, float *b );

class cg_object3D
{
public:
	// constructor
	cg_object3D ();
	// destructor
	~cg_object3D ();

	// Laden eines Wavefront-Objektes aus einer Datei,
	// Benötigter Speicher wird in dieser Fkt. allokiert
	// rgb: Farbe
	// x,y,z: Position
	void load ( const char *filename, bool use_vbos );

	// Objekt Material + Farben setzen
	void setMaterial ( float red, float green, float blue, float alpha, float spec, float shine, float emis );

	// Objekt Position setzen
	void setPosition ( float x, float y, float z );

	// Objekt Normalenmodus setzen
	void setNormalmode ( int normalmode = 2 );

	// Freigeben des belegten Speichers
	void free ();

	// Zeichnen des Objektes
	void draw ();

private:
	unsigned short int  normal_mode;	// 0: keine Normalen, 1: Normalen per Surface, 2: Normalen per Vertex
	int   numpoints;
	int   numtris;
	int	  numnormals;

	bool  clustertexcoords;
	bool  clusternormals;

	float *points;					// Liste der importierten Vertex Koordinaten
	float *texcoords;				// Liste der importierten Texturkoordinaten (Wavefront)
	float *normals;					// Liste der importierten Normalen (Wavefront)
	int   *tris;						// surface indexes
	float *f_normals;					// surface normals, berechnet
	float *p_normals;					// vertex normals, berechnet aus f_normals
	int	  *i_normals;				// importierte Vertexnormalen
	int   *i_texcoords;				// importierte Texturkoordinaten

	bool vbo_geladen;
	GLuint vbos[4];					// IDs der VBOs 0-Points, 1-tris, 2-f_normals, 3-p_normals

	float color[4];					// Material
	float amb[4];
	float diff[4];
	float spec[4];
	float shine;
	float emis[4];

	float pos[3];

	float borderPoints[4][3];


	bool readClusterVertex( char **r_line, int *p, int *t, int *n );
};



#endif // __EXT

#endif // WAVEFRONT_H
