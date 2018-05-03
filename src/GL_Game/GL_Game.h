// Deklaracje funkcji u�ywanych jako obs�uga zdarze� GLUTa.
void OnRender();
void OnReshape(int, int);
void OnKeyPress(unsigned char, int, int);
void OnKeyDown(unsigned char, int, int);
void OnKeyUp(unsigned char, int, int);
void OnSpecialKeyPress(int, int, int);
void OnSpecialKeyDown(int, int, int);
void OnSpecialKeyUp(int, int, int);
void OnTimer(int);

// Uniwersalna struktura reprezentuj�ca tr�jwymiarowy wektor.
// Mo�e s�u�y� do przechowywania po�o�enia punktu, wsp�rz�dnych wektora itp.
// Wskazane jest rozszerzenie tej struktury o metody i operatory pozwalaj�ce
// na wykonywanie operacji na wektorach - to na pewno si� przyda w przysz�o�ci.
struct vec3 {
	float x, y, z;
};

// Struktura pozwalaj�ca na przechowanie aktualnego stanu kamery.
struct SCameraState {
	vec3 pos; // pozycja kamery
	vec3 dir; // kierunek patrzenia (wektor jednostkowy)
	float speed; // mno�nik zmian pozycji - "szybko��" ruch�w kamery
	float velM, velS; // zmienne pomocnicze do efektu bezwladnosci kamery - aktualna predkosc
};

SCameraState player;

double T = 0.0; // aktualny czas (a dokladniej - czas z ostatniego wywolania OnTimer())

vec3 LightPos; // pozycja zrodla swiatla

