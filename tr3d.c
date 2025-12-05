#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <math.h>

#define ESC "\x1b"
#define MAXPOINTS 5000

typedef struct{
    int x;
    int y;
    int z;
} vec3;

int cols;
int rows;

int foco = 20;
vec3 camera = {0,0,0};
double camyaw = 0;
double campitch = 0;

vec3 pontos[MAXPOINTS];
int qtdPontos = 0;

void transformPontos(char coord, int trans){
    for (int i = 0; i<qtdPontos ; i++){
        if (coord == 'x'){
            pontos[i].x += trans;
        }
        else if (coord == 'y'){
            pontos[i].y += trans;
        }
        else{
            pontos[i].z += trans;
        }
    }
}

void hide_cursor() {
    printf("\e[?25l");
    fflush(stdout);
}

void plotPoint(int x, int y){
    if (x < 0 || y < 0 || x>=cols || y >=rows){
        return;
    }
    printf("\x1b[%d;%dH%c", y, x, 'o');
    fflush(stdout);
}
vec3 get3dPointProj(int x, int y, int z){
    vec3 new;
    int newx, newy;
    if (z != camera.z){
        double dx = (x - camera.x), dy = (y - camera.y), dz = (z - camera.z);

        double cosy = cos(camyaw), siny = sin(camyaw), cosp = cos(campitch), sinp = sin(campitch);
        double rotYawX = dx*cosy + dz*siny;
        double rotYawZ = -dx*siny + dz*cosy;
        double rotPitY =  dy*cosp - rotYawZ*sinp;
        double rotPitZ = dy*sinp + rotYawZ*cosp;

        dx = rotYawX;
        dy = rotPitY;
        dz = rotPitZ;
        if (dz >= 0){
            newx = cols+1, newy = rows+1, new.z = z;
            return new;
        }

        double p = (double)(camera.z - foco)/dz;
        newx = dx*p + camera.x;
        newy = dy*p + camera.y;
    }
    else{
        newx = cols+1, newy = rows+1;
    }
    
    new.x = newx, new.y = newy, new.z = z;
    return new;
}
void bresLine(vec3 a, vec3 b){
    int dx = abs(b.x - a.x);
    int sx = (a.x < b.x) ? 1 : -1;

    int dy = -abs(b.y - a.y);
    int sy = (a.y < b.y) ? 1 : -1;

    int err = dx + dy;

    while (1) {
        plotPoint(a.x, a.y);

        if (a.x == b.x && a.y == b.y) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; a.x += sx; }
        if (e2 <= dx) { err += dx; a.y += sy; }
    }
}

void render3dLine(vec3 a, vec3 b){
    vec3 newa = get3dPointProj(a.x, a.y, a.z);
    vec3 newb = get3dPointProj(b.x, b.y, b.z);

    bresLine(newa, newb);
}
void render3dCube(vec3 a, vec3 b){
    //spam fds
    vec3 A = get3dPointProj(a.x, a.y, a.z);
    vec3 B = get3dPointProj(b.x, b.y, b.z);
    vec3 C = get3dPointProj(a.x, a.y, b.z);
    vec3 D = get3dPointProj(a.x, b.y, b.z);
    vec3 E = get3dPointProj(b.x, a.y, b.z);
    vec3 F = get3dPointProj(b.x, a.y, a.z);
    vec3 G = get3dPointProj(b.x, b.y, a.z);
    vec3 H = get3dPointProj(a.x, b.y, a.z);

    bresLine(A, F);
    bresLine(A, H);
    bresLine(H, G);
    bresLine(F, G);
    bresLine(G, B);
    bresLine(F, E);
    bresLine(A, C);
    bresLine(H, D);
    bresLine(B, D);
    bresLine(B, E);
    bresLine(E, C);
    bresLine(C, D);
}

void render3dPontos(){
    int newx = 0, newy = 0;
    for (int i = 0; i<qtdPontos ; i++){
        vec3 new = get3dPointProj(pontos[i].x, pontos[i].y, pontos[i].z);
        plotPoint(new.x, new.y);
    }
}

void turnPointsIntoCubes(){
    if (qtdPontos < 1 || qtdPontos%2 != 0){
        return;
    }
    for (int i = 0; i < qtdPontos; i+=2){
        render3dCube(pontos[i], pontos[i+1]);
    }
}

void novoCubo(int x, int y, int z){
    if (qtdPontos < MAXPOINTS){
        vec3 p1 = {x, y, z};
        vec3 p2 = {x+10, y+10, z+10};
        pontos[qtdPontos] = p1;
        pontos[qtdPontos+1] = p2;
        qtdPontos+=2;
    }
}

int main(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    
    printf(ESC "[2J");
    hide_cursor();


    for(int i = 0; i < 100; i+=10){
        novoCubo(i,10,-30);
    }
    

    int pressed = 1;
    while(1){
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        camera.x = cols/2;
        camera.y = rows/2;

        plotPoint(camera.x, camera.y);
        pressed = 1;
        for (int c = 0; c < 256; c++) {
            if (GetAsyncKeyState(c) & 0x8000) {
                pressed = c;
            }
        }
        if (pressed == 37)transformPontos('x', -1);
        if (pressed == 38)transformPontos('y', -1);
        if (pressed == 39)transformPontos('x', 1);
        if (pressed == 40)transformPontos('y', 1);
        if (pressed == 'W')transformPontos('z', 1);
        if (pressed == 'S')transformPontos('z', -1);
        if (pressed == 'Y')campitch+= 0.01;
        if (pressed == 'H')campitch-= 0.01;
        if (pressed == 'G')camyaw+= 0.01;
        if (pressed == 'J')camyaw-= 0.01;


        
        turnPointsIntoCubes();
        for(int i = 0; i<rows*2;i++){
            printf("\n");
        }
    
    }

    return 0;
}