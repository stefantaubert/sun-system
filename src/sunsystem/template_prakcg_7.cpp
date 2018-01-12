#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "Include/freeglut.h"
#include "Include/glext.h"
#define __EXPORT_HELP
#include "help.h"
#include "window.h"
#include "wavefront.h"
#include "input.h"
#include "light.h"
#include "image.h"



void setCamera ();		// Kamera platzieren, siehe Maus-Callbacks
void drawScene ();		// Zeichnet die Szene im Weltkoordinatensystem
void initTextures();	// Texturen laden



/////////////////////////////////////////////////////////////////////////////////
//	Kamerafunktion
/////////////////////////////////////////////////////////////////////////////////
void setCamera ()
{
	cg_mouse mouse;
	// Ansichtstransformationen setzen,
	// SetCamera() zum Beginn der Zeichenfunktion aufrufen
	double x, y, z, The, Phi;
	static double radius = 25;
	// Maus abfragen
	if ( cg_mouse::buttonState( GLUT_LEFT_BUTTON ) )
	{
		cg_globState::cameraHelper[0] += mouse.moveX();
		cg_globState::cameraHelper[1] += mouse.moveY();
	}
	if ( cg_mouse::buttonState( GLUT_MIDDLE_BUTTON ) )
	{
		radius += 0.1 * mouse.moveY();
		if ( radius < 1.0 ) radius = 1.0;
	}
	Phi = 0.2 * cg_globState::cameraHelper[0] / cg_globState::screenSize[0] * M_PI + M_PI * 0.5;
	The = 0.2 * cg_globState::cameraHelper[1] / cg_globState::screenSize[1] * M_PI;
	x = radius * cos ( Phi ) * cos ( The );
	y = radius * sin ( The );
	z = radius * sin ( Phi ) * cos ( The );
	int Oben = ( The <= 0.5 * M_PI || The > 1.5 * M_PI ) * 2 - 1;
	gluLookAt ( x, y, z, 0, 0, 0, 0, Oben, 0 );
}



/////////////////////////////////////////////////////////////////////////////////
//	Anfang des OpenGL Programmes
/////////////////////////////////////////////////////////////////////////////////
int main ( int argc, char **argv )
{
	init( argc, argv );

	// hier Objekte laden, erstellen etc.
	initTextures();

	// Die Hauptschleife starten
	glutMainLoop ();

	return 0;
}



void displayFunc ()
{
	// Hilfe-Instanzen
	cg_help help;
	cg_globState globState;
	cg_key key;

	// Tastatur abfragen
	// Achtung: einmaliges Betätigen funktioniert so nur mit glutIgnoreKeyRepeat(true) (siehe main())
	if ( key.keyState( 27 ) )
	{
		exit ( 0 ); // Escape -> Programm beenden
	}
	else if ( 1 == key.keyState( 'f' ) || 1 == key.keyState( 'F' ) )
	{
		help.toggleFps();	// Framecounter on/off
	}
	else if ( 1 == key.keyState( 'h' ) || 1 == key.keyState( 'H' ) || 1 == key.specialKeyState( GLUT_KEY_F1 ) )
	{
		help.toggle();	// Hilfetext on/off
	}
	else if ( 1 == key.keyState( 'k' ) || 1 == key.keyState( 'K' ) )
	{
		help.toggleKoordsystem();	// Koordinatensystem on/off
	}
	else if ( 1 == key.keyState( 'w' ) || 1 == key.keyState( 'W' ) )
	{
		globState.drawMode = ( globState.drawMode == GL_FILL ) ? GL_LINE : GL_FILL; // Wireframe on/off
	}
	else if ( 1 == key.keyState( 'l' ) || 1 == key.keyState( 'L' ) )
	{
		globState.lightMode = !globState.lightMode;	// Beleuchtung on/off
	}
	else if ( 1 == key.keyState( 'c' ) || 1 == key.keyState( 'C' ) )
	{
		globState.cullMode = !globState.cullMode; // Backfaceculling on/off
	}
	else if ( 1 == key.keyState( 'i' ) || 1 == key.keyState( 'I' ) )
	{
		globState.cameraHelper[0] = 0;	// Initialisierung der Kamera
		globState.cameraHelper[1] = 0;
	}


	// Szene zeichnen: CLEAR, SETCAMERA, DRAW_SCENE

	// Back-Buffer neu initialisieren
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity ();

	// Kamera setzen (spherische Mausnavigation)
	setCamera();

	// Koordinatensystem zeichnen
	help.drawKoordsystem ( -8, 10, -8, 10, -8, 10 );

	// Zeichenmodus einstellen (Wireframe on/off)
	glPolygonMode( GL_FRONT_AND_BACK, globState.drawMode );

	// Backface Culling on/off);
	glFrontFace( GL_CCW );
	glCullFace( GL_BACK );
	if ( globState.cullMode ) glEnable( GL_CULL_FACE );
	else glDisable( GL_CULL_FACE );

	// Farbmodus oder Beleuchtungsmodus ?
	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT, GL_DIFFUSE );
	if ( globState.lightMode == GL_TRUE ) // Beleuchtung aktivieren
	{
		float m_amb[4] = {0.2, 0.2, 0.2, 1.0};
		float m_diff[4] = {0.2, 0.2, 0.6, 1.0};
		float m_spec[4] = {0.8, 0.8, 0.8, 1.0};
		float m_shine = 32.0;
		float m_emiss[4] = {0.0, 0.0, 0.0, 1.0};

		SetMaterial( GL_FRONT_AND_BACK, m_amb, m_diff, m_spec, m_shine, m_emiss );

		SetLights();

		glEnable( GL_LIGHTING );
	}
	else   // Zeichnen im Farbmodus
	{
		glDisable( GL_LIGHTING );
		glColor4f( 0.2, 0.2, 0.6, 1.0 );
	}


	// Geometrie zeichnen /////////////////!!!!!!!!!!!!!!!!!!!!!!!!///////////////////////
	drawScene();


	// Hilfetext zeichnen
	help.draw();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDisable( GL_CULL_FACE );

	glFlush ();				// Daten an Server (fuer die Darstellung) schicken
	glutSwapBuffers();		// Buffers wechseln
}


