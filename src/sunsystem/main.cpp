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

void drawScene();		// Zeichnet die Szene im Weltkoordinatensystem
void initPlanets(); // Initialisiert die Planeten.

/////////////////////////////////////////
//Einstellungen
/////////////////////////////////////////

//Enthält die Position des aktuell zu sehenden Planeten im Sonnensystem.
static int kamera_mode = 0;

//Enthält einen Wert, der angibt, ob die Sicht vom Planeten auf die Sonne oder nur auf den Planeten zeigt.
static bool view_from_planet_to_sun = false;

//Enthält die Skalierung des Sonnensystems.
static float skaling = 0.1;

//Enthält die SKalierung der Geschwindigkeiten der Planeten um die Sonne und um ihre eigene Achse.
static float velocity_skaling = 2;

//Enthält die Schrittgröße für das verschnellern bzw. verlangsamen der Geschwindigkeit der Planeten.
static float velocity_skaling_steps = 0.2;

//Enthält die maximal mögliche Geschwindigkeit der Planeten.
static float max_velocity = 4.0;

//Enthält einen Wert, der angibt, ob das Sonnensystem animiert werden soll.
static bool animate = true;

//Enthält die Entfernung der Planeten zueinander (von Hülle zu Hülle).
static float planet_distance = 250;

//Enthält die Winkelgeschwindigkeit der Planeten
static float angular_speed = 50;

//Enthält die aktuelle Eigenrotation aller Planeten.
static float planet_angular_rotation = 0;

//Enthält die Anzahl der Planeten incl. Sonne.
const int planet_count = 9;

//Definiert einen Planet.
class Planet;

//Enthält die Instanzen aller Planeten incl. Sonne.
static Planet* planets[planet_count];

//Beschreibt einen Satelliten.
class Satellite {
private:
	// der Satellit setzt sich aus 3 Teilobjekten zusammen.
#define NUM_OBJECTS 3	

	//Anzahl an Texturen.
#define NUM_TEXTURES 3

	//Enthält die Pfade zu den Texturen.
	const char * texture_files[NUM_TEXTURES] = {
		"../../res/textures/gold.bmp",
		"../../res/textures/solar.bmp",
		"../../res/textures/metal.bmp"
	};

	//Enthält den Ordner für die Objekte.
	const char* objects_dir = "../../res/scene/";

	//Enthält die Pfade zu den Objekten.
	const char* objects_paths[NUM_OBJECTS] = { "satellite_rumpf.obj", "satellite_fluegel.obj","satellite_antenne.obj" };

	//Enthält die Texturen der Objekte.
	cg_image textures[NUM_TEXTURES];

	//Enthält die Objekte.
	cg_object3D objects[NUM_OBJECTS];

	//Objektbezeichner für den Zugriff auf die Wavefront Objekte
	enum {
		RUMPF = 0,
		FLUEGEL = 1,
		ANTENNE = 2
	};

public:
	//Lädt die einzelnen Satelliten-Teile
	void loadObjects()
	{
		// alle Objekte Laden, wenn der Pfad verfügbar ist (Pfad != "")
		for (int i = 0; i < NUM_OBJECTS; i++) {
			if (strlen(objects_paths[i]) > 0)
			{
				char file[255] = "";
#ifndef _MSC_VER
				strcat(file, ".");
#endif // _MSC_VER

				strcat(file, objects_dir);
				strcat(file, objects_paths[i]);	// file enthält nun den vollständigen Dateinamen
												// Hier das Objekt laden
				objects[i].load(file, true);
			}
		}
	}

	//Initialisiert die Texturen für den Satelliten.
	void initTextures() {
		for (int i = 0; i < NUM_TEXTURES; i++)
		{
			bool success = false;
			success = textures[i].load(texture_files[i], false);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			if (!success) printf("Can not load texture %s!\n", texture_files[i]);
		}
	}

	//Zeichnet den Satelliten mit den entsprechenden Texturen.
	void draw() {
		float skaling = 0.05;

		glPushMatrix();

		////Ausrichtung des Satelliten
		glRotatef(180, 0, 1, 0);

		glScalef(skaling, skaling, skaling);

		textures[RUMPF].bind();
		objects[RUMPF].draw();
		textures[FLUEGEL].bind();
		objects[FLUEGEL].draw();
		textures[ANTENNE].bind();
		objects[ANTENNE].draw();

		glPopMatrix();
	}
};

//Stellt eine texturierte Kugel dar.
class TexturedSphere
{
private:
	//Enthält die Textur, die an den Planeten gebunden ist.
	cg_image texture;

