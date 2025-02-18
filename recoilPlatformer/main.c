#include <stdio.h>
#include "raylib.h"
#include "testLevelScreen.h"

typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, MENU } GameScreen;

int main() 
{
    InitWindow(192 * 4, 160 * 4.5f, ""); // 192 * 4, 160 * 4.5f
    //initialize here

    GameScreen currentScreen = GAMEPLAY;
    
    switch (currentScreen)
    {
    case GAMEPLAY:
        testGameplayScreenInit();
    default:
        break;
    }

    SetTargetFPS(60);

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
        
        //menu
        DrawRectangle(0, 620, 192 * 4, 100, GRAY);

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