/////////////////////////////////////////////////////////////
//	Texturdefinition
/////////////////////////////////////////////////////////////
#define NUM_TEXTURES 8

const char * texture_files[NUM_TEXTURES] = {
									"./textures/mobo2.bmp",
									"./textures/tux.bmp",
									"./textures/meteorit.bmp",
									"./textures/bagger4c.bmp",
									"./textures/ziegelmauer.bmp",
									"./textures/lightmap.bmp",
									"./textures/maske-w.bmp",
									"./textures/maske-s.bmp"
								};

cg_image textures[NUM_TEXTURES];		// die GL Texturobjekte

void initTextures() {

	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		bool success = false;
		if (i > 0 )	// alle außer mobo2.bmp
		{
			success = textures[i].load( texture_files[i], false );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		} else { // mobo2.bmp  -> Mipmap erzeugen
			success = textures[i].load( texture_files[i], true );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}
		if ( !success ) printf( "Can not load texture %s!\n", texture_files[i] );
	}
}


void drawTexQuad( bool linear, bool mipmap )
{
	textures[4].bind();				// Textur VORN wählen

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	if ( linear ) // Taste Y : Lineare Filterung mit "Antialiasing"
		glTexParameteri ( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	else		// Taste Y : einfache Filterung
		glTexParameteri ( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST );

	glEnable( GL_CULL_FACE ); glCullFace( GL_BACK);
	glBegin( GL_QUADS );
		glColor3f( 1, 1, 1 );									// Polygon ist GELB
	    glNormal3f( 0, 0, 1 );									// Normale ist Z
		// die gesamte Textur gestreckt auf das Polygon legen
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -8.0f,-9.6f, 0.0f );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  8.0f,-9.6f, 0.0f );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  8.0f, 9.6f, 0.0f );
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -8.0f, 9.6f, 0.0f );
	glEnd();

	// und nun die Rückseiten
	// WEISS

	textures[0].bind();						// Textur 1 wählen
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	// Farbmultiplikation

	if ( mipmap ) // Mipmapping für den MIN-Filter benutzen ---> Taste X
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	else		// Lineare Filterung benutzen			  ---> Taste X
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glCullFace( GL_FRONT );											// nur Rückseiten zeichnen
	glBegin( GL_QUADS );
		glColor3f( 1.0, 1.0, 1.0 );									// Weiss
	    glNormal3f( 0, 0, -1 );										// Normale = -Z
		const int wiederholungen = 15;								// die Textur mehrfach auf das Polygon legen
		glTexCoord2f( 0.0f, 0.0f);						glVertex3f( -8.0f,-9.6f, 0.0f );
		glTexCoord2f( wiederholungen, 0.0f );			glVertex3f( 8.0f,-9.6f, 0.0f );
		glTexCoord2f( wiederholungen,wiederholungen );	glVertex3f( 8.0f, 9.6f, 0.0f );
		glTexCoord2f( 0.0f,wiederholungen );			glVertex3f( -8.0f, 9.6f, 0.0f );
	glEnd();


	// transparentes Polygon
	glCullFace( GL_BACK );
	glEnable( GL_BLEND );

	// Bagger
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	textures[3].bind();						// Textur 1 wählen

	// maske-s
