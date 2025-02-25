#include "textures.h"

void SplitSpritesheet(Texture2D texture, Texture2D* sprites, int rows, int columns, int spriteWidth, int spriteHeight)
{
    Image img = LoadImageFromTexture(texture);

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < columns; ++col)
        {
            // Define the source rectangle for the current sprite
            Rectangle sourceRect = { col * spriteWidth, row * spriteHeight, spriteWidth, spriteHeight };

            // Crop the image to get the sprite
            Image cropped = ImageFromImage(img, sourceRect);


            // Create a texture from the cropped image
            sprites[row * columns + col] = LoadTextureFromImage(cropped);

            // Unload the cropped image after creating the texture
            UnloadImage(cropped);
        }
    }

    // Unload the full image as it's no longer needed
    UnloadImage(img);
}

void Init_Anim(struct Animated* entity_animation, int startFrame, int endFrame, float frameTime)
{
    entity_animation->currentFrame = startFrame;
    entity_animation->frameTime = frameTime;
    entity_animation->elapsedTime = 0.0f;
    entity_animation->startFrame = startFrame;
    entity_animation->endFrame = endFrame;
    entity_animation->numFrames = endFrame - startFrame + 1;
}

void Update_Anim(struct Animated* entity_animation, float deltaTime)
{
    entity_animation->elapsedTime += deltaTime;

    if (entity_animation->elapsedTime >= entity_animation->frameTime)
    {
        entity_animation->elapsedTime -= entity_animation->frameTime;
        entity_animation->currentFrame++;

        if (entity_animation->currentFrame > entity_animation->endFrame)
        {
            entity_animation->currentFrame = entity_animation->startFrame;
        }
    }
}
