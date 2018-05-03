#include "stdafx.h"
#include "GL_Game.h"
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "irrKlang.h"

#define PI 3.14159265358979323846

using namespace std;
using namespace irrklang;

//-------------------------------

int fullscreen = 1;
int muzyka;

int automatyczne_chodzenie = 1;
float dokladnosc = 0.7;

//int czas = 1000000;
int czas = 250;

//-------------------------------

int i = 0;
int j = 0;
int turbo = 0;
int latarka = 0;
float odchylenie = 0;
unsigned long frame = 0;
int koniec = 0;
int menu = 1;
int odpalona = 0;
int odpalona_koniec = 0;

ISoundEngine *SoundEngine = createIrrKlangDevice();
ISoundSource *coinSrc = SoundEngine->addSoundSourceFromFile("audio/Coin.mp3");
ISoundSource *gitaraSrc = SoundEngine->addSoundSourceFromFile("audio/nie-jestem-fanem-piwa.mp3");
ISoundSource *akordeonSrc = SoundEngine->addSoundSourceFromFile("audio/gleba.mp3");
ISoundSource *menuSrc = SoundEngine->addSoundSourceFromFile("audio/menu.mp3");
ISoundSource *akordeonSrc2 = SoundEngine->addSoundSourceFromFile("audio/gleba2.mp3");
ISoundSource *boostSrc = SoundEngine->addSoundSourceFromFile("audio/boost2.mp3");
ISoundSource *winSrc = SoundEngine->addSoundSourceFromFile("audio/Ajajaj.mp3");


ISound *muza_menu;

vec3 Cube;
vec3 Cube2;
vec3 Cube_next;

GLuint plane;


#pragma region Tekstura
GLuint tex_top;
GLuint tex_front;
GLuint tex_left;
GLuint tex_bottom;
GLuint tex_right;
GLuint tex_back;
GLuint menu_tex;
GLuint gold_tex;
GLuint metal_tex;
#pragma endregion


void print(float x, float y, void *font, char *string)
{
	char *c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}
}

struct SFace {
	int v[3];
	int n[3];
	int t[3];
};

GLuint LoadObj(char * file) {

	FILE * fp = fopen(file, "r");

	if (fp == NULL) {
		printf("ERROR: Cannot read model file \"%s\".\n", file);
		return -1;
	}

	std::vector<vec3> * v = new std::vector<vec3>();
	std::vector<vec3> * n = new std::vector<vec3>();
	std::vector<vec3> * t = new std::vector<vec3>();
	std::vector<SFace> * f = new std::vector<SFace>();

	char buf[128];

	while (fgets(buf, 128, fp) != NULL) {
		if (buf[0] == 'v' && buf[1] == ' ') {
			vec3 * vertex = new vec3();
			sscanf(buf, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
			v->push_back(*vertex);
		}
		if (buf[0] == 'v' && buf[1] == 't') {
			vec3 * vertex = new vec3();
			sscanf(buf, "vt %f %f", &vertex->x, &vertex->y);
			t->push_back(*vertex);
		}
		if (buf[0] == 'v' && buf[1] == 'n') {
			vec3 * vertex = new vec3();
			sscanf(buf, "vn %f %f %f", &vertex->x, &vertex->y, &vertex->z);
			n->push_back(*vertex);
		}
		if (buf[0] == 'f' && buf[1] == ' ') {
			SFace * face = new SFace();
			sscanf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&face->v[0], &face->t[0], &face->n[0],
				&face->v[1], &face->t[1], &face->n[1],
				&face->v[2], &face->t[2], &face->n[2]
			);
			f->push_back(*face);
		}
	}

	fclose(fp);

	GLuint dlId;
	dlId = glGenLists(1);
	glNewList(dlId, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < f->size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			vec3 * cv = &(*v)[((*f)[i].v[j] - 1)];
			vec3 * ct = &(*t)[((*f)[i].t[j] - 1)];
			vec3 * cn = &(*n)[((*f)[i].n[j] - 1)];
			glTexCoord2f(ct->x, ct->y);
			glNormal3f(cn->x, cn->y, cn->z);
			glVertex3f(cv->x, cv->y, cv->z);
		}
	}
	glEnd();
	glEndList();

	delete v;
	delete n;
	delete t;
	delete f;

	plane = dlId;

	return dlId;

}


