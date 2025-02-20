#include "testLevelScreen.h"
#include "player.h"
#include "platforms.h"
#include <math.h>

const int MAX_LEVELS = 3;

struct Player player;

float gridSizeY; 
float gridSizeX; //122% more

Rectangle checkArea;

struct Platform horizontalPlatforms[20];
int platformIndex;


int level;
char fileName[50];
bool levelChangeTriggered;

float gameplay_deltaTime;
float gameplay_accumulatedTime;
float gameplay_fixedDeltaTime;


void loadLevelData(const char* levelPath) //assets/level.png
{

    Image levelImage = LoadImage(levelPath);// Load the 40x40 PNG

    if (levelImage.data == NULL) 
    {
        printf("Failed to load image!\n");
        return;
    }

    printf("level%d", level);

    platformIndex = 0;

    for (int i = 0; i < ROWS; i++) 
    {
        for (int j = 0; j < COLS; j++)
        {
            Color pixel = GetImageColor(levelImage, i, j);

            if (pixel.r == 0 && pixel.g == 0 && pixel.b == 0) 
            {
                testLevel[i][j] = 1;  // Solid tile
            }
            else if (pixel.r == 0 && pixel.g == 255 && pixel.b == 0)
            {
                //goal
                testLevel[i][j] = -2;
            }
            else if (pixel.r == 255 && pixel.g == 255 && pixel.b == 0)
            {
                testLevel[i][j] = -1;

                // Player initialization
                Vector2 startPos;

                startPos.x = i * gridSizeX;
                startPos.y = j * gridSizeY;

                Vector2 startVel = { 0.0f, 0.0f };
                initPlayer(&player, gridSizeX * 1.2, gridSizeY * 1.5, startPos, startVel);
                levelChangeTriggered = false;
            }
            else if (pixel.r == 255 && pixel.g == 0 && pixel.b == 0)
            {
                //spike
                testLevel[i][j] = 2;
            }
            else if (pixel.r == 0 && pixel.g == 0 && pixel.b == 255)
            {
                testLevel[i][j] = 3; //moving platform starting right

                Vector2 startPos;

                startPos.x = i * gridSizeX;
                startPos.y = j * gridSizeY;

                Vector2 startVel;
                startVel.x = 100;
                startVel.y = 0;

                horizontalPlatforms[platformIndex] = initPlatform(gridSizeX * 5, gridSizeY, startPos, startVel);
                if (platformIndex < 19)
                {
                    platformIndex++;
                }


            }
            else if (pixel.r == 0 && pixel.g == 255 && pixel.b == 255)
            {
                testLevel[i][j] = 3.5; //moving platform starting left
            }
            else if (pixel.r == 255 && pixel.g == 0 && pixel.b == 255)
            {
                testLevel[i][j] = 4; //moving platform starting up
            }
            else if (pixel.r == 100 && pixel.g == 0 && pixel.b == 255)
            {
                testLevel[i][j] = 4.5; //moving platform starting down
            }
            else 
            {
                testLevel[i][j] = 0;  // Empty space
            }
        }
    }

    UnloadImage(levelImage); // Free memory after processing

}


void drawLevel(Color tileColor)
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            switch (testLevel[i][j])
            {
            case 1:
                DrawRectangle(i * gridSizeX, j * gridSizeY, gridSizeX + 1, gridSizeY + 1, tileColor);
                break;
            case 2://spike
                DrawRectangle(i * gridSizeX, j * gridSizeY, gridSizeX + 1, gridSizeY + 1, RED);
                break;
            case -2: //goal
                DrawRectangle(i * gridSizeX, j * gridSizeY, gridSizeX + 1, gridSizeY + 1, GREEN);
                break;
            case -1: //spawnpoint
                DrawRectangle(i * gridSizeX, j * gridSizeY, gridSizeX + 1, gridSizeY + 1, ORANGE);
                break;                
            default:
                break;
            }

            if (testLevel[i][j] > 2 || testLevel[i][j]< -2)
            {
                //DrawRectangle(i * gridSizeX, j * gridSizeY, gridSizeX + 1, gridSizeY + 1, GRAY);
            }
        }
    }
}

void testGameplayScreenInit()
{
    level = 0;

    ROWS = 70;
    COLS = 70;

    gameplay_fixedDeltaTime = 1.0f / 60.0f;

    // Get screen dimensions
    float screenWidth = 192 * 4;
    float screenHeight = 160 * 4;

    gridSizeY = screenHeight / ROWS;  // Calculate grid size based on screen height and rows
    gridSizeX = gridSizeY * 1.22f;              // Maintain 1.22 aspect ratio for grid width

    if (gridSizeX * COLS > screenWidth) 
    {
        gridSizeX = screenWidth / COLS;   // Adjust if too wide
        gridSizeY = gridSizeX / 1.22f;     // Maintain aspect ratio
    }

    // Level initialization
    snprintf(fileName, sizeof(fileName), "assets/level%d.png", level);
    loadLevelData(fileName);

}


