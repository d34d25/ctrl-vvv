#include <stdio.h>
#include "raylib.h"
#include "testLevelScreen.h"

typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, MENU } GameScreen;

int main() 
{
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    int screenWidth = 192 * 4;
    int screenHeight = 160 * 4.5f;

    InitWindow(screenWidth, screenHeight, ""); // 192 * 4, 160 * 4.5f   

    ToggleFullscreen();
    //initialize here

    

    GameScreen currentScreen = GAMEPLAY;
    
    switch (currentScreen)
    {
    case GAMEPLAY:
        testGameplayScreenInit();
    default:
        break;
    }

    SetTargetFPS(120);

    while (!WindowShouldClose()) 
    {

        //update here
        switch (currentScreen)
        {
        case GAMEPLAY:
            testGameplayScreenUpdate();
        default:
            break;
        }

        BeginDrawing();
        
        //draw here
        ClearBackground(BLACK);

        //end menu

        switch (currentScreen)
        {
        case GAMEPLAY:

            testGameplayScreenDraw();
        default:
            break;
        }

       

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