// Funkcja odczytująca bitmapę i tworząca na jej podstawie teksturę z zadanym rodzajem filtracji
GLuint LoadTexture(char * file, int magFilter, int minFilter) {

	// Odczytanie bitmapy
	Bitmap *tex = new Bitmap();
	if (!tex->loadBMP(file)) {
		printf("ERROR: Cannot read texture file \"%s\".\n", file);
		return -1;
	}

	// Utworzenie nowego id wolnej tekstury
	GLuint texId;
	glGenTextures(1, &texId);

	// "Bindowanie" tekstury o nowoutworzonym id
	glBindTexture(GL_TEXTURE_2D, texId);

	// Określenie parametrów filtracji dla tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter); // Filtracja, gdy tekstura jest powiększana
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter); // Filtracja, gdy tekstura jest pomniejszana

																	  // Wysłanie tekstury do pamięci karty graficznej zależnie od tego, czy chcemy korzystać z mipmap czy nie
	if (minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST) {
		// Automatyczne zbudowanie mipmap i wysłanie tekstury do pamięci karty graficznej
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, tex->width, tex->height, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
	}
	else {
		// Wysłanie tekstury do pamięci karty graficznej 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
	}

	// Zwolnienie pamięci, usunięcie bitmapy z pamięci - bitmapa jest już w pamięci karty graficznej
	delete tex;

	// Zwrócenie id tekstury
	return texId;
}

