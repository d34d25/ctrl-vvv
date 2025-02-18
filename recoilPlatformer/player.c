#include "player.h"
#include <math.h>

void initPlayer(struct Player* player, int width, int height, Vector2 pos, Vector2 vel)
{
	player->width = width;
	player->height = height;

	player->position = pos;
	player->velocity = vel;

	player->cooldown = 0.0f;

	player->timesShot = 0;

	player->inAir = false;
	player->gravityInversed = false;
}

void drawPlayer(struct Player* player)
{
	DrawRectangle(player->position.x, player->position.y, player->width, player->height, WHITE);
	//DrawCircle(player->aimPoint.x, player->aimPoint.y, 5, WHITE);
}

void updatePlayer(struct Player *player, float dt)
{
	Vector2 mousePos = GetMousePosition();

	player->aimPoint.x = mousePos.x;
	player->aimPoint.y = mousePos.y;

	player->position.x += player->velocity.x * dt;
	player->position.y += player->velocity.y * dt;

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
void applyForce(struct Player* player)
{
	//printf("Cooldown: %.2f\n", player->cooldown);
	// Apply cooldown check
	if (player->cooldown > 0.0f)
	{
		player->cooldown -= GetFrameTime();
		player->timesShot = 0;
		return;
	}

	Vector2 direction = {
		player->aimPoint.x - player->position.x,
		player->aimPoint.y - player->position.y
	};

	float length = sqrtf(direction.x * direction.x + direction.y * direction.y);

	if (length != 0) {  // Avoid division by zero
		direction.x /= length;
		direction.y /= length;
	}

	float force = 100.0f; //130.0f

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player->timesShot <= 2)
	{
		player->timesShot++;
		printf("mouse pressed \n");
		Vector2 forceVector = { direction.x * -1 * force, direction.y * -1 * force };
		player->velocity.x = forceVector.x * 4;
		player->velocity.y = forceVector.y * 10;

		if (player->timesShot >= 2) player->cooldown = 1.15f; // Set cooldown time in seconds
	}
}

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
		else if (player->position.x > obstacle->x)
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
}