	//Enthält die Render-Qualität der Kugel.
	const int quality = 100;

public:
	//Enthält den Pfad zur Textur.
	char *texture_path;

	//Enthält den Radius in Tausend Kilometer.
	float radius;

	//Lädt die vorher definierte Textur aus dem Pfad.
	virtual void loadTexture() {
		bool success = false;
		success = texture.load(texture_path, false);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (!success)
			printf("Can not load texture %s!\n", texture_path);
	}

	//Zeichnet den Planeten mit seiner Textur in das Sonnensystem.
	virtual	void draw(GLUquadricObj *quadric) {
		glPushMatrix();

		texture.bind();

		glRotatef(planet_angular_rotation, 0, 1, 0);
		glRotatef(-90, 1, 0, 0);
		gluSphere(quadric, radius * skaling, quality, quality);

		glPopMatrix();
	}
};

//Stellt einen Planeten dar.
class Planet : public TexturedSphere {

protected:
	//Enthält die Entfernung zur Sonne in Tausend Kilometer.
	//Wird pro Planet neu berechnet und ist demnach nicht maßstabsgetreu, da sonst das Sonnensystem zu groß werden würde.
	float dist_to_sun = 0;

public:
	//Enthält den aktuellen Rotations-Grad zur Sonne.
	float sun_rotation = 0;

	//Enthält die Position im Sonnensystem.
	int position;

	//Enthält die Geschwindigkeiten in Tausen Kilometer pro Stunde.
	float velocity;

	//Gibt die Entfernung zur Sonne in Tausend Kilometer zurück
	float getDistToSun() {
		return dist_to_sun;
	}

	//Holt den jeweiligen Planet, der eine Position näher zur Sonne ist.
	Planet* get_predecessor() {
		if (position == 0)
			return 0;
		else
			return planets[position - 1];
	}

	//Zeichnet den Planeten mit seiner Textur in das Sonnensystem.
	virtual	void draw(GLUquadricObj *quadric) {
		if (animate)
			sun_rotation += velocity / 360.0 * velocity_skaling;

		glPushMatrix();

		glRotatef(sun_rotation, 0, 1, 0);

		//Vorgänger holen
		Planet* predecessor = get_predecessor();

		//Berechnet die Entfernung des PLaneten zu Sonne.
		if (predecessor != 0)
			dist_to_sun = predecessor->dist_to_sun + (predecessor->radius + planet_distance + radius) * skaling;

		glTranslatef(dist_to_sun, 0, 0);
		TexturedSphere::draw(quadric);

		glPopMatrix();
	}

};

//Stellt die Erde dar.
class Earth : public Planet
{
public:
	void loadTexture() {
		Planet::loadTexture();

		clouds = new TexturedSphere();
		clouds->radius = radius + 0.1;
		clouds->texture_path = "../../res/textures/clouds.bmp";
		clouds->loadTexture();

		satellite = new Satellite();
		satellite->loadObjects();
		satellite->initTextures();
	}

	//Zeichnet die Erde, samt Wolken und Satelliten.
	void draw(GLUquadricObj *quadric) {
		//Die Wolken werden mit Hilfe von GL_BLEND transparent über die Erde gelegt, dadruch entsteht ein 3D Effekt.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Planet::draw(quadric);

		drawClouds(quadric);

		glDisable(GL_BLEND);

		drawSatellite();
	}

	//Zeichnet die Wolken, die um die Erde kreisen.
	void drawClouds(GLUquadricObj *quadric) {
		glPushMatrix();

		glRotatef(sun_rotation, 0, 1, 0);

		glTranslatef(dist_to_sun, 0, 0);
		glRotatef(planet_angular_rotation, 0, 1, 0);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		clouds->draw(quadric);
		glPopMatrix();
	}

	//Zeichnet den Satellit, der im die Erde kreist.
	void drawSatellite() {
		float distToEarth = 2;
		float satelliteRotationFaktor = 5;
		glPushMatrix();

		glRotatef(sun_rotation, 0, 1, 0);

		glTranslatef(dist_to_sun, 0, 0);

		////Rotation um die Erde
		glRotatef(planet_angular_rotation*satelliteRotationFaktor, 0, 1, 0);
		glTranslatef(distToEarth, 0, 0);

		////Eigenrotation
		glRotatef(planet_angular_rotation*satelliteRotationFaktor, 1, 0, 0);

		satellite->draw();

		glPopMatrix();
	}

private:
	//Enthält die Wolken.
	TexturedSphere* clouds;