void LoadTextures() {
	tex_back = LoadTexture("skybox\\back.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	tex_bottom = LoadTexture("skybox\\bottom.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	tex_front = LoadTexture("skybox\\front.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	tex_left = LoadTexture("skybox\\left.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	tex_right = LoadTexture("skybox\\right.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	tex_top = LoadTexture("skybox\\top.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	menu_tex = LoadTexture("Menu\\Main2.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	gold_tex = LoadTexture("textures\\gold1.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	metal_tex = LoadTexture("textures\\metal.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
}

void Draw_Skybox()
{

	// Włączamy teksturowanie
	glEnable(GL_TEXTURE_2D);

	// Ustawienie sposobu teksturowania - GL_MODULATE sprawia, że światło ma wpływ na teksturę; GL_DECAL i GL_REPLACE rysują teksturę tak jak jest
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Ustawienie materiału
	float m_amb[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float m_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_spe[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_spe);

	// Wybór tekstury korzystając z jej id
	glBindTexture(GL_TEXTURE_2D, tex_front);

	glBegin(GL_QUADS);

	// Przod
	glNormal3f(0.0f, 0.0f, 1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glEnd();


	// Wybór tekstury korzystając z jej id
	glBindTexture(GL_TEXTURE_2D, tex_back);

	glBegin(GL_QUADS);

	// Tyl
	glNormal3f(0.0f, 0.0f, -1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.002f, 1.0f, -1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.01f, 1.0f, -1.0f);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.01f, -1.0f, -1.0f);
	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.002f, -1.0f, -1.0f);

	glEnd();


	// Wybór tekstury korzystając z jej id
	glBindTexture(GL_TEXTURE_2D, tex_left);

	glBegin(GL_QUADS);

	// Lewa
	glNormal3f(-1.0f, 0.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);

	glEnd();


	// Wybór tekstury korzystając z jej id
	glBindTexture(GL_TEXTURE_2D, tex_right);

	glBegin(GL_QUADS);

	// Prawa
	glNormal3f(1.0f, 0.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.01f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.01f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.01f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.01f);

	glEnd();

	// Wybór tekstury korzystając z jej id
	glBindTexture(GL_TEXTURE_2D, tex_top);

	glBegin(GL_QUADS);

	// Gora
	glNormal3f(0.0f, 1.0f, 0.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 0.99f, 1.0f);
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 0.99f, 1.0f);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.99f, -1.0f);
	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 0.99f, -1.0f);

	glEnd();


	// Wybór tekstury korzystając z jej id
	glBindTexture(GL_TEXTURE_2D, tex_bottom);

	glBegin(GL_QUADS);

	// Dol
	glNormal3f(0.0f, -1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glEnd();

	glPopMatrix();




}


void Rysuj_menu()
{

	// Włączamy teksturowanie
	glEnable(GL_TEXTURE_2D);

	// Ustawienie sposobu teksturowania - GL_MODULATE sprawia, że światło ma wpływ na teksturę; GL_DECAL i GL_REPLACE rysują teksturę tak jak jest
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// Ustawienie materiału
	float m_amb[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float m_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_spe[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_spe);

	// Wybór tekstury korzystając z jej id
	glBindTexture(GL_TEXTURE_2D, menu_tex);

	glBegin(GL_QUADS);

	// Tyl
	glNormal3f(0.0f, 0.0f, -1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.002f, 1.0f, -1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.01f, 1.0f, -1.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.01f, -1.0f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.002f, -1.0f, -1.0f);

	glEnd();
}

void Odpal_Muzyke(int flaga)
{
	if (flaga == 0)
	{
		if (muzyka == 1)
			ISound *gitara = SoundEngine->play2D(gitaraSrc, true/*playLooped*/, false/*startPaused*/, true/*trackSound*/);
		else if (muzyka == 2)
			ISound *akordeon = SoundEngine->play2D(akordeonSrc, true/*playLooped*/, false/*startPaused*/, true/*trackSound*/);

		odpalona = 1;
	}

	else if (flaga == 1)
	{
		SoundEngine->stopAllSounds();
		ISound *akordeon2 = SoundEngine->play2D(akordeonSrc2, true/*playLooped*/, false/*startPaused*/, true/*trackSound*/);
		odpalona_koniec = 1;
	}

	else if (flaga == 2)
	{
		SoundEngine->stopAllSounds();
		ISound *win = SoundEngine->play2D(winSrc, true/*playLooped*/, false/*startPaused*/, true/*trackSound*/);
		odpalona_koniec = 1;
	}
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	//glutInitWindowSize(1366, 768);
	glutInitWindowSize(640, 320);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("Highflyer");
	
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnReshape);
	glutKeyboardFunc(OnKeyPress);
	glutKeyboardUpFunc(OnKeyUp);
	glutSpecialFunc(OnSpecialKeyPress); // klawisze specjalne (strzalki, F1-F12, PgUp/PgDn, Home, End, Delete, Insert)
	glutSpecialUpFunc(OnSpecialKeyUp);
	glutTimerFunc(10, OnTimer, 0);

	if(fullscreen)
	glutFullScreen();

	glEnable(GL_DEPTH_TEST);
	

	// Ustawiamy komponent ambient naszej sceny - wartosc niezalezna od swiatla (warto zresetowac)
	float gl_amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gl_amb);

	glEnable(GL_CULL_FACE); // Włączenie cullingu - rysowania tylko jednej strony wielokątów
	glCullFace(GL_BACK); // Określenie, którą stronę wielokątów chcemy ukrywać
	glFrontFace(GL_CCW); // Określenie, jaki kierunek definicji wierzchołków oznacza przód wielokątu (GL_CCW - przeciwnie do ruchu wskazówek zegara, GL_CW - zgodnie)
	glEnable(GL_LIGHTING); // Wlaczenie oswietlenia
	glShadeModel(GL_SMOOTH); // Wybor techniki cieniowania
	glEnable(GL_LIGHT0); // Wlaczenie 0-go zrodla swiatla
	glEnable(GL_LIGHT3); // Wlaczenie 3-go zrodla swiatla

	// Inicjalizacja stanu kamery:

	player.pos.x =  0.0f;
	player.pos.y = 1.75;
	player.pos.z = 10.0f;
	player.dir.x = 0.0f;
	player.dir.y = 0.0f;
	player.dir.z = -1.0f;
	player.speed = 0.03f;

	LoadObj("Model/A102.obj");
	LoadTextures(); // Każdą teksturę ładujemy *raz* (nie w każdej klatce!), np. przed wejściem do pętli głównej

	muza_menu = SoundEngine->play2D(menuSrc, true/*playLooped*/, false/*startPaused*/, true/*trackSound*/);

	glutMainLoop();

	return 0;
}


#pragma region Obsluga wejscia

bool keystate[256];
bool keystate_special[256];

void OnKeyPress(unsigned char key, int x, int y) 
	{
		if (!keystate[key]) 
			OnKeyDown(key, x, y);

		keystate[key] = true;
	}

void OnSpecialKeyPress(int key, int x, int y) 
	{
		if (!keystate_special[key])
			OnSpecialKeyDown(key, x, y);

		keystate_special[key] = true;
	}

void OnKeyDown(unsigned char key, int x, int y)
{
	if (key == 27)
		glutLeaveMainLoop();
	if (key == 'l' || key == 'L')
	{
		if (!latarka)
		{
			glEnable(GL_LIGHT1); // Wlaczenie 1-go zrodla swiatla
			latarka = 1;
		}
		else
		{
			glDisable(GL_LIGHT1);
			latarka = 0;
		}
	}
	if (key == 'c' || key == 'C')
	{
		if (turbo > 0)
		{
			ISound *boost = SoundEngine->play2D(boostSrc, false/*playLooped*/, false/*startPaused*/, true/*trackSound*/);
			player.speed += 0.5;
			turbo--;
		}
	}
}

void OnSpecialKeyDown(int key, int x, int y)
{

}

void OnKeyUp(unsigned char key, int x, int y) 
	{
		keystate[key] = false;
	}

void OnSpecialKeyUp(int key, int x, int y) 
	{
		keystate_special[key] = false;
	}

#pragma endregion


void Mysz(int x, int y)
{
	if (menu != 1)
	{
		float phi = atan2(player.dir.z, player.dir.x);

		float zmienna = 0.02;

		if (y > glutGet(GLUT_WINDOW_HEIGHT) / 2)
			if (player.dir.y < 1)
				player.dir.y += zmienna;

		if (y < glutGet(GLUT_WINDOW_HEIGHT) / 2)
			if (player.dir.y > -1)
				player.dir.y -= zmienna;

		if (x > glutGet(GLUT_WINDOW_WIDTH) / 2)					//prawo
		{
			phi += zmienna;

			if (odchylenie > -40)
				odchylenie -= 2;

			else
			{
				if (odchylenie < 0)
					odchylenie++;
			}

		}

		if (x < glutGet(GLUT_WINDOW_WIDTH) / 2)		//lewo
		{
			phi -= zmienna;

			if (odchylenie < 40)
				odchylenie += 2;


			else
			{
				if (odchylenie > 0)
					odchylenie--;
			}
		}

		player.dir.x = cos(phi);
		player.dir.z = sin(phi);
	}
}	

// Aktualizacja stanu gry - wywoływana za pośrednictwem zdarzenia-timera.
void OnTimer(int id) {

	// Chcemy, by ta funkcja została wywołana ponownie za 10ms.
	glutTimerFunc(10, OnTimer, 0);

	glutSetCursor(GLUT_CURSOR_NONE);
	glutPassiveMotionFunc(Mysz);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);


#pragma region Poruszanie

	if (koniec == 0 && menu != 1)
	{

		//automatyczne chodzenie:

		if (automatyczne_chodzenie)
		{
			player.pos.x += player.dir.x * player.speed;
			player.pos.y += player.dir.y * player.speed;
			player.pos.z += player.dir.z * player.speed;
		}

		// Chodzenie do przodu:
		if (keystate['w'] || keystate['W'])
		{
			player.pos.x += player.dir.x * player.speed;
			player.pos.y += player.dir.y * player.speed;
			player.pos.z += player.dir.z * player.speed;
		}

		// Chodzenie do tyłu:
		if (keystate['s'] || keystate['S'])
		{
			player.pos.x -= player.dir.x * player.speed / 2;
			player.pos.y -= player.dir.y * player.speed / 2;
			player.pos.z -= player.dir.z * player.speed / 2;
		}

		float phi = atan2(player.dir.z, player.dir.x);

		//Obrót w prawo:
		if (keystate['d'] || keystate['D'])
		{
			phi += player.speed / 2;

			if (odchylenie > -40)
				odchylenie--;
		}

		else
		{
			if (odchylenie < 0)
				odchylenie++;
		}

		//Obrót w lewo:
		if (keystate['a'] || keystate['A'])
		{
			phi -= player.speed / 2;

			if (odchylenie < 40)
				odchylenie++;
		}

		else
		{
			if (odchylenie > 0)
				odchylenie--;
		}

		player.dir.x = cos(phi);
		player.dir.z = sin(phi);


		//W dół
		if (keystate_special[GLUT_KEY_UP])
			if (player.dir.y > -1)
				player.dir.y -= player.speed;

		//W góre
		if (keystate_special[GLUT_KEY_DOWN])
			if (player.dir.y < 1)
				player.dir.y += player.speed;

		//reset

		if (keystate['r'] || keystate['R'])
		{
			player.pos.x = 0.0f;
			player.pos.y = 1.75;
			player.pos.z = 10.0f;
			player.dir.x = 0.0f;
			player.dir.y = 0.0f;
			player.dir.z = -1.0f;
			player.speed = 0.03f;
		}


		//fullscreen
		if (keystate['f'] || keystate['F'])
			glutFullScreen();
	}

}
#pragma endregion

void OnRender() 
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Ustawienie kamery na podstawie jej stanu przechowywanego w zmiennej player.
	gluLookAt(
		player.pos.x - 2 * player.dir.x, player.pos.y - 2 * player.dir.y, player.pos.z - 2 * player.dir.z, // Pozycja kamery
		player.pos.x, player.pos.y, player.pos.z, // Punkt na ktory patrzy kamera (pozycja + kierunek)
		0.0f, 1.0f, 0.0f // Wektor wyznaczajacy pion
	);

	if (menu == 1)
	{
		glTranslatef(0.0f,2.0f,0.0f);
		glScalef(15.0f,10.0f,0.0f);
		Rysuj_menu();
		if (keystate_special[GLUT_KEY_F1])
		{
			menu = 0;
			muzyka = 1;
		}

		else if (keystate_special[GLUT_KEY_F2])
		{
			menu = 0;
			muzyka = 2;
		}
		
	}

	else
	{
		muza_menu->stop();
		if (odpalona == 0)
			Odpal_Muzyke(0);

#pragma region Swiatlo_0

		float l0_amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float l0_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float l0_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float l0_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
		glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spe);
		glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);

		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

