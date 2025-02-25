#pragma once
#include "raylib.h"

typedef enum
{
	IDLE,
	MOVING
}AnimationState;

struct Animated
{
	AnimationState currentState;

	int currentFrame;
	float frameTime;
	float elapsedTime;
	int numFrames;
	int startFrame;
	int endFrame;
};

void SplitSpritesheet(Texture2D texture, Texture2D* sprites, int rows, int columns, int spriteWidth, int spriteHeight);

void Init_Anim(struct Animated* entity_animation, int startFrame, int endFrame, float frameTime);

void Update_Anim(struct Animated* entity_animation, float deltaTime);