	//Enthält den Satelliten.
	Satellite* satellite;
};

//Stellt die Sonne dar.
class Sun : public Planet
{
public:
	//Zeichnet die Sonne, mit ihrem Leuchten und ihrem Licht.
	void draw(GLUquadricObj *quadric) {
		cg_globState globState;

		if (globState.lightMode == GL_TRUE)
		{
			setYellowGlow();
			setSunLight();
		}

		Planet::draw(quadric);
		setToNormal();
	}

private:
	//Setzt das Material der Sonne auf ein gelbes Leuchten.
	void setYellowGlow()
	{
		GLfloat mat1_ambient[] = { 0.2, 0.2, 0.2, 1 };
		GLfloat mat1_diffuse[] = { 0.5, 0.5, 0.1, 1 };
		GLfloat mat1_specular[] = { 1.0, 1.0, 1.0, 1 };
		GLfloat mat1_emission[] = { 0.8, 0.8, 0.2, 1 };
		GLfloat mat1_shininess = 64;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat1_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat1_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat1_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat1_emission);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat1_shininess);
	}

	//Setzt das Licht der Sonne in die Mitte des Koordinatensystems.
	void setSunLight() {
		// Parameter eines globalen Lichts
		GLint g_localviewer = GL_FALSE;
		float g_amb[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

		glEnable(GL_LIGHTING);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, g_localviewer);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, g_amb);

		// Licht 0
		cg_light l(0);
		l.setPosition(0.0f, 0.0f, 0.0f, 1.0f);
		l.setAmbient(0.0f, 0.0f, 0.0f, 0.0f);
		l.setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
		l.setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
		l.setSpotlight(0.0f, 0.0f, 0.0f, 180.0f, 0.0f);
		l.setAttentuation(1.0f, 0.0f, 0.0f);
		l.enable();
		l.draw();
	}

	//Setzt das Material zurück.
	void setToNormal()
	{
		GLfloat mat1_emission_aus[] = { 0, 0, 0, 1 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat1_emission_aus);
	}
};

/////////////////////////////////////////
//	Kamerafunktion
//  Kamera platzieren, siehe Maus - Callbacks
/////////////////////////////////////////
void setCamera()
{
	// "Sonnen"-Ansicht
	if (kamera_mode == 0) {
		cg_mouse mouse;
		// Ansichtstransformationen setzen,
		// SetCamera() zum Beginn der Zeichenfunktion aufrufen
		double x, y, z, The, Phi;
		static double radius = 500;
		// Maus abfragen
		if (cg_mouse::buttonState(GLUT_LEFT_BUTTON))
		{
			cg_globState::cameraHelper[0] += mouse.moveX();
			cg_globState::cameraHelper[1] += mouse.moveY();
		}
		if (cg_mouse::buttonState(GLUT_MIDDLE_BUTTON))
		{
			radius += 0.1 * mouse.moveY();
			if (radius < 1.0) radius = 1.0;
		}
		Phi = 0.2 * cg_globState::cameraHelper[0] / cg_globState::screenSize[0] * M_PI + M_PI * 0.5;
		The = 0.2 * cg_globState::cameraHelper[1] / cg_globState::screenSize[1] * M_PI;
		x = radius * cos(Phi) * cos(The);
		y = radius * sin(The);
		z = radius * sin(Phi) * cos(The);
		int Oben = (The <= 0.5 * M_PI || The > 1.5 * M_PI) * 2 - 1;
		// globale, mausgesteuerte Sicht
		gluLookAt(x, y, z, 0, 0, 0, 0, Oben, 0);
	}
	// "Planeten"-Ansicht
	else
	{
		//Enthält den aktuelle ausgewählten Planet.
		Planet *current_planet = planets[kamera_mode];

		//Hinweis: Dadurch, dass PI nur eine Konstante ist, ist der Kosinus von 1/2 PI nicht null und damit die Sicht auf den Planeten nicht immer genau in der Mitte.
		float pos_x = cos(current_planet->sun_rotation * M_PI / 180.0) * current_planet->getDistToSun();
		float pos_z = -cos((90.0 - current_planet->sun_rotation) * M_PI / 180.0) * current_planet->getDistToSun();

		if (view_from_planet_to_sun) {
			//Sicht auf die Sonne; Kamera auf der Planetoberfläche positioniert
			gluLookAt(pos_x, current_planet->radius * skaling, pos_z, 0, 0, 0, 0, 1, 0);
		}
		else
		{
			//Sicht auf den Planeten in Richtung negativer Z-Achse.
			gluLookAt(pos_x, 0, pos_z + (current_planet->radius * skaling * 10) + 5, pos_x, 0, pos_z, 0, 1, 0);
		}
	}
}