//	glBlendFunc( GL_ZERO, GL_SRC_COLOR );
//	textures[7].bind();							// Textur 1 wählen

	// maske-w
//	glBlendFunc(GL_SRC_COLOR, GL_ONE);
//	textures[6].bind();						// Textur 1 wählen

	// lightmap
//	glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
//	textures[5].bind();						// Textur 1 wählen


	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );	// Farbmultiplikation

	glDisable( GL_DEPTH_TEST );
	glDepthFunc( GL_ALWAYS );

	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glPushMatrix();
		//glScalef(0.7,0.4,1);

//		glMatrixMode( GL_TEXTURE );
//		glPushMatrix();
//		static float m = 0;
//		glTranslatef( m=m+0.005, 0, 0 );

		glBegin( GL_QUADS );
			glColor3f( 1, 1, 1 );									// Polygon ist GELB
			glNormal3f( 0,0,1 );									// Normale ist Z
			// die gesamte Textur gestreckt auf das Polygon legen
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -8.0f,-9.6f, 0.0f );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  8.0f,-9.6f, 0.0f );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  8.0f, 9.6f, 0.0f );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -8.0f, 9.6f, 0.0f );
		glEnd();

//		glPopMatrix();
//		glMatrixMode( GL_MODELVIEW );

	glPopMatrix();

	glEnable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );
	// Ende transparentes Polygon

	glDisable( GL_CULL_FACE );
}


void drawTexQuadrics(void) {

	glColor3f(1,1,1);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// Quadric erzeugen
	GLUquadricObj *quadric = gluNewQuadric();

		// Quadric texturieren
		gluQuadricNormals(quadric, GLU_OUTSIDE);
		gluQuadricTexture(quadric, GL_TRUE);

		// Texturmodus wählen: GL_MODULATE, GL_REPLACE, GL_BLEND
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// Disk
		glPushMatrix();
			glTranslatef(-7.5, 0.0, 0.0);
			textures[1].bind();  // tux
			gluDisk(quadric, 0.0, 3.0, 35, 10);
		glPopMatrix();

		// Kugel
		glPushMatrix();
			// führen eine Rotation des Zylinders um seine Längsachse durch
			static float winkel = 0.0;
			if (winkel > 360.0) winkel -= 360.0;
			glRotatef(winkel+=0.1, 0,0,1);

			textures[2].bind();  // meteorit
			gluSphere(quadric, 3.0, 35, 35);
		glPopMatrix();

		// Zylinder
		glPushMatrix();
			glTranslatef(7.5,0.0,-5.0);					// Platzieren

			textures[0].bind();  // mobo2

			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
				glLoadIdentity();
				static float f = 0.1;
				f = f + 0.001;
				glTranslatef(f,0,0);
				glScalef(0.5,0.5,1);

				// Geometrie der Quadric zuweisen: Zylinder
				gluCylinder(quadric, 3.5, 1.5, 10.0, 30, 10);
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);

			// Ansichtstransformation holen und gleich wieder auf den Stack legen
		glPopMatrix();

	// Quadric löschen
	gluDeleteQuadric(quadric);

}


void drawScene ()
{
	// Zeichnet die Szene 1x im Weltkoordinatensystem
	static bool linear = false;
	static bool mipmap = false;

	cg_key key;
	if ( 1 == key.keyState( 'y' ) || 1 == key.keyState( 'Y' ) ) linear = !linear;
	if ( 1 == key.keyState( 'x' ) || 1 == key.keyState( 'X' ) ) mipmap = !mipmap;

	glEnable( GL_TEXTURE_2D );		// Texturengine im Rasterizer aktivieren

	drawTexQuad( linear, mipmap );
//	drawTexQuadrics();

	glDisable( GL_TEXTURE_2D );		// Texturengine im Rasterizer deaktivieren
}
