#include <raylib.h>

#include <stdlib.h>

#include "snake_graphics/head_left.inc"
#include "snake_graphics/head_right.inc"
#include "snake_graphics/head_up.inc"
#include "snake_graphics/head_down.inc"
#include "snake_graphics/tail_left.inc"
#include "snake_graphics/tail_right.inc"
#include "snake_graphics/tail_up.inc"
#include "snake_graphics/tail_down.inc"
#include "snake_graphics/apple.inc"
#include "snake_graphics/body_horizontal.inc"
#include "snake_graphics/body_vertical.inc"
#include "snake_graphics/body_topleft.inc"
#include "snake_graphics/body_topright.inc"
#include "snake_graphics/body_bottomleft.inc"
#include "snake_graphics/body_bottomright.inc"

typedef enum Direction {
	LEFT,
	RIGHT,
	UP,
	DOWN,
} Direction;

typedef enum TexID {
	HeadLeft,
	HeadRight,
	HeadUp,
	HeadDown,
	TailLeft,
	TailRight,
	TailUp,
	TailDown,
	Apple,
	BodyHorizontal,
	BodyVertical,
	BodyTopLeft,
	BodyTopRight,
	BodyBottomLeft,
	BodyBottomRight,
	TexIDn
} TexID;

typedef struct Snakeblock {
	int x;
	int y;
	TexID tex;
} Snakeblock;

typedef Snakeblock Fruit;

typedef struct Snake {
	int size;
	Direction dir;
	Snakeblock *body;
	Snakeblock taillastpos;
} Snake;

void processInput(Snake *snake, bool *allowmove, int *framecount) {
	if (!(*allowmove)) return;
	if (IsKeyPressed(KEY_UP) && (snake->dir == LEFT || snake->dir == RIGHT)) {
		snake->dir = UP;
		*framecount = 0;
	} else if (IsKeyPressed(KEY_DOWN) && (snake->dir == LEFT || snake->dir == RIGHT)) {
		snake->dir = DOWN;
		*framecount = 0;
	} else if (IsKeyPressed(KEY_LEFT) && (snake->dir == UP || snake->dir == DOWN)) {
		snake->dir = LEFT;
		*framecount = 0;
	} else if (IsKeyPressed(KEY_RIGHT) && (snake->dir == UP || snake->dir == DOWN)) {
		snake->dir = RIGHT;
		*framecount = 0;
	}
	*allowmove = true;
}

bool progressSnake(Snake *snake)
{
	int x = snake->body[0].x;
	int y = snake->body[0].y;
	switch (snake->dir)
	{
	case UP:
	{
		y -= 50;
	} break;
	case DOWN:
	{
		y += 50;
	} break;
	case RIGHT:
	{
		x += 50;
	} break;
	case LEFT:
	{
		x -= 50;
	} break;
	}
	if (x > 750 || x < 0 || y > 750 || y < 0) return false;
	
	snake->taillastpos = snake->body[snake->size - 1];
	for (int i = snake->size - 1; i > 0; --i) {
		if (snake->body[i - 1].x == x && snake->body[i - 1].y == y)
			return false;
		snake->body[i] = snake->body[i - 1];
	}
	snake->body[0].x = x;
	snake->body[0].y = y;
	
	return true;
}

void growSnake(Snake *snake) {
	snake->body[snake->size] = snake->taillastpos;
	++snake->size;
}

void generateFruit(Fruit *fruit, Snake *snake) {
	int x, y;
	bool inbody = true;
	while (inbody) {
		x = 50 * GetRandomValue(0, 15);
		y = 50 * GetRandomValue(0, 15);
		inbody = false;
		for (int i = 0; i < snake->size; ++i) {
			if (snake->body[i].x == x && snake->body[i].y == y) {
				inbody = true;
				break;
			}
		}
	}
	fruit->x = x;
	fruit->y = y;
}