/////////////////////////////////////////
//	Anfang des OpenGL Programmes
/////////////////////////////////////////
int main(int argc, char **argv)
{
	init(argc, argv);

	cg_globState::cameraHelper[1] = 1200;
	cg_globState globState;
	globState.lightMode = true;

	// hier Objekte laden, erstellen etc.
	initPlanets();

	// Die Hauptschleife starten
	glutMainLoop();

	return 0;
}

void displayFunc()
{
	// Hilfe-Instanzen
	cg_help help;
	cg_globState globState;
	cg_key key;

	// Tastatur abfragen
	// Achtung: einmaliges Betätigen funktioniert so nur mit glutIgnoreKeyRepeat(true) (siehe main())
	if (key.keyState(27))
	{
		exit(0); // Escape -> Programm beenden
	}

	else if (1 == key.keyState('f') || 1 == key.keyState('F'))
	{
		help.toggleFps();	// Framecounter on/off
	}
	else if (1 == key.keyState('h') || 1 == key.keyState('H') || 1 == key.specialKeyState(GLUT_KEY_F1))
	{
		help.toggle();	// Hilfetext on/off
	}
	else if (1 == key.keyState('k') || 1 == key.keyState('K'))
	{
		help.toggleKoordsystem();	// Koordinatensystem on/off
	}
	else if (1 == key.keyState('w') || 1 == key.keyState('W'))
	{
		globState.drawMode = (globState.drawMode == GL_FILL) ? GL_LINE : GL_FILL; // Wireframe on/off
	}
	else if (1 == key.keyState('l') || 1 == key.keyState('L'))
	{
		globState.lightMode = !globState.lightMode;	// Beleuchtung on/off
	}
	else if (1 == key.keyState('c') || 1 == key.keyState('C'))
	{
		globState.cullMode = !globState.cullMode; // Backfaceculling on/off
	}
	else if (1 == key.keyState('i') || 1 == key.keyState('I'))
	{
		globState.cameraHelper[0] = 0;	// Initialisierung der Kamera
		globState.cameraHelper[1] = 0;
	}

	// Szene zeichnen: CLEAR, SETCAMERA, DRAW_SCENE

	// Back-Buffer neu initialisieren
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Kamera setzen (spherische Mausnavigation)
	setCamera();

	// Koordinatensystem zeichnen
	help.drawKoordsystem(-8, 10, -8, 10, -8, 10);

	// Zeichenmodus einstellen (Wireframe on/off)
	glPolygonMode(GL_FRONT_AND_BACK, globState.drawMode);

	// Backface Culling on/off;
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	if (globState.cullMode) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	// Farbmodus oder Beleuchtungsmodus ?
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	if (globState.lightMode == GL_TRUE) // Beleuchtung aktivieren
	{
		float m_amb[4] = { 0.2, 0.2, 0.2, 1.0 };
		float m_diff[4] = { 0.2, 0.2, 0.6, 1.0 };
		float m_spec[4] = { 0.8, 0.8, 0.8, 1.0 };
		float m_shine = 32.0;
		float m_emiss[4] = { 0.0, 0.0, 0.0, 1.0 };

		SetMaterial(GL_FRONT_AND_BACK, m_amb, m_diff, m_spec, m_shine, m_emiss);

		glEnable(GL_LIGHTING);
	}
	else   // Zeichnen im Farbmodus
	{
		glDisable(GL_LIGHTING);
		glColor4f(0.2, 0.2, 0.6, 1.0);
	}

	// Geometrie zeichnen
	drawScene();

	// Hilfetext zeichnen
	help.draw();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	glFlush();				// Daten an Server (fuer die Darstellung) schicken
	glutSwapBuffers();		// Buffers wechseln
}

