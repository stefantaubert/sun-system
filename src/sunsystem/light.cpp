#include "light.h"



void SetMaterial ( GLenum face, GLfloat amb[4], GLfloat diff[4], GLfloat spec[4], GLfloat shine, GLfloat emis[4] )
// Aktualisierung des OpenGL Materials
{
	glMaterialfv ( face, GL_AMBIENT, amb );
	glMaterialfv ( face, GL_DIFFUSE, diff );
	glMaterialfv ( face, GL_SPECULAR, spec );
	glMaterialf  ( face, GL_SHININESS, shine );
	glMaterialfv ( face, GL_EMISSION, emis );
}


void SetLights ()
{
	// Parameter eines globalen Lichts
	GLint g_localviewer = GL_FALSE;
	float g_amb[4] = {0.2f, 0.2f, 0.2f, 1.0f};

	glEnable( GL_LIGHTING );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, g_localviewer );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, g_amb );

	// Licht 0
	cg_light l ( 0 );
	l.setPosition( -1.0f, 1.0f, 1.0f, 0.0f ); // paralles Licht
//	l.setPosition( -5.0f, 5.0f, 5.0f, 1.0f ); // Punktlicht
	l.setAmbient( 0.0f, 0.0f, 0.0f, 0.0f );
	l.setDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
	l.setSpecular( 1.0f, 1.0f, 1.0f, 1.0f );
	l.setSpotlight( 0.0f, 0.0f, 0.0f, 180.0f, 0.0f );
	l.setAttentuation( 1.0f, 0.0f, 0.0f );
	l.enable();
	l.draw();
}



cg_light::cg_light ( int num )
{
	id = GL_LIGHT0 + num;
	setPosition( 0.0f, 0.0f, 1.0f, 0.0f );
	setRotation( 0.0f, 0.0f );
	setAmbient( 0.0f, 0.0f, 0.0f, 1.0f );
	setDiffuse( 1.0f, 1.0f, 1.0f, 1.0f );
	setSpecular( 1.0f, 1.0f, 1.0f, 1.0f );
	setSpotlight( 0.0f, -1.0f, 0.0f, 180.0f, 0.0f );
	setAttentuation( 1.0f, 0.0f, 0.0f );
	disable();
}


void cg_light::setPosition ( float x, float y, float z, float w )
{
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	pos[3] = w;
}

void cg_light::setRotation ( float horizontal, float vertikal )
{
    rot[0] = horizontal;
    rot[1] = vertikal;
}


void cg_light::setAmbient ( float r, float g, float b, float a )
{
	amb[0] = r;
	amb[1] = g;
	amb[2] = b;
	amb[3] = a;
}


void cg_light::setDiffuse ( float r, float g, float b, float a )
{
	diff[0] = r;
	diff[1] = g;
	diff[2] = b;
	diff[3] = a;
}


void cg_light::setSpecular ( float r, float g, float b, float a )
{
	spec[0] = r;
	spec[1] = g;
	spec[2] = b;
	spec[3] = a;
}


void cg_light::setSpotlight ( float directionX, float directionY, float directionZ, float cutoff, float exponent )
{
	spot[0] = directionX;
	spot[1] = directionY;
	spot[2] = directionZ;
	spot[3] = cutoff;
	spot[4] = exponent;
}


void cg_light::setAttentuation ( float constant, float linear, float quadric )
{
	att[0] = constant;
	att[1] = linear;
	att[2] = quadric;
}


void cg_light::draw ()
{
	if ( enabled )
	{
		glPushMatrix();
			// rotation
			glRotatef( rot[0], 0, 1, 0);
			glRotatef( rot[1], 1, 0, 0);
			// position
			glLightfv ( id, GL_POSITION, pos );
			// color
			glLightfv ( id, GL_AMBIENT, amb );
			glLightfv ( id, GL_DIFFUSE, diff );
			glLightfv ( id, GL_SPECULAR, spec );
			// spotlight
			glLightfv ( id, GL_SPOT_DIRECTION, spot );
			glLightf  ( id, GL_SPOT_CUTOFF, spot[3] );
			glLightf  ( id, GL_SPOT_EXPONENT, spot[4] );
			// attentuation
			glLightf  ( id, GL_CONSTANT_ATTENUATION,  att[0] );
			glLightf  ( id, GL_LINEAR_ATTENUATION,    att[1] );
			glLightf  ( id, GL_QUADRATIC_ATTENUATION, att[2] );
			// enable
			glEnable( id );
		glPopMatrix();
	}
	else
		glDisable( id );
}


void cg_light::enable ()
{
	enabled = true;
}


void cg_light::disable ()
{
	enabled = false;
}


void cg_light::toggle ()
{
	enabled = !enabled;
}
