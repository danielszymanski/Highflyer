// Deklaracje funkcji u¿ywanych jako obs³uga zdarzeñ GLUTa.
void OnRender();
void OnReshape(int, int);
void OnKeyPress(unsigned char, int, int);
void OnKeyDown(unsigned char, int, int);
void OnKeyUp(unsigned char, int, int);
void OnSpecialKeyPress(int, int, int);
void OnSpecialKeyDown(int, int, int);
void OnSpecialKeyUp(int, int, int);
void OnTimer(int);

// Uniwersalna struktura reprezentuj¹ca trójwymiarowy wektor.
// Mo¿e s³u¿yæ do przechowywania po³o¿enia punktu, wspó³rzêdnych wektora itp.
// Wskazane jest rozszerzenie tej struktury o metody i operatory pozwalaj¹ce
// na wykonywanie operacji na wektorach - to na pewno siê przyda w przysz³oœci.
struct vec3 {
	float x, y, z;
};

// Struktura pozwalaj¹ca na przechowanie aktualnego stanu kamery.
struct SCameraState {
	vec3 pos; // pozycja kamery
	vec3 dir; // kierunek patrzenia (wektor jednostkowy)
	float speed; // mno¿nik zmian pozycji - "szybkoœæ" ruchów kamery
	float velM, velS; // zmienne pomocnicze do efektu bezwladnosci kamery - aktualna predkosc
};

SCameraState player;

double T = 0.0; // aktualny czas (a dokladniej - czas z ostatniego wywolania OnTimer())

vec3 LightPos; // pozycja zrodla swiatla

