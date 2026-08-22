#include "raylib.h"

int main () {
    int screenHeight = 450;
    int screenWidth = 800;
    InitWindow(screenWidth, screenHeight, "My Window");
    SetTargetFPS(60);
    int r, g, b, r_rc, g_rc, b_rc, a;
    int x_rc, y_rc, h_rc, w_rc;

    r = 255;
    g = 255;
    b = 255;

    r_rc = 255;
    g_rc = 255;
    b_rc = 0;

    a = 255;


    x_rc = 200;
    y_rc = 200;
    w_rc = 100;
    h_rc = 100;

    while(!WindowShouldClose()) { 
        BeginDrawing();
        ClearBackground(CLITERAL(Color){ r, g, b, a });
        DrawRectangle(x_rc, y_rc, w_rc, h_rc, CLITERAL(Color){ r_rc, g_rc, b_rc, a });
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// w || !gcc main.c -o main -lraylib && ./main
