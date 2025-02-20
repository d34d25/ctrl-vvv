#include "platforms.h"  // Include this only at the top of the file, not inside any function!

struct Platform initPlatform(int width, int height, Vector2 pos, Vector2 vel) 
{
    struct Platform p;

    p.position.x = pos.x;
    p.position.y = pos.y;

    p.width = width;
    p.height = height;
  
    p.velocity.x = vel.x;
    p.velocity.y = vel.y;

    return p;
}

void updatePlatform(struct Platform *p, float dt)
{
    p->position.x += p->velocity.x * dt;
    p->position.y += p->velocity.y * dt;
}

bool checkHorizontalCollisionsPlatform(struct Platform* platform, Rectangle* obstacle, float dt)
{
    return (platform->position.x + platform->velocity.x * dt + platform->width > obstacle->x &&
        platform->position.x + platform->velocity.x * dt < obstacle->x + obstacle->width &&
        platform->position.y + platform->height > obstacle->y &&
        platform->position.y < obstacle->y + obstacle->height);
}

bool checkVerticalCollisionsPlatform(struct Platform* platform, Rectangle* obstacle, float dt)
{
    return (platform->position.x + platform->width > obstacle->x &&
        platform->position.x < obstacle->x + obstacle->width &&
        platform->position.y + platform->velocity.y * dt + platform->height > obstacle->y &&
        platform->position.y + platform->velocity.y * dt < obstacle->y + obstacle->height);
}

void resolveCollisionsPlatformTiles(struct Platform* platform, Rectangle* obstacle, float dt)
{

	if (checkHorizontalCollisionsPlatform(platform, obstacle, dt))
	{
        if (platform->position.x + platform->width < obstacle->x) // Moving right
        {
            platform->position.x = obstacle->x - platform->width;
        }
        else if (platform->position.x > obstacle->x) // Moving left
        {

            platform->position.x = obstacle->x + obstacle->width;
        }


		platform->velocity.x *= -1.0f;
	}

    if (checkVerticalCollisionsPlatform(platform, obstacle, dt))
    {
        if (platform->position.y < obstacle->y) // Moving down
        {
            platform->position.y = obstacle->y - platform->height;
        }
        else if (platform->position.y > obstacle->y + obstacle->height) // Moving up
        {
            platform->position.y = obstacle->y + obstacle->height;
        }

        platform->velocity.y *= -1.0f;
    }

}

void drawPlatform(struct Platform *p, Color c)
{
    DrawRectangle(p->position.x, p->position.y, p->width, p->height, c);
}
