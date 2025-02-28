#include "player.h"
#include "platforms.h"
#include <math.h>

const int MAX_SPEED = 400;
const float GRAVITY = 400.0f;

float gravMultiplier = 1.0f;

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

}


/*void updatePlayer(struct Player* player, float dt)
{
	// Calculate the combined velocity + acceleration vector
	Vector2 totalVelocity = { player->velocity.x + player->acceleration.x,
							 player->velocity.y + player->acceleration.y };

	// Calculate the speed (magnitude of the combined vector)
	float speed = sqrtf(totalVelocity.x * totalVelocity.x + totalVelocity.y * totalVelocity.y);

	// If the speed exceeds the maximum allowed speed, normalize and scale the total velocity
	if (speed > MAX_SPEED)
	{
		float scale = MAX_SPEED / speed;
		totalVelocity.x *= scale;
		totalVelocity.y *= scale;
	}

	// Update the player position using the scaled velocity
	player->position.x += totalVelocity.x * dt;
	player->position.y += totalVelocity.y * dt;

	// Update the velocity based on the acceleration (after applying the speed cap)
	player->velocity.x += player->acceleration.x * dt;
	player->velocity.y += player->acceleration.y * dt;
}*/

//gravity
void applyGravity(struct Player* player)
{
	float gravityAcc = 350.0f; //350

	float gravityDirection = player->gravityInversed ? -1.0f : 1.0f;
	
	player->velocity.y += (gravityAcc * gravMultiplier) * gravityDirection;

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
	float moveSpeed = 200.0f;

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
	}


	if (IsKeyDown(KEY_Z) && !player->inAir)
	{
		player->gravityInversed = !player->gravityInversed;
	}

	player->acceleration.x = 0.0f;
	player->acceleration.y = 0.0f;

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

bool checkCollisionsMovingSpikes(struct Player* player, struct Platform* p, float dt)
{
	Rectangle obstacle;

	obstacle.x = p->position.x;
	obstacle.y = p->position.y;

	obstacle.width = p->width;
	obstacle.height = p->height;

	if (checkHorizontalCollisions(player, &obstacle, dt) || checkVerticalCollisions(player, &obstacle, dt))
	{
		return true;
	}

	return false;

}

void resolveCollisions(struct Player* player, Rectangle* obstacle, float dt)
{
	float offsetX = 1.0f; //1.0f //0.0f makes the flickering to stop
	float offsetY = 1.0f;

	if (checkHorizontalCollisions(player, obstacle, dt))
	{
		player->acceleration.x = 0.0f;
		player->velocity.x = 0.0f;

		//printf("colliding x \n");

		if ((player->position.x + player->width * 1.5f < obstacle->x)|| (player->position.x > obstacle->x + obstacle->width * 1.5f))
		{
			player->acceleration.x = 0.0f;
		}

		if (player->position.x < obstacle->x) //moving rigth
		{
			player->position.x = obstacle->x - player->width - offsetX;
		}
		else if (player->position.x > obstacle->x) //moving left
		{
			player->position.x = obstacle->x + obstacle->width + offsetX;
		}

	}

	if (checkVerticalCollisions(player, obstacle, dt))
	{
		//printf("colliding y \n");
		player->inAir = false;

		player->acceleration.x = 0.0f;
		player->acceleration.y = 0.0f;
		player->velocity.y = 0.0f;
		
		if (player->position.y + player->height < obstacle->y)  //moving down
		{
			player->position.y = obstacle->y - player->height - offsetY;
		}
		else if (player->position.y > obstacle->y + obstacle->height) //moving up
		{
			player->position.y = obstacle->y + obstacle->height + offsetY;
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

void resolveCollisionsBounceY(struct Player* player, Rectangle* obstacle, float dt)
{
	
	if (checkVerticalCollisions(player, obstacle, dt))
	{

		if (player->position.y < obstacle->y)  //moving down
		{
			player->gravityInversed = true;
		}
		else if (player->position.y > obstacle->y + obstacle->height) //moving up
		{
			player->gravityInversed = false;
		}

	}

}

void resolveCollisionsGravInversor(struct Player* player,struct Platform* p, float dt)
{
	Rectangle obstacle;

	obstacle.x = p->position.x;
	obstacle.y = p->position.y;

	obstacle.width = p->width;
	obstacle.height = p->height;

	if (checkVerticalCollisions(player, &obstacle, dt))
	{
		player->velocity.y = 0.0f;
		player->gravityInversed = !player->gravityInversed;
	}
}

void resolvePlayerOutOfBounds(struct Player* player, float worldWidth, float worldHeight, float gridX, float gridY)
{
	// Check left boundary Appear on the right
	if (player->position.x + player->width <= 0)
	{
		player->position.x = worldWidth - gridX; // Align to the last grid column
		player->position.y = roundf(player->position.y / gridY) * gridY; // Snap to grid
	}
	// Check right boundary  Appear on the left
	else if (player->position.x >= worldWidth )
	{
		player->position.x = 0; // Start at the left edge
		player->position.y = roundf(player->position.y / gridY) * gridY; // Snap to grid
	}

	// Check top boundary  Appear at the bottom
	if (player->position.y + player->height <= 0)
	{
		player->position.y = worldHeight - gridY;
	}
	// Check bottom boundary  Appear at the top
	else if (player->position.y >= worldHeight)
	{
		player->position.y = -player->height;
	}
}

void resolveSpeedChangingTilesR(struct Player* player, Rectangle* obstacle, float dt)
{
	float offset = 0.5f;
	float speed = 75.0f;

	if (checkHorizontalCollisions(player, obstacle, dt))
	{
		player->velocity.x = 0.0f;
		player->acceleration.x = 0.0f;

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

		player->inAir = false;
		player->velocity.y = 0.0f;
		player->acceleration.x = speed;
		

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
}

void resolveSpeedChangingTilesL(struct Player* player, Rectangle* obstacle, float dt)
{
	float offset = 0.5f;
	float speed = -75.0f;


	if (checkHorizontalCollisions(player, obstacle, dt))
	{
		player->velocity.x = 0.0f;
		player->acceleration.x = 0.0f;

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
		player->inAir = false;
		player->velocity.y = 0.0f;
		player->acceleration.y = 0.0f;
		player->acceleration.x = speed;
		


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
}



/*
solid tiles Y

if (checkVerticalCollisions(player, obstacle, dt))
	{
		//printf("colliding y \n");
		player->inAir = false;

		player->acceleration.y = 0.0f;
		player->velocity.y = 0.0f;

		if (player->position.y + player->height < obstacle->y)  //moving down
		{
			player->position.y = obstacle->y - player->height - offsetY;
		}
		else if (player->position.y > obstacle->y + obstacle->height) //moving up
		{
			player->position.y = obstacle->y + obstacle->height + offsetY;
		}

	}
	else if (player->velocity.y != 0)
	{
		player->inAir = true;
	}

*/