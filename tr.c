#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
#include <math.h>

#define WIDTH 100
#define HEIGHT 42
#define ON (int)'0'
#define OFF (int)' '
#define ESC "\x1b"

int grid[HEIGHT][WIDTH];

typedef struct {
    int x;
    int y;
} vec2;

void hide_cursor() {
    printf("\e[?25l");
    fflush(stdout);
}
void show_cursor() {
    printf("\e[?25h");
    fflush(stdout);
}
void draw(int grid[HEIGHT][WIDTH]){
    printf(ESC "[H"); 
    for (int h = 0; h<HEIGHT; h++){
        for(int w = 0; w<WIDTH; w++){
            putchar(grid[h][w]);
        }
        putchar('\n');
    }
}

void plot(int x, int y){
    grid[y][x] = ON;
}
void clearGrid(){
    for (int i = 0; i < HEIGHT; i++){
        for (int j = 0; j < WIDTH; j++){
            grid[i][j] = OFF;
        }
    }
}
void bresLine(vec2 a, vec2 b){
    int dx = abs(b.x - a.x);
    int sx = (a.x < b.x) ? 1 : -1;

    int dy = -abs(b.y - a.y);
    int sy = (a.y < b.y) ? 1 : -1;

    int err = dx + dy;

    while (1) {
        if (a.x >= 0 && a.x < WIDTH){
            if (a.y >= 0 && a.y < HEIGHT){
                plot(a.x, a.y);
            }
        }
        if (a.x == b.x && a.y == b.y) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; a.x += sx; }
        if (e2 <= dx) { err += dx; a.y += sy; }
    }
}

void rotatePoint(vec2* p, vec2 o){
    int x = (*p).x;
    int y = (*p).y;
    int x2 = o.x;
    int y2 = o.y;

    double nx, ny;

    nx = (x-x2)*cos(1) - (y-y2)*sin(1)+x2;
    ny = (x-x2)*sin(1) + (y-y2)*cos(1)+x2;

    (*p).x = (int)(nx+0.5);
    (*p).y = (int)(ny+0.5);
}

int main(){
    printf(ESC "[2J");
    hide_cursor();

    vec2 a = {10,5}, b = {21, 6}, c = {12, 16};
    vec2 center = {HEIGHT/2,WIDTH/2};

    int pressed = 1;
    while (1){
        pressed = 1;
        for (int c = 0; c < 256; c++) {
            if (GetAsyncKeyState(c) & 0x8000) {
                pressed = c;
            }
        }
        if (pressed == 'W'){
            rotatePoint(&a, b);
            rotatePoint(&c, b);
        }
        if (pressed == 'A'){
            rotatePoint(&a, c);
            rotatePoint(&b, c);
        }
        if (pressed == 'S'){
            rotatePoint(&b, a);
            rotatePoint(&c, a);
        }
        if (pressed == 'D'){
            rotatePoint(&a, center);
            rotatePoint(&b, center);
            rotatePoint(&c, center);
        }


        clearGrid();
        bresLine(a, b);
        bresLine(b, c);
        bresLine(a, c);
        draw(grid);
        
    }

    return 0;
}