#pragma endregion


#pragma region Swiatlo_1(latarka)

		float l1_amb[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		float l1_dif[] = { 0.3f, 0.3f, 0.3f, 1.0f };
		float l1_spe[] = { 0.3f, 0.3f, 0.3f, 1.0f };
		float l1_pos[] = { player.pos.x, player.pos.y, player.pos.z, 1.0f };

		glLightfv(GL_LIGHT1, GL_AMBIENT, l1_amb);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, l1_dif);
		glLightfv(GL_LIGHT1, GL_SPECULAR, l1_spe);
		glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);

		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

		float lightDir[3] = { player.dir.x, player.dir.y, player.dir.z };
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDir);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0f);

#pragma endregion

#pragma region Swiatlo_3(Cube)

		float l3_amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float l3_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float l3_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float l3_pos[] = { Cube.x, Cube.y, Cube.z, 1.0f };

		glLightfv(GL_LIGHT3, GL_AMBIENT, l3_amb);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, l3_dif);
		glLightfv(GL_LIGHT3, GL_SPECULAR, l3_spe);
		glLightfv(GL_LIGHT3, GL_POSITION, l3_pos);

		glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 2.0f);
		glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 2.0f);
		glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 2.0f);

#pragma endregion


#pragma region Zielony

		float grass_amb[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		float grass_dif[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		float grass_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT, grass_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, grass_dif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, grass_spe);
		glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

#pragma endregion

		// Narysowanie "siatki" złożonej ze 121 kolorowych sfer.
		for (int ix = -20; ix <= 20; ix += 1) {
			for (int iz = -20; iz <= 20; iz += 1) {
				glColor3f(.5f + .1f * ix, .5f - .1f * iz, 0.0f);
				glPushMatrix();
				glTranslatef(ix, 1.0f, iz);
				glutSolidSphere(.05f, 8, 8);
				glPopMatrix();
			}
		}


#pragma region Samolot

		// Włączamy teksturowanie
		glEnable(GL_TEXTURE_2D);

		// Ustawienie sposobu teksturowania - GL_MODULATE sprawia, że światło ma wpływ na teksturę; GL_DECAL i GL_REPLACE rysują teksturę tak jak jest
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		float m3_amb[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		float m3_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float m3_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT, m3_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m3_dif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, m3_spe);
		glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);

		// Wybór tekstury korzystając z jej id
		glBindTexture(GL_TEXTURE_2D, metal_tex);

		glPushMatrix();

		glTranslatef(player.pos.x + player.dir.x, player.pos.y + player.dir.y, player.pos.z + player.dir.z);

		glScalef(0.09, 0.09, 0.09);

		float phi = atan2(player.dir.z, player.dir.x);

		glRotatef(-90 - phi / PI * 180, 0, 1, 0);
		glRotatef(player.dir.y * 45, 1, 0, 0);

		glRotatef(odchylenie, 0, 0, 1);

		glCallList(plane);

		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