int main(void) {
	InitWindow(800, 800, "Snake");
	SetTargetFPS(60);
	
	bool changed = ChangeDirectory(GetApplicationDirectory());
	if (!changed) return 1;
	Image *imgs = malloc(TexIDn * sizeof(Image));
	imgs[HeadLeft] = LoadImageFromMemory(".png", head_left.data, head_left.size);
	imgs[HeadRight] = LoadImageFromMemory(".png", head_right.data, head_right.size);
	imgs[HeadUp] = LoadImageFromMemory(".png", head_up.data, head_up.size);
	imgs[HeadDown] = LoadImageFromMemory(".png", head_down.data, head_down.size);
	imgs[TailLeft] = LoadImageFromMemory(".png", tail_left.data, tail_left.size);
	imgs[TailRight] = LoadImageFromMemory(".png", tail_right.data, tail_right.size);
	imgs[TailUp] = LoadImageFromMemory(".png", tail_up.data, tail_up.size);
	imgs[TailDown] = LoadImageFromMemory(".png", tail_down.data, tail_down.size);
	imgs[Apple] = LoadImageFromMemory(".png", apple.data, apple.size);
	imgs[BodyHorizontal] = LoadImageFromMemory(".png", body_horizontal.data, body_horizontal.size);
	imgs[BodyVertical] = LoadImageFromMemory(".png", body_vertical.data, body_vertical.size);
	imgs[BodyTopLeft] = LoadImageFromMemory(".png", body_topleft.data, body_topleft.size);
	imgs[BodyTopRight] = LoadImageFromMemory(".png", body_topright.data, body_topright.size);
	imgs[BodyBottomLeft] = LoadImageFromMemory(".png", body_bottomleft.data, body_bottomleft.size);
	imgs[BodyBottomRight] = LoadImageFromMemory(".png", body_bottomright.data, body_bottomright.size);
	
	Texture2D *textures = malloc(TexIDn * sizeof(Texture2D));
	for (int i = 0; i < TexIDn; ++i) {
		ImageResize(imgs+i, 50, 50);
		textures[i] = LoadTextureFromImage(imgs[i]);
	}
	
	for (int i = 0; i < TexIDn; ++i) {
		UnloadImage(imgs[i]);
	}
	
	Snake snake;
	snake.size = 2;
	snake.dir = RIGHT;
	snake.body = malloc(256 * sizeof(Snakeblock));
	snake.body[0].x = 0;
	snake.body[0].y = 0;
	snake.body[1].x = 0;
	snake.body[1].y = 50;
	Fruit fruit;
	generateFruit(&fruit, &snake);
	
	bool allowmove = false;
	int framecount = 0;
	
	while (!WindowShouldClose()) {
		processInput(&snake, &allowmove, &framecount);
		
		if (framecount % 6 == 0) {
			if (!progressSnake(&snake)) break;
			allowmove = true;
		}
		if (snake.body[0].x == fruit.x && snake.body[0].y == fruit.y) {
			growSnake(&snake);
			generateFruit(&fruit, &snake);
		}
		++framecount;
		
		BeginDrawing();
			int last = snake.size - 1;
			ClearBackground(GREEN);
			DrawTexture(textures[snake.dir], snake.body[0].x, snake.body[0].y, WHITE);
			if (snake.body[last].x == snake.body[last - 1].x) {
				if (snake.body[last].y > snake.body[last - 1].y) {
					DrawTexture(textures[TailDown], snake.body[last].x, snake.body[last].y, WHITE);
				} else {
					DrawTexture(textures[TailUp], snake.body[last].x, snake.body[last].y, WHITE);
				}
			} else {
				if (snake.body[last].x > snake.body[last - 1].x) {
					DrawTexture(textures[TailRight], snake.body[last].x, snake.body[last].y, WHITE);
				} else {
					DrawTexture(textures[TailLeft], snake.body[last].x, snake.body[last].y, WHITE);
				}
			}
			DrawTexture(textures[Apple], fruit.x, fruit.y, WHITE);
			for (int i = snake.size - 2; i > 0; --i) {
				if (snake.body[i].x == snake.body[i-1].x && snake.body[i].x == snake.body[i+1].x)
					DrawTexture(textures[BodyVertical], snake.body[i].x, snake.body[i].y, WHITE);
				else if (snake.body[i].y == snake.body[i-1].y && snake.body[i].y == snake.body[i+1].y)
					DrawTexture(textures[BodyHorizontal], snake.body[i].x, snake.body[i].y, WHITE);
				else {
					int diffnx = snake.body[i].x - snake.body[i-1].x;
					int diffpx = snake.body[i].x - snake.body[i+1].x;
					int diffny = snake.body[i].y - snake.body[i-1].y;
					int diffpy = snake.body[i].y - snake.body[i+1].y;
					if (diffpx == -50 && diffny == -50 || diffnx == -50 && diffpy == -50)
						DrawTexture(textures[BodyTopLeft], snake.body[i].x, snake.body[i].y, WHITE);
					else if (diffpx == 50 && diffny == -50 || diffnx == 50 && diffpy == -50)
						DrawTexture(textures[BodyTopRight], snake.body[i].x, snake.body[i].y, WHITE);
					else if (diffpx == 50 && diffny == 50 || diffnx == 50 && diffpy == 50)
						DrawTexture(textures[BodyBottomRight], snake.body[i].x, snake.body[i].y, WHITE);
					else if (diffpx == -50 && diffny == 50 || diffnx == -50 && diffpy == 50)
						DrawTexture(textures[BodyBottomLeft], snake.body[i].x, snake.body[i].y, WHITE);
				}
			}
		EndDrawing();
	}
	
	return 0;
}