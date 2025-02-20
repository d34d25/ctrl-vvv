#include "player.h"
#include "platforms.h"
#include <math.h>

void initPlayer(struct Player* player, int width, int height, Vector2 pos, Vector2 vel)
{
	player->width = width;
	player->height = height;

	player->position = pos;
	player->velocity = vel;

	player->inAir = false;
	player->gravityInversed = false;
}

void drawPlayer(struct Player* player)
{
	DrawRectangle(player->position.x, player->position.y, player->width, player->height, WHITE);
}

void updatePlayer(struct Player *player, float dt)
{

	player->position.x += player->velocity.x * dt;
	player->position.y += player->velocity.y * dt;

	player->position.x += player->acceleration.x * dt;
	player->position.y += player->acceleration.y * dt;

	//printf("Player X: %.2f, Player Y: %.2f\n", player->position.x, player->position.y);

}

//gravity
void applyGravity(struct Player* player)
{
	float graivityAcc = 350.0f;
	const float GRAVITY = 400.0f;

	float gravityDirection = player->gravityInversed ? -1.0f : 1.0f;
	
	player->velocity.y += graivityAcc * gravityDirection;

	if (player->velocity.y >= GRAVITY)
	{
		player->velocity.y = GRAVITY;
	}
	else if (player->velocity.y <= -GRAVITY)
	{
		player->velocity.y = -GRAVITY;
	}
	
}


//movement

void move(struct Player* player)
{
	float force = 100.0f;
	float moveSpeed = 3.0f * force;

	if (IsKeyDown(KEY_LEFT))
	{
		player->velocity.x = -moveSpeed;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		player->velocity.x = moveSpeed;
	}
	else
	{
		applyDeacceleration(player);
		player->acceleration.x = 0.0f;
		player->acceleration.y = 0.0f;
	}


	if (IsKeyDown(KEY_Z) && !player->inAir)
	{
		player->gravityInversed = !player->gravityInversed;
	}
}

void applyDeacceleration(struct Player* player)
{
	float deceleration = 3000.0f;
	float minVelocity = 1.75f;  // A small threshold to stop the player completely

	if (player->velocity.x > minVelocity)
	{
		player->velocity.x -= deceleration * GetFrameTime();
		if (player->velocity.x < 0) player->velocity.x = 0;
	}
	else if (player->velocity.x < -minVelocity)
	{
		player->velocity.x += deceleration * GetFrameTime();
		if (player->velocity.x > 0) player->velocity.x = 0;
	}
	else
	{

		player->velocity.x = 0.0f;
	}

}


//collisions
bool checkHorizontalCollisions(struct Player* player, Rectangle* obstacle, float dt)
{
	return (player->position.x + player->velocity.x * dt + player->width > obstacle->x &&
		player->position.x + player->velocity.x * dt < obstacle->x + obstacle->width &&
		player->position.y + player->height > obstacle->y &&
		player->position.y < obstacle->y + obstacle->height);
}

bool checkVerticalCollisions(struct Player* player, Rectangle* obstacle, float dt)
{
	return (player->position.x + player->width > obstacle->x &&
		player->position.x < obstacle->x + obstacle->width &&
		player->position.y + player->velocity.y * dt + player->height > obstacle->y &&
		player->position.y + player->velocity.y * dt < obstacle->y + obstacle->height);
}


void resolveCollisions(struct Player* player, Rectangle* obstacle, float dt)
{
	float offset = 0.5f;

	if (checkHorizontalCollisions(player, obstacle, dt))
	{
		player->velocity.x = 0.0f;

		//printf("colliding x \n");
		if (player->position.x < obstacle->x) //moving rigth
		{
			player->position.x = obstacle->x - player->width - offset;
		}
		else if (player->position.x > obstacle->x) //moving left
		{
			player->position.x = obstacle->x + obstacle->width + offset;
		}

	}

	if (checkVerticalCollisions(player, obstacle, dt))
	{
		//printf("colliding y \n");
		player->inAir = false;
		player->velocity.y = 0.0f;

		if (player->position.y < obstacle->y)  //moving down
		{
			player->position.y = obstacle->y - player->height - offset;
		}
		else if (player->position.y > obstacle->y + obstacle->height) //moving up
		{
			player->position.y = obstacle->y + obstacle->height + offset;
		}

	}
	else if (player->velocity.y != 0)
	{
		player->inAir = true;
	}
} //solid tiles

void resolveCollisionsPlatformsX(struct Player* player, struct Platform* p, float dt)
{

	float offset = 0.75f;
	float checkOffset = 2.0f;

	Rectangle obstacle;

	obstacle.x = p->position.x;
	obstacle.y = p->position.y;

	obstacle.width = p->width;
	obstacle.height = p->height;
 	
	if (checkHorizontalCollisions(player, &obstacle, dt))
	{
		player->velocity.x = p->velocity.x;

		if (player->position.x < obstacle.x) //moving rigth
		{
			player->position.x = obstacle.x - player->width - offset;
		}
		else if (player->position.x > obstacle.x) //moving left
		{
			player->position.x = obstacle.x + obstacle.width + offset;
		}
	}
	


	if (checkVerticalCollisions(player, &obstacle, dt))
	{
		player->inAir = false;
		player->velocity.y = 0.0f;
		player->acceleration.x = p->velocity.x;
        
		if (player->position.y <= obstacle.y + checkOffset)  //moving down
		{
			player->position.y = obstacle.y - player->height - offset;
		}
		else if (player->position.y >= obstacle.y + obstacle.height - checkOffset) //moving up
		{
			player->position.y = obstacle.y + obstacle.height + offset;
		}
	}

}

void resolveCollisionsPlatformsY(struct Player* player, struct Platform* p, float dt)
{

	float offsetV = 0.75f;

	float checkOffset = 2.0f;

	Rectangle obstacle;

	obstacle.x = p->position.x;
	obstacle.y = p->position.y;

	obstacle.width = p->width;
	obstacle.height = p->height;

	if (checkVerticalCollisions(player, &obstacle, dt))
	{
		player->inAir = false;
		
		player->velocity.y = 0.0f;

		if (player->position.y + player->height <= obstacle.y + checkOffset)  //moving down
		{
			player->position.y = obstacle.y - player->height - offsetV;

			if (!player->gravityInversed)
			{
				player->acceleration.y = p->velocity.y;
			}
		}
		else if (player->position.y >= obstacle.y + obstacle.height - checkOffset)  //moving up
		{
			player->position.y = obstacle.y + obstacle.height + offsetV;

			if (player->gravityInversed)
			{
				player->acceleration.y = p->velocity.y;
			}
		}

	}
	else if (player->velocity.y != 0)
	{
		player->inAir = true;
	}

}




/*if (checkHorizontalCollisions(player, &obstacle, dt))
	{
		player->velocity.x = p->velocity.x;

		if (player->position.x < obstacle.x) //moving rigth
		{
			player->position.x = obstacle.x - player->width - offset;
		}
		else if (player->position.x > obstacle.x)
		{
			player->position.x = obstacle.x + obstacle.width + offset;
		}
	}
	else
	{

	}*/