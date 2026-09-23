#include "interpreter/interpreter.h"
#include "raylib.h"
#include "interpreter/interpreter.c"

int main () {
    int w = 500;
    int h = 500;

    Interpreter* inter = new_interpreter_from_file("./script.txt");

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(w, h, "My Window");
    SetTargetFPS(60);
    int r, g, b, r_rc, g_rc, b_rc, a;
    int x, y, h_rc, w_rc;
    int i; // Iterator variable

    while(get_var(inter, "i") == -1){
        step_interpreter(inter);
    }
    r = get_var(inter, "r");
    g = get_var(inter, "g");
    b = get_var(inter, "b");

    // Rectangle color (static)
    r_rc = 255;
    g_rc = 255;
    b_rc = 0;
    a = 255;

    w_rc = get_var(inter, "size");
    h_rc = get_var(inter, "size");


    x = get_var(inter, "x");
    y = get_var(inter, "y");




    while(!WindowShouldClose()) { 
        if(IsWindowResized()) {
            set_var(inter, "w", GetScreenWidth());
            set_var(inter, "h", GetScreenHeight());
        }
        BeginDrawing();
        ClearBackground(CLITERAL(Color){ r, g, b, a });
        DrawRectangle(x, y, w_rc, h_rc, CLITERAL(Color){ r_rc, g_rc, b_rc, a });
        EndDrawing();
        while(i == get_var(inter, "i")){
            step_interpreter(inter);
        }

        i = get_var(inter, "i");
        r = get_var(inter, "r");
        g = get_var(inter, "g");
        b = get_var(inter, "b");

        x = get_var(inter, "x");
        y = get_var(inter, "y");

        r_rc = 255 - r;
        g_rc = 255 - g;
        b_rc = 255 - b;

    }

    CloseWindow();

    return 0;
}

// w || !gcc main.c -o main -lraylib && ./main