void testGameplayScreenUpdate()
{
    gameplay_deltaTime = GetFrameTime();
    gameplay_accumulatedTime += gameplay_deltaTime;    

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (testLevel[i][j] != 0 && !(testLevel[i][j] >=3 && testLevel[i][j] <= 4.5))
            {
                Rectangle mapRect = { i * gridSizeX, j * gridSizeY, gridSizeX, gridSizeY };

                for (int i = 0; i < platformIndex; i++)
                {
                    resolveCollisionsPlatformTiles(&horizontalPlatforms[i], &mapRect, gameplay_fixedDeltaTime);
                }
               
            }

        }
    }

    while (gameplay_accumulatedTime >= gameplay_fixedDeltaTime)
    {
        move(&player);

        applyGravity(&player);
        
        // Dynamically adjust checkArea size based on grid size
        checkArea.width = gridSizeX * 6;  // Adjust checkArea to cover multiple grid cells
        checkArea.height = gridSizeY * 6;

        checkArea.x = player.position.x + (player.width / 2) - (checkArea.width / 2);
        checkArea.y = player.position.y + (player.height / 2) - (checkArea.height / 2);

        // Determine the range of tiles the player is near
        int startX = fmax(0, checkArea.x / gridSizeX);
        int startY = fmax(0, checkArea.y / gridSizeY);
        int endX = fmin(ROWS - 1, (checkArea.x + checkArea.width) / gridSizeX);
        int endY = fmin(COLS - 1, (checkArea.y + checkArea.height) / gridSizeY);

        // Only check tiles within the player's bounding box
        for (int i = startX; i <= endX; i++)
        {
            for (int j = startY; j <= endY; j++)
            {

                if (testLevel[i][j] != 0)
                {
                    Rectangle mapRect = { i * gridSizeX, j * gridSizeY, gridSizeX, gridSizeY };

                    switch (testLevel[i][j])
                    {
                    case 1: //solid tile
                        resolveCollisions(&player, &mapRect, gameplay_fixedDeltaTime);

                        break;
                    case -2: // goal
                        if (!levelChangeTriggered && (checkHorizontalCollisions(&player, &mapRect, gameplay_fixedDeltaTime) ||
                            checkVerticalCollisions(&player, &mapRect, gameplay_fixedDeltaTime)))
                        {
                            levelChangeTriggered = true;

                            if (level < MAX_LEVELS)
                            {
                                level++;
                                snprintf(fileName, sizeof(fileName), "assets/level%d.png", level);
                                changeScreen(fileName);
                            }
                        }
                        break;
                    case 2: //spike
                        if (checkHorizontalCollisions(&player, &mapRect, gameplay_fixedDeltaTime) || checkVerticalCollisions(&player, &mapRect, gameplay_fixedDeltaTime))
                        {
                            for (int i = 0; i < ROWS; i++)
                            {
                                for (int j = 0; j < COLS; j++)
                                {
                                    if (testLevel[i][j] == -1)
                                    {
                                        player.position.x = gridSizeX * i;
                                        player.position.y = gridSizeY * j;

                                        player.velocity.x = 0;
                                        player.velocity.y = 0;

                                        player.gravityInversed = false;

                                    }
                                }
                            }
                            
                        }

                        break;
                    default:
                        break;
                    }
                }
            }

        }

        for (int i = 0; i < platformIndex; i++) 
        {
            resolveCollisionsPlatformsX(&player, &horizontalPlatforms[i], gameplay_fixedDeltaTime);
            
        }


        updatePlayer(&player, gameplay_fixedDeltaTime);

        for (int i = 0; i < platformIndex; i++)
        {
            updatePlatform(&horizontalPlatforms[i], gameplay_fixedDeltaTime);
        }
        
       
        //printf("Platform index: %d\n", platformIndex);


        gameplay_accumulatedTime -= gameplay_fixedDeltaTime;
    }
 
}

void testGameplayScreenDraw()
{
    Color currentColor = YELLOW;

   switch (level % 5)  // Cycles every 5 levels
   {
   case 0: currentColor = YELLOW; break;
   case 1: currentColor = SKYBLUE; break;
   case 2: currentColor = PINK; break;
   case 3: currentColor = BLUE; break;
   case 4: currentColor = DARKGREEN; break;
   }

   Color checkAreaColor;
   checkAreaColor.a = 100;
   checkAreaColor.r = 200;
   checkAreaColor.g = 200;
   checkAreaColor.b = 200;

   for (int i = 0; i < platformIndex; i++)
   {
       drawPlatform(&horizontalPlatforms[i], currentColor);;
   }
   

   drawLevel(currentColor);
   //DrawRectangle(checkArea.x, checkArea.y, checkArea.width, checkArea.height, checkAreaColor);
   drawPlayer(&player);
}

//level stuff

void changeScreen(const char* level)
{
    loadLevelData(level);
}