#pragma endregion


		//trasa

		const int dlugosc_trasy = 18;

		float punkty_x[dlugosc_trasy] = { 0,1,3,5,6.5,6,3,1,-1,-3,-5,-6,-6,-6,-6,-4,-4,-4 };
		float punkty_y[dlugosc_trasy] = { 3,4,5,4,3,2,3,4,3,3,2,2,3,4,5,4,3,2 };
		float punkty_z[dlugosc_trasy] = { 3,1,-1,-3,-5,-7, -8, -7.5,-7.5,-7,-6,-4, -2,0, 2,5,7,9 };

#pragma region Gold

		// Włączamy teksturowanie
		glEnable(GL_TEXTURE_2D);

		// Ustawienie sposobu teksturowania - GL_MODULATE sprawia, że światło ma wpływ na teksturę; GL_DECAL i GL_REPLACE rysują teksturę tak jak jest
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// Ustawienie materiału
		float m_amb[] = { 0.7f, 0.7f, 0.7f, 1.0f };
		float m_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float m_spe[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, m_dif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, m_spe);

		// Wybór tekstury korzystając z jej id
		glBindTexture(GL_TEXTURE_2D, gold_tex);

#pragma endregion

#pragma region Do_Zbierania

		glPushMatrix();
		Cube.x = punkty_x[i];
		Cube.y = punkty_y[i];
		Cube.z = punkty_z[i];

		if (
			(player.pos.x >= Cube.x - dokladnosc && player.pos.x <= Cube.x + dokladnosc) &&
			(player.pos.y >= Cube.y - dokladnosc / 2 && player.pos.y <= Cube.y + dokladnosc / 2) &&
			(player.pos.z >= Cube.z - dokladnosc && player.pos.z <= Cube.z + dokladnosc) &&
			i < dlugosc_trasy && koniec == 0)
		{
			ISound *coin = SoundEngine->play2D(coinSrc, false/*playLooped*/, false/*startPaused*/, true/*trackSound*/);

			czas += 70;
			i++;
		}

		if (i == dlugosc_trasy)
		{
			koniec = 1;
			automatyczne_chodzenie = 0;
			glTranslatef(player.pos.x, player.pos.y, player.pos.z);

			if (odpalona_koniec == 0)
				Odpal_Muzyke(2);

			char buffer1[30] = { '\0' };
			sprintf(buffer1, "Wygrana!Twoj wynik:%d", czas);	// konwersja na string
			print(0, 0, GLUT_BITMAP_TIMES_ROMAN_24, buffer1);
		}

		glTranslatef(Cube.x, Cube.y, Cube.z);
		glRotatef(3 * frame, 0.0f, 1.0f, 0.0f);
		glutSolidCube(0.35);

		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