//Initialisiert die Planeten mit den entsprechenden Werten.
void initPlanets() {
	//Sonne
	planets[0] = new Sun();
	planets[0]->texture_path = "../../res/textures/sonne.bmp";
	planets[0]->velocity = 0;
	planets[0]->radius = 696;
	planets[0]->position = 0;
	planets[0]->loadTexture();

	//Merkur
	planets[1] = new Planet();
	planets[1]->texture_path = "../../res/textures/merkur.bmp";
	planets[1]->velocity = 172;
	planets[1]->radius = 2.5;
	planets[1]->loadTexture();
	planets[1]->position = 1;

	//Venus
	planets[2] = new Planet();
	planets[2]->texture_path = "../../res/textures/venus.bmp";
	planets[2]->velocity = 126;
	planets[2]->radius = 6;
	planets[2]->loadTexture();
	planets[2]->position = 2;

	//Erde
	planets[3] = new Earth();
	planets[3]->texture_path = "../../res/textures/erde.bmp";
	planets[3]->velocity = 107;
	planets[3]->radius = 6;
	planets[3]->loadTexture();
	planets[3]->position = 3;

	//Mars
	planets[4] = new Planet();
	planets[4]->texture_path = "../../res/textures/mars.bmp";
	planets[4]->velocity = 87;
	planets[4]->radius = 3.5;
	planets[4]->loadTexture();
	planets[4]->position = 4;

	//Jupiter
	planets[5] = new Planet();
	planets[5]->texture_path = "../../res/textures/jupiter.bmp";
	planets[5]->velocity = 47;
	planets[5]->radius = 69;
	planets[5]->loadTexture();
	planets[5]->position = 5;

	//Saturn
	planets[6] = new Planet();
	planets[6]->texture_path = "../../res/textures/saturn.bmp";
	planets[6]->velocity = 35;
	planets[6]->radius = 57.5;
	planets[6]->loadTexture();
	planets[6]->position = 6;

	//Uranus
	planets[7] = new Planet();
	planets[7]->texture_path = "../../res/textures/uranus.bmp";
	planets[7]->velocity = 25;
	planets[7]->radius = 25.5;
	planets[7]->loadTexture();
	planets[7]->position = 7;

	//Neptun
	planets[8] = new Planet();
	planets[8]->texture_path = "../../res/textures/neptun.bmp";
	planets[8]->velocity = 20;
	planets[8]->radius = 24.5;
	planets[8]->loadTexture();
	planets[8]->position = 8;
}

// Zeichnet die Szene 1x im Weltkoordinatensystem
void drawScene()
{
	cg_key key;

	//Nach-Links-Taste -> zum vorherigen Planeten wechseln.
	if (1 == key.specialKeyState(GLUT_KEY_LEFT))
	{
		kamera_mode--;
		if (kamera_mode < 0)
			kamera_mode = planet_count - 1;
	}
	//Nach-Rechts-Taste -> zum nachfolgenden Planeten wechseln.
	else if (1 == key.specialKeyState(GLUT_KEY_RIGHT))
	{
		kamera_mode++;
		if (kamera_mode >= planet_count)
			kamera_mode = 0;
	}
	//Nach-Oben-Taste -> Planetengeschwindigkeit verschnellern.
	else if (1 == key.specialKeyState(GLUT_KEY_UP))
	{
		velocity_skaling += velocity_skaling_steps;

		if (velocity_skaling > max_velocity)
		{
			velocity_skaling = max_velocity;
		}
	}
	////Sicht auf Planet oder auf Sonne von Planet aus.
	else if (1 == key.keyState('x') || 1 == key.keyState('X'))
	{
		view_from_planet_to_sun = !view_from_planet_to_sun;
	}
	//Nach-Unten-Taste -> Planetengeschwindigkeit verlangsamen.
	else if (1 == key.specialKeyState(GLUT_KEY_DOWN))
	{
		velocity_skaling -= velocity_skaling_steps;

		if (velocity_skaling < 0)
		{
			velocity_skaling = 0;
		}
	}
	//Animation ein-/ausschalten.
	else if (1 == key.keyState('a') || 1 == key.keyState('A'))
	{
		if (kamera_mode == 0) {
			animate = !animate;
		}
	}

	// Texturengine im Rasterizer aktivieren
	glEnable(GL_TEXTURE_2D);

	glColor3f(1, 1, 1);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// Quadric erzeugen
	GLUquadricObj *quadric = gluNewQuadric();

	// Quadric texturieren
	gluQuadricNormals(quadric, GLU_OUTSIDE);
	gluQuadricTexture(quadric, GL_TRUE);

	// Texturmodus wählen: GL_MODULATE, GL_REPLACE, GL_BLEND
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (animate) {
		planet_angular_rotation += angular_speed / 360.0 * velocity_skaling;
	}

	//Alle Planeten zeichnen.
	for (int i = 0; i < planet_count; i++)
	{
		planets[i]->draw(quadric);
	}

	glMatrixMode(GL_MODELVIEW);
	gluDeleteQuadric(quadric);

	// Texturengine im Rasterizer deaktivieren
	glDisable(GL_TEXTURE_2D);	
}