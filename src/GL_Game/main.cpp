// oglApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "3rd\glew\include\GL\glew.h"
#include "3rd\glm\glm.hpp"
#include "3rd\GLFW\include\glfw3.h"
#include "Game.h"
#include "Content.h"
#include "Effect.h"
#include "Keyboard.h"
#include <cstdio>
#include <conio.h>

void czyszczenie_wyniku();


int _tmain(int argc, _TCHAR* argv[]) {
	

	int wybor;

	cout << "------------------" << endl;
	cout << "|  Witaj w grze!  |" << endl ;
	cout << "------------------" << endl << endl;
	
	poczatek:
	
	do
	{
		cout << "Zbierz jak najwiecej szesciennych pudel w okreslonym czasie!" << endl;
		cout << "Poruszanie: I, J, K, L " << endl;
		cout << "Spacja - skok" << endl << endl;

		cout << "1. Graj!" << endl;
		cout << "2. Najlepszy wynik" << endl;
		cout << "3. Wyczysc rekord" << endl << endl;
		cout << "ESC. Wyjscie" << endl;

		int best=0;

		FILE *plik = fopen("rekord.txt", "a+");
		
		if (plik)
			fscanf(plik, "%d", &best);

		fclose(plik);

		wybor = getch();

			switch (wybor)
			{
				case 50:
					system("cls");
					cout << "Najlepszy wynik: " << best << endl << endl;
					break;

				case 51:
					system("cls");
					czyszczenie_wyniku();
					cout << "Wyczyszczono rekord." << endl << endl;
					break;

				case 27:
					exit(0);

			default:
				system("cls");
				break;
			}

	} while (wybor != 49);

	cout << "Powodzenia!" << endl;


	// inicjlizacja GLFW
	if (!glfwInit()) {
	    return -1;
	}
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 2);
	GLFWwindow* window;
	window = glfwCreateWindow( 800, 600, "GlGame4", NULL, NULL);
	if ( !window ) {
	    glfwTerminate();
	    return -1;
	}	
	glfwMakeContextCurrent(window);

	GLenum err = glGetError(); 
	if( err ) {
		fprintf( stderr, "ERROR: %s\n", glewGetErrorString( err ) );
		exit(EXIT_FAILURE);
	}
	
	// inicjalizacja GLEW
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult;
	GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult) {
		fprintf( stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	// glowna petla gry
	Game game;
	game.Init();
	Keyboard::KeyboardInit();
	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
	    // rendering
		game.Update();
		game.Redraw();

	    glfwSwapBuffers(window);
	
	    glfwPollEvents();
	}
	
	glfwTerminate();

	goto poczatek;

	return 0;
}


void czyszczenie_wyniku()
{
	int zero = 0;

	FILE *plik = fopen("rekord.txt", "w+");

	if (plik)
		fprintf(plik,"%d", zero);

	fclose(plik);

}