#pragma endregion 


#pragma region Cube_next

		int next;
		next = i + 1;

		if (next == dlugosc_trasy)
			next = 0;

		glPushMatrix();
		Cube_next.x = punkty_x[next];
		Cube_next.y = punkty_y[next];
		Cube_next.z = punkty_z[next];

		glTranslatef(Cube_next.x, Cube_next.y, Cube_next.z);
		glutWireCube(0.35);

		glPopMatrix();


#pragma endregion



#pragma region Bonusy
		const int bonusy = 5;

		float p_x[bonusy] = { 0,3,-5,4,100};
		float p_y[bonusy] = { 2,3,2,4,100 };
		float p_z[bonusy] = { 6,-7.5,-5,-2,100 };

		glPushMatrix();
		Cube2.x = p_x[j];
		Cube2.y = p_y[j];
		Cube2.z = p_z[j];

		if (
			(player.pos.x >= Cube2.x - dokladnosc/2 && player.pos.x <= Cube2.x + dokladnosc/2) &&
			(player.pos.y >= Cube2.y - dokladnosc / 2 && player.pos.y <= Cube2.y + dokladnosc / 2) &&
			(player.pos.z >= Cube2.z - dokladnosc/2 && player.pos.z <= Cube2.z + dokladnosc/2) &&
			j < bonusy)
		{
			ISound *coin = SoundEngine->play2D(coinSrc, false/*playLooped*/, false/*startPaused*/, true/*trackSound*/);

			czas += 120;
			turbo += 1;
			j++;
		}

		glTranslatef(Cube2.x, Cube2.y, Cube2.z);
		glRotatef(3 * frame, 0.0f, 1.0f, 0.0f);
		glutSolidCube(0.15);
		glPopMatrix();

		if (player.speed > 0.03f)
			player.speed -= 0.05;


		if (player.speed < 0.03f)
			player.speed = 0.03;
#pragma endregion

#pragma region Skybox

		glPushMatrix();

		glTranslatef(player.pos.x + player.dir.x, 0.0f, player.pos.z + player.dir.z);
		glScaled(50, 50, 50);
		Draw_Skybox();

		glPopMatrix();

#pragma endregion

		glPushMatrix();
		glTranslatef(player.pos.x, player.pos.y, player.pos.z);		//----------------------------	Wyświetlanie czasu

		char buffer[10] = { '\0' };
		sprintf(buffer, "%d", czas);	// konwersja na string
		if (koniec == 0)
			print(0, 0, GLUT_BITMAP_TIMES_ROMAN_24, buffer);

		glPopMatrix();

		if (player.pos.y < 1.2)
		{
			koniec = 1;
			automatyczne_chodzenie = 0;
			glTranslatef(player.pos.x, player.pos.y, player.pos.z);
			print(0, 0, GLUT_BITMAP_TIMES_ROMAN_24, "Przegrana!Rozbiles sie! :O");
			if (odpalona_koniec==0)
				Odpal_Muzyke(1);
		}

		else if (czas <= 0)
		{
			koniec = 1;
			automatyczne_chodzenie = 0;
			glTranslatef(player.pos.x, player.pos.y, player.pos.z);
			print(0, 0, GLUT_BITMAP_TIMES_ROMAN_24, "Przegrana!Koniec czasu! :(");
			if (odpalona_koniec == 0)
				Odpal_Muzyke(1);
		}

		frame++;
		if (koniec == 0)
			czas--;

	}
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void OnReshape(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(60.0f, (float) width / height, .01f, 100.0f);
}
