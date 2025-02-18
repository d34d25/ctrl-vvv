#pragma once

#ifndef PLAYER_H  // Include guard to prevent multiple inclusions
#define PLAYER_H

#include <stdio.h>
#include "raylib.h"


struct Player
{
	int width;
	int height;
	Vector2 position;
	Vector2 velocity;
	Vector2 aimPoint;
	float cooldown;
	int timesShot;
	bool gravityInversed;
	bool inAir;
};

void initPlayer(struct Player* player, int width, int height, Vector2 pos, Vector2 vel);

void drawPlayer(struct Player* player);

void updatePlayer(struct Player* player, float dt);

//gravity
void applyGravity(struct Player* player);

//collisions

bool checkHorizontalCollisions(struct Player* player, Rectangle* obstacle, float dt);
bool checkVerticalCollisions(struct Player* player, Rectangle* obstacle, float dt);

void resolveCollisions(struct Player* player, Rectangle* obstacle, float dt);

//movement
void applyForce(struct Player* player);

void move(struct Player* player);

void applyDeacceleration(struct Player* player);

#endif 