#ifndef PLATFORMS_H
#define PLATFORMS_H

#include "raylib.h"  // Include raylib here

// Define the Platform struct
struct Platform 
{
    Vector2 position;
    Vector2 velocity;

    int width;
    int height;
};

// Function prototype for initPlatform
struct Platform initPlatform(int width, int height, Vector2 pos, Vector2 vel);
void updatePlatform(struct Platform* p, float dt);

void drawPlatform(struct Platform* p, Color c);

bool checkHorizontalCollisionsPlatform(struct Platform* platform, Rectangle* obstacle, float dt);
bool checkVerticalCollisionsPlatform(struct Platform* platform, Rectangle* obstacle, float dt);

void resolveCollisionsPlatformTiles(struct Platform* platform, Rectangle* obstacle, float dt);


#endif // PLATFORMS_H
