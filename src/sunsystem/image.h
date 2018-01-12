#ifndef IMAGE_H
#define IMAGE_H



#ifdef _WIN32
	#include "Include/freeglut.h"
#else
	#include <GL/freeglut.h>
#endif

typedef unsigned char BYTE;
typedef unsigned short int WORD;
//typedef unsigned int DWORD;
typedef long int LONG;
typedef unsigned int UINT;

#ifdef _WIN32
	typedef unsigned long QWORD;				// Windows 8 byte (???)
#else
	typedef unsigned long long int QWORD;		// Unix 8 byte
#endif



struct rawImageRec;



/////////////////////////////////////////////////////////////////////////////
//
//	Universelle Textur Struktur cg_image
//
/////////////////////////////////////////////////////////////////////////////
#pragma pack(1)
class cg_image
{
public:
	// Konstruktor
	cg_image ();

	// Destruktor
	~cg_image ();

	// Funktion zum Einlesen einer Textur. Rueckgabewert signalisiert, ob das Laden erfolgreich war.
	// Nach Aufruf der Funktion ist die Textur gebunden, kann also sofort modifiziert (glTexParameter etc.) und/oder verwendet werden
	bool load ( const char * fileName, bool generateMipmaps = false );

	// Binden der Texture, sodass sie verwendet wird
	void bind ();

	// Freigeben des belgten Speichers
	void free ();


private:
	GLuint glTex;				// OpenGL-Textur
	int BPP;					// Bits Per Pixel
	int sizeX;					// Breite
	int sizeY;					// Hoehe
	unsigned char *data;		// RGB Daten (bzw. Indizes)
	unsigned char *Palette;		// Optionale Palette

	// Datei-Lader
	bool loadBMP ( const char * fileName, bool verbose = true );
	bool loadTGA ( const char * fileName, bool verbose = true );
	bool loadRGB ( const char * fileName, bool verbose = true );

	//	Hilfsfunktionen
	static void convertShort ( unsigned short * array, unsigned int length );
	static void convertUint ( GLuint * array, unsigned int length );
	static rawImageRec * rawImageOpen ( const char * fileName, bool verbose );
	static void rawImageClose ( rawImageRec * raw );
	static bool rawImageGetRow ( rawImageRec * raw, unsigned char * buf, int y, int z );
	static bool rawImageGetData ( rawImageRec * raw, cg_image * final );

	// Konvertieren 8bit in 24bit Format
	void convert8bit2RGB ();
	// Konvertieren 8bit in 32bit Format mit vorgegebene Alpha Wert
	void convert8bit2RGBA ( int alpha );
	// Konvertieren 24bit in 32bit Format mit vorgegebene Alpha Wert
	void convertRGB2RGBA ( int alpha );
	// Stellt den Alpha Wert im Bild auf Alpha ein (nur wenn das Bild im 32bit Format vorliegt)
	void setAlpha ( int alpha );
	// Vertikales Drehen des Bildes
	bool verticalFlip ();
	// Horizontales Drehen des Bildes
	bool horizontalFlip ();
	// Tauscht rote und blaue Komponente (relevant fuer TGA)
	bool swapRB ();

	//	Allgemeine Hilfsfunktionen
	static int getFileSize ( const char * fileName );
	static int getFileSize ( int fileDes );
};



/******************************************************************************/
//
//	RGB Header Struktur
//
/******************************************************************************/
struct rawImageRec
{
	unsigned short imagic;
	unsigned short type;
	unsigned short dim;
	unsigned short sizeX, sizeY, sizeZ;
	unsigned long min, max;
	unsigned long wasteBytes;
	char name[80];
	unsigned long colorMap;
	FILE *file;
	unsigned char *tmp, *tmpR, *tmpG, *tmpB;
	unsigned long rleEnd;
	GLuint *rowStart;
	GLint *rowSize;
};


/******************************************************************************/
//
//	BMP Header Strukturen
//
/******************************************************************************/
#if 0
typedef struct tagRGBQUAD
{
	/* rgbq */
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER
{
	/* bmih */
	DWORD   biSize;
	int    biWidth;
	int    biHeight;
	WORD    biPlanes;
	WORD    biBitCount;
	DWORD   biCompression;
	DWORD   biSizeImage;
	int    biXPelsPerMeter;
	int    biYPelsPerMeter;
	DWORD   biClrUsed;
	DWORD   biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER
{
	/* bmfh */
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFO
{
	/* bmi */
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD            	bmiColors[1];
} BITMAPINFO;

#endif


/******************************************************************************/
//  TGA File Header
/******************************************************************************/
struct TGAHeader
{
	BYTE   IdLength;            /* Image ID Field Length      */
	BYTE   CmapType;            /* Color Map Type             */
	BYTE   ImageType;           /* Image Type                 */
	/*
	** Color Map Specification
	*/
	BYTE   CmapIndex[2];        /* First Entry Index          */
	WORD   CmapLength;       	/* Color Map Length           */
	BYTE   CmapEntrySize;       /* Color Map Entry Size       */
	/*
	** Image Specification
	*/
	WORD   X_Origin;            /* X-origin of Image          */
	WORD   Y_Origin;            /* Y-origin of Image          */
	WORD   ImageWidth;          /* Image Width                */
	WORD   ImageHeight;         /* Image Height               */
	BYTE   PixelDepth;          /* Pixel Depth                */
	BYTE   ImagDesc;            /* Image Descriptor           */
};



#endif	// IMAGE_H
