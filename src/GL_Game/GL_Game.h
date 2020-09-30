void OnRender();
void OnReshape(int, int);
void OnKeyPress(unsigned char, int, int);
void OnKeyDown(unsigned char, int, int);
void OnKeyUp(unsigned char, int, int);
void OnSpecialKeyPress(int, int, int);
void OnSpecialKeyDown(int, int, int);
void OnSpecialKeyUp(int, int, int);
void OnTimer(int);

struct vec3 {
	float x, y, z;
};

struct SCameraState {
	vec3 pos;
	vec3 dir;
	float speed;
	float velM, velS;
};

SCameraState player;

double T = 0.0; 
vec3 LightPos;
