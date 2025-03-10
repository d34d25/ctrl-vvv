#include <stdio.h>
#include "raylib.h"
#include "testLevelScreen.h"

typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, MENU } GameScreen;

int main() 
{
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    int screenWidth = GetScreenWidth();//192 * 4;
    int screenHeight = GetScreenHeight();//160 * 4; //160 * 4; 

    InitWindow(screenWidth, screenHeight, "");

    ToggleFullscreen();
    //initialize here

    

    GameScreen currentScreen = GAMEPLAY;
    
    switch (currentScreen)
    {
    case GAMEPLAY:
        testGameplayScreenInit(screenWidth,screenHeight);
    default:
        break;
    }

    SetTargetFPS(144);

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
