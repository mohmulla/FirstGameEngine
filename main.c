#include <stdio.h>
#include <SDL.h>
#include <SDL_surface.h>
#include <crtdbg.h>

#define FPS 60;
#define FRAME_TARGET_TIME 1000/FPS

int game_is_running = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int last_frame_time = 0;

int level = 1;

int keyPressed = 0;
int keyPressed_down = 0;
int keyPressed_up = 0;
int keyPressed_right = 0;
int keyPressed_left = 0;
int keyPressed_s = 0;

int attack = 0;

int face = 0;

struct collisionLine
{
	int x0;
	int y0;
	int x1;
	int y1;
};

struct object
{
	int x;
	int y;
	int width;
	int height;
	 char* sprite[2];
	int subImage;
	int subImageMax;
	int alive;
};

struct enemy
{
	int x;
	int y;
	int width;
	int height;
	char* sprite[30];
	int subImage;
	int subImageMax;
	int hp;
	int xspeed;
	int yspeed;
	int counter;
	int side;
	int hurt;
	int hurtCounter;
};

struct player
{
	int x;
	int y;
	int width;
	int height;
	int xspeed;
	int yspeed;
	const char* sprite[30];
	int subImage;
	int subImageMax;
	struct collisionLine dLine;
	struct collisionLine uLine;
	struct collisionLine rLine;
	struct collisionLine lLine;

};

struct sword
{
	int x;
	int y;
	int width;
	int height;
	char* sprite[30];
	int subImage;
	int subImageMax;
};


int initialize_window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		//fprintf(stderr, "error initializing SDL\n");
		return 0;
	}

	window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_BORDERLESS);

	renderer = SDL_CreateRenderer(window, -1, 0);

	return 1;

}

void setup(void)
{
	
}

void process_input(struct player* thePlayer)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case  SDL_QUIT:
			game_is_running = 0;
			break;

		case SDL_KEYDOWN :
			if (event.key.keysym.sym == SDLK_ESCAPE)
				game_is_running = 0;

			if (event.key.keysym.sym == SDLK_DOWN)
			{
				if (keyPressed_down == 0 && keyPressed_up == 0 && keyPressed_right == 0 && keyPressed_left == 0)
				keyPressed_down = 1;
			}

			if (event.key.keysym.sym == SDLK_UP)
			{
				if (keyPressed_down == 0 && keyPressed_up == 0 && keyPressed_right == 0 && keyPressed_left == 0)
				keyPressed_up = 1;
			}

			if (event.key.keysym.sym == SDLK_RIGHT)
			{
				if (keyPressed_down == 0 && keyPressed_up == 0 && keyPressed_right == 0 && keyPressed_left == 0)
				keyPressed_right = 1;
			}

			if (event.key.keysym.sym == SDLK_LEFT)
			{
				if (keyPressed_down == 0 && keyPressed_up == 0 && keyPressed_right == 0 && keyPressed_left == 0)
				keyPressed_left = 1;
			}

			if (event.key.keysym.sym == SDLK_s)
			{
				if (keyPressed_down == 0 && keyPressed_up == 0 && keyPressed_right == 0 && keyPressed_left == 0 && attack == 0)
				{
					keyPressed_s = 1;
					
				}
			}

			break;

		case SDL_KEYUP :
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				if (keyPressed_down == 1)
					keyPressed_down = 0;
			}

			if (event.key.keysym.sym == SDLK_UP)
			{
				if (keyPressed_up == 1)
					keyPressed_up = 0;
			}

			if (event.key.keysym.sym == SDLK_RIGHT)
			{
				if (keyPressed_right == 1)
					keyPressed_right = 0;
			}

			if (event.key.keysym.sym == SDLK_LEFT)
			{
				if (keyPressed_left == 1)
					keyPressed_left = 0;
			}

			if (event.key.keysym.sym == SDLK_s)
			{
				if (keyPressed_s == 1)
				{
					keyPressed_s = 0;

					
				}
			}

			break;
	}

}

void update(struct object * objects[], struct player* thePlayer, struct sword* theSword, struct enemy* theEnemy)
{
	void enemyAi(struct enemy* theEnemy);
	void swordLeft(struct sword* theSword);
	void swordRight(struct sword* theSword);
	void swordUp(struct sword* theSword);
	void swordDown(struct sword* theSword);
	void spriteAttackLeft(struct player* thePlayer);
	void spriteAttackRight(struct player* thePlayer);
	void spriteAttackUp(struct player* thePlayer);
	void spriteAttackDown(struct player* thePlayer);
	void objectAi(struct object* theObject, struct player* thePlayer);
	void spriteStandLeft(struct player* thePlayer);
	void spriteStandRight(struct player* thePlayer);
	void spriteStandUp(struct player* thePlayer);
	void spriteStandDown(struct player* thePlayer);
	void spriteWalkLeft(struct player* thePlayer);
	void spriteWalkRight(struct player* thePlayer);
	void spriteWalkUp(struct player* thePlayer);
	void spriteWalkDown(struct player* thePlayer);
	int collisionFreeLeft(struct object* objects[], struct player* thePlayer);
	int collisionFreeRight(struct object* objects[], struct player* thePlayer);
	int collisionFreeDown(struct object* objects[], struct player* thePlayer);
	int collisionFreeUp(struct object* objects[], struct player* thePlayer);
	void ballAi(struct ball *ball);
	int initializeLevel(struct object* objects[], struct player* thePlayer);

	static int isLevelInitailized = 0;

	if (!isLevelInitailized)
	{
		isLevelInitailized = initializeLevel(objects, thePlayer, theSword);
	}

	float a = SDL_GetTicks();
	float b = last_frame_time + FRAME_TARGET_TIME;
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), b));

	last_frame_time = SDL_GetTicks();

	enemyAi(theEnemy);

	thePlayer->dLine.x0 = thePlayer->x;
	thePlayer->dLine.y0 = thePlayer->y + thePlayer->height;
	thePlayer->dLine.x1 = thePlayer->x + thePlayer->width;
	thePlayer->dLine.y1 = thePlayer->y + thePlayer->height;
	thePlayer->uLine.x0 = thePlayer->x;
	thePlayer->uLine.y0 = thePlayer->y;
	thePlayer->uLine.x1 = thePlayer->x + thePlayer->width;
	thePlayer->uLine.y1 = thePlayer->y;
	thePlayer->rLine.x0 = thePlayer->x + thePlayer->width;
	thePlayer->rLine.y0 = thePlayer->y;
	thePlayer->rLine.x1 = thePlayer->x + thePlayer->width;
	thePlayer->rLine.y1 = thePlayer->y + thePlayer->height;
	thePlayer->lLine.x0 = thePlayer->x;
	thePlayer->lLine.y0 = thePlayer->y;
	thePlayer->lLine.x1 = thePlayer->x;
	thePlayer->lLine.y1 = thePlayer->y + thePlayer->height;

	

	printf("%i\n",attack);

	if (attack == 0)
	{
		if (keyPressed_down)
		{
			face = 1;
			attack = 0;

			if (collisionFreeDown(objects, thePlayer))
			{
				thePlayer->yspeed = 1;
				thePlayer->xspeed = 0;

				spriteWalkDown(thePlayer);
			}
			else
			{
				thePlayer->yspeed = 0;
				spriteWalkDown(thePlayer);

			}

		}
		else if (keyPressed_up)
		{
			face = -1;


			spriteWalkUp(thePlayer);

			if (collisionFreeUp(objects, thePlayer))
			{
				thePlayer->yspeed = -1;
				thePlayer->xspeed = 0;

				
			}
			else
				thePlayer->yspeed = 0;
		}
		else
		{
			thePlayer->yspeed = 0;

			if (face == 1 && attack == 0)
			{
				spriteStandDown(thePlayer);
			}
			else if (face == -1 && attack == 0)
			{
				spriteStandUp(thePlayer);
			}
		}

		if (keyPressed_right)
		{
			face = 2;

			spriteWalkRight(thePlayer);

			if (collisionFreeRight(objects, thePlayer))
			{
				thePlayer->yspeed = 0;
				thePlayer->xspeed = 1;

				
			}
			else
				thePlayer->xspeed = 0;
		}
		else if (keyPressed_left)
		{
			face = -2;

			spriteWalkLeft(thePlayer);

			if (collisionFreeLeft(objects, thePlayer))
			{
				thePlayer->yspeed = 0;
				thePlayer->xspeed = -1;

				
			}
			else
				thePlayer->xspeed = 0;
		}
		else
		{
			thePlayer->xspeed = 0;

			if (face == 2)
			{
				spriteStandRight(thePlayer);
			}
			else if (face == -2)
			{
				spriteStandLeft(thePlayer);
			}
		}

		if (keyPressed_s)
		{
			if (face == 1)
			{
				if (attack == 0)
				{
					attack = 1;
					spriteAttackDown(thePlayer);
					thePlayer->subImage = 0;

					swordDown(theSword);
					theSword->x = thePlayer->x + 5;
					theSword->y = thePlayer->y + 16;
					theSword->subImage = 0;
				}
			}
			if (face == -1)
			{
				if (attack == 0)
				{
					attack = 1;
					spriteAttackUp(thePlayer);
					thePlayer->subImage = 0;

					swordUp(theSword);
					theSword->x = thePlayer->x + 2;
					theSword->y = thePlayer->y - 10;
					theSword->subImage = 0;
				}
			}
			if (face == 2)
			{
				if (attack == 0)
				{
					attack = 1;
					spriteAttackRight(thePlayer);
					thePlayer->subImage = 0;

					swordRight(theSword);
					theSword->x = thePlayer->x + 16;
					theSword->y = thePlayer->y + 3;
					theSword->subImage = 0;
				}
			}
			if (face == -2)
			{
				if (attack == 0)
				{
					attack = 1;
					spriteAttackLeft(thePlayer);
					thePlayer->subImage = 0;

					swordLeft(theSword);
					theSword->x = thePlayer->x - 8;
					theSword->y = thePlayer->y + 3;
					theSword->subImage = 0;
				}
			}
		}
	}

	if (keyPressed_s == 0)
	{
		if (attack == 30)
		{
			attack = 0;
			
		}
	}

	if (attack > 0 && attack < 30)
	{
		if (attack < 15)
		{
			if ( face == 1 )
			{
				for (int i = 0; i < 2; i++)
				{
					if (theSword->y + 11 >= objects[i]->y)
					{
						if (theSword->x > objects[i]->x && theSword->x < objects[i]->x + 16)
						{
							objects[i]->x = -16;
							objects[i]->y = -16;
						}
					}
				}

				if (theSword->y + 11 >= theEnemy->y)
				{
					if (theSword->x > theEnemy->x && theSword->x < theEnemy + 16)
					{
						theEnemy->hurt = 1;
					}
				}
			}
			if (face == -1)
			{
				for (int i = 0; i < 2; i++)
				{
					if (theSword->y  <= objects[i]->y + 16)
					{
						if (theSword->x > objects[i]->x && theSword->x < objects[i]->x + 16)
						{
							objects[i]->x = -16;
							objects[i]->y = -16;
						}
					}
				}
			}
			if (face == 2)
			{
				for (int i = 0; i < 2; i++)
				{
					if (theSword->x + 5 >= objects[i]->x && theSword->x + 5 < objects[i]->x + 16)
					{
						if (theSword->y < objects[i]->y + 16 && theSword->y > objects[i]->y )
						{
							objects[i]->x = -16;
							objects[i]->y = -16;
						}
					}
				}
			}
			if (face == -2)
			{
				for (int i = 0; i < 2; i++)
				{
					if (theSword->x - 5 < objects[i]->x + 16 && theSword->x - 5 > objects[i]->x)
					{
						if (theSword->y < objects[i]->y + 16 && theSword->y > objects[i]->y)
						{
							objects[i]->x = -16;
							objects[i]->y = -16;
						}
					}
				}
			}
		}
		attack++;
	}
	if (attack == 30)
	{
		theSword->x = -32;
		theSword->y = -32;

		if ( face == 1 )
		{
			spriteStandDown(thePlayer);
		}
		if (face == -1)
		{
			spriteStandUp(thePlayer);
		}
		if (face == 2)
		{
			spriteStandRight(thePlayer);
		}
		if (face == -2)
		{
			spriteStandLeft(thePlayer);
		}
	}
	
	

	thePlayer->y += thePlayer->yspeed;
	thePlayer->x += thePlayer->xspeed;

	for (int i = 0; i < 2; i++)
	{
		objectAi(objects[i], thePlayer);
	}
}

void render(struct object *objects[], struct player* thePlayer, struct sword* theSword, struct enemy* theEnemy)
{
	

	SDL_SetRenderDrawColor(renderer, 255, 128, 128, 128);
	SDL_RenderClear(renderer);

	
	SDL_Rect obj[8];
	SDL_Rect objPlayer;
	SDL_Rect objSword;
	SDL_Rect objEnemy;

	for (int i = 0; i < 8; i++)
	{
		// 1: assign objects to rectangle
		for (int j = 0; j < 4; j++)
		{
			obj[i].x = objects[i]->x;
			obj[i].y = objects[i]->y;
			obj[i].w = objects[i]->width;
			obj[i].h = objects[i]->height;
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		// 2: fill rect
		SDL_RenderFillRect(renderer, &obj[i]);

		// 3: create surface
		SDL_Surface* surface;

		int theSubImage = objects[i]->subImage;

		surface = SDL_LoadBMP(objects[i]->sprite[theSubImage]);

		objects[i]->subImage += 1;

		if (objects[i]->subImage > objects[i]->subImageMax)
		{
			objects[i]->subImage = 0;
		}

		// 4: create texture
		SDL_Texture* texture;
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_FreeSurface(surface);

		// 5: copy texture to renderer
		SDL_RenderCopy(renderer, texture, NULL, &obj[i]);

		SDL_DestroyTexture(texture);
	}

	
	// 1: assign objects to rectangles
	/*
	for (int i = 0; i < 2; ++i)
	{
		obj[i].x = objects[i]->x;
		obj[i].y = objects[i]->y;
		obj[i].w = objects[i]->width;
		obj[i].h = objects[i]->height;
	}
	*/
	objPlayer.x = thePlayer->x;
	objPlayer.y = thePlayer->y;
	objPlayer.w = thePlayer->width;
	objPlayer.h = thePlayer->height;

	objSword.x = theSword->x;
	objSword.y = theSword->y;
	objSword.w = theSword->width;
	objSword.h = theSword->height;

	objEnemy.x = theEnemy->x;
	objEnemy.y = theEnemy->y;
	objEnemy.w = theEnemy->width;
	objEnemy.h = theEnemy->height;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	
	// 2: fill rectangles
	/*
	for (int i = 0; i < 2; ++i)
	{
		SDL_RenderFillRect(renderer, &obj[i]);
	}
	*/
	SDL_RenderFillRect(renderer, &objPlayer);
	SDL_RenderFillRect(renderer, &objSword);
	SDL_RenderFillRect(renderer, &objEnemy);
	
		
	//SDL_Surface* surfaces[2];
	SDL_Surface* playerSurface;
	SDL_Surface* swordSurface;
	SDL_Surface* enemySurface;

	// 3: import surfaces
	/*
	for (int i = 0; i < 2; ++i)
	{
		if (objects[i]->alive == 1)
		{
			int theSubImage = objects[i]->subImage;

			surfaces[i] = SDL_LoadBMP(objects[i]->sprite[theSubImage]);

			objects[i]->subImage += 1;

			if (objects[i]->subImage > objects[i]->subImageMax)
			{
				objects[i]->subImage = 0;
			}
		}
	}
	*/
	int playerSubImage = thePlayer->subImage;
	playerSurface = SDL_LoadBMP(thePlayer->sprite[playerSubImage]);
	thePlayer->subImage += 1;
	if (thePlayer->subImage > thePlayer->subImageMax)
		thePlayer->subImage = 0;

	int swordSubImage = theSword->subImage;
	swordSurface = SDL_LoadBMP(theSword->sprite[swordSubImage]);
	theSword->subImage += 1;
	if (theSword->subImage > theSword->subImageMax)
		theSword->subImage = 0;

	int enemySubImage = theEnemy->subImage;
	enemySurface = SDL_LoadBMP(theEnemy->sprite[swordSubImage]);
	theEnemy->subImage += 1;
	if (theEnemy->subImage > theEnemy->subImageMax)
		theEnemy->subImage = 0;
	

	//SDL_Texture* textures[2];
	SDL_Texture* playerTexture;
	SDL_Texture* swordTexture;
	SDL_Texture* enemyTexture;

	// 4: assign surfaces to textures
	/*
	for (int i = 0; i < 2; ++i)
	{
		textures[i] = SDL_CreateTextureFromSurface(renderer, surfaces[i]);
	}
	*/
	playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
	swordTexture = SDL_CreateTextureFromSurface(renderer, swordSurface);
	enemyTexture = SDL_CreateTextureFromSurface(renderer, enemySurface);
	

	/*
	for (int i = 0; i < 2; ++i)
	{
		SDL_FreeSurface(surfaces[i]);
	}
	*/
	SDL_FreeSurface(playerSurface);
	SDL_FreeSurface(swordSurface);
	SDL_FreeSurface(enemySurface);
	

	
	// 5 : copy texture to renderer
	/*
	for (int i = 0; i < 2; i++)
	{
		SDL_RenderCopy(renderer, textures[i], NULL, &obj[i]);
	}
	*/
	SDL_RenderCopy(renderer, playerTexture, NULL, &objPlayer);
	SDL_RenderCopy(renderer, swordTexture, NULL, &objSword);
	SDL_RenderCopy(renderer, enemyTexture, NULL, &objEnemy);

	/*
	for (int i = 0; i < 2; i++)
	{
		SDL_DestroyTexture(textures[i]);
	}
	*/
	SDL_DestroyTexture(playerTexture);
	SDL_DestroyTexture(swordTexture);
	SDL_DestroyTexture(enemyTexture);
	
	// 6: Present render

	SDL_RenderPresent(renderer);

	
}

void distroy_window(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(void)
{
	game_is_running = initialize_window();

	setup();

	struct player thePlayer;
	struct player* pPlayer = &thePlayer;

	struct sword theSword;
	struct sword* pSword = &theSword;

	struct enemy theEnemy;
	struct enemy* pEnemy = &theEnemy;

	struct object gameObjects[8];
	struct object* pGameObjects[8];

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	for (int i = 0; i < 8; ++i)
	{
		pGameObjects[i] = &gameObjects[i];
	}

	while (game_is_running)
	{
		process_input(pPlayer);
		update(pGameObjects, pPlayer, pSword, pEnemy);
		render(pGameObjects, pPlayer, pSword, pEnemy);
	}

	distroy_window();

	_CrtDumpMemoryLeaks();

	return 0;

}


int initializeLevel(struct object* objects[], struct player* thePlayer, struct sword* theSword, struct enemy * theEnemy)
{

	thePlayer->x = 400;
	thePlayer->y = 400;
	thePlayer->width = 16;
	thePlayer->height = 16;
	thePlayer->xspeed = 0;
	thePlayer->yspeed = 0;
	thePlayer->sprite[0] = "C:/link_walk_1.bmp";
	thePlayer->sprite[1] = "C:/link_walk_1.bmp";
	
	thePlayer->sprite[2] = "C:/link_walk_1.bmp";
	thePlayer->sprite[3] = "C:/link_walk_1.bmp";
	thePlayer->sprite[4] = "C:/link_walk_1.bmp";
	thePlayer->sprite[5] = "C:/link_walk_1.bmp";
	thePlayer->sprite[6] = "C:/link_walk_1.bmp";
	thePlayer->sprite[7] = "C:/link_walk_1.bmp";
	thePlayer->sprite[8] = "C:/link_walk_1.bmp";
	thePlayer->sprite[9] = "C:/link_walk_1.bmp";
	thePlayer->sprite[10] = "C:/link_walk_1.bmp";
	thePlayer->sprite[11] = "C:/link_walk_1.bmp";
	thePlayer->sprite[12] = "C:/link_walk_1.bmp";
	thePlayer->sprite[13] = "C:/link_walk_1.bmp";
	thePlayer->sprite[14] = "C:/link_walk_1.bmp";
	thePlayer->sprite[15] = "C:/link_walk_1.bmp";
	thePlayer->sprite[16] = "C:/link_walk_2.bmp";
	thePlayer->sprite[17] = "C:/link_walk_2.bmp";
	thePlayer->sprite[18] = "C:/link_walk_2.bmp";
	thePlayer->sprite[19] = "C:/link_walk_2.bmp";
	thePlayer->sprite[20] = "C:/link_walk_2.bmp";
	thePlayer->sprite[21] = "C:/link_walk_2.bmp";
	thePlayer->sprite[22] = "C:/link_walk_2.bmp";
	thePlayer->sprite[23] = "C:/link_walk_2.bmp";
	thePlayer->sprite[24] = "C:/link_walk_2.bmp";
	thePlayer->sprite[25] = "C:/link_walk_2.bmp";
	thePlayer->sprite[26] = "C:/link_walk_2.bmp";
	thePlayer->sprite[27] = "C:/link_walk_2.bmp";
	thePlayer->sprite[28] = "C:/link_walk_2.bmp";
	thePlayer->sprite[29] = "C:/link_walk_2.bmp";
	
	
	thePlayer->subImage = 0;
	thePlayer->subImageMax = 29;
	thePlayer->dLine.x0 = thePlayer->x ;
	thePlayer->dLine.y0 = thePlayer->y + thePlayer->height;
	thePlayer->dLine.x1 = thePlayer->x + thePlayer->width;
	thePlayer->dLine.y1 = thePlayer->y + thePlayer->height;
	thePlayer->uLine.x0 = thePlayer->x;
	thePlayer->uLine.y0 = thePlayer->y;
	thePlayer->uLine.x1 = thePlayer->x + thePlayer->width;
	thePlayer->uLine.y1 = thePlayer->y;
	thePlayer->rLine.x0 = thePlayer->x + thePlayer->width;
	thePlayer->rLine.y0 = thePlayer->y;
	thePlayer->rLine.x1 = thePlayer->x + thePlayer->width;
	thePlayer->rLine.y1 = thePlayer->y + thePlayer->height;
	thePlayer->lLine.x0 = thePlayer->x;
	thePlayer->lLine.y0 = thePlayer->y;
	thePlayer->lLine.x1 = thePlayer->x;
	thePlayer->lLine.y1 = thePlayer->y + thePlayer->height;


	objects[0]->x = 128;
	objects[0]->y = 128;
	objects[0]->width = 16;
	objects[0]->height = 16;
	objects[0]->sprite[0] = "C:/smile.bmp";
	objects[0]->sprite[1] = "C:/smile.bmp";
	objects[0]->subImage = 0;
	objects[0]->subImageMax = 1;
	objects[0]->alive = 1;

	objects[1]->x = 256;
	objects[1]->y = 256;
	objects[1]->width = 16;
	objects[1]->height = 16;
	objects[1]->sprite[0] = "C:/smile.bmp";
	objects[1]->sprite[1] = "C:/smile.bmp";
	objects[1]->subImage = 0;
	objects[1]->subImageMax = 1;
	objects[1]->alive = 1;

	objects[2]->x = 0;
	objects[2]->y = 0;
	objects[2]->width = 16;
	objects[2]->height = 16;
	objects[2]->sprite[0] = "C:/world_block.bmp";
	objects[2]->sprite[1] = "C:/world_block.bmp";
	objects[2]->subImage = 0;
	objects[2]->subImageMax = 1;

	objects[3]->x = 16;
	objects[3]->y = 0;
	objects[3]->width = 16;
	objects[3]->height = 16;
	objects[3]->sprite[0] = "C:/world_block.bmp";
	objects[3]->sprite[1] = "C:/world_block.bmp";
	objects[3]->subImage = 0;
	objects[3]->subImageMax = 1;

	objects[4]->x = 32;
	objects[4]->y = 0;
	objects[4]->width = 16;
	objects[4]->height = 16;
	objects[4]->sprite[0] = "C:/world_block.bmp";
	objects[4]->sprite[1] = "C:/world_block.bmp";
	objects[4]->subImage = 0;
	objects[4]->subImageMax = 1;

	objects[5]->x = 48;
	objects[5]->y = 0;
	objects[5]->width = 16;
	objects[5]->height = 16;
	objects[5]->sprite[0] = "C:/world_block.bmp";
	objects[5]->sprite[1] = "C:/world_block.bmp";
	objects[5]->subImage = 0;
	objects[5]->subImageMax = 1;

	objects[6]->x = 64;
	objects[6]->y = 0;
	objects[6]->width = 16;
	objects[6]->height = 16;
	objects[6]->sprite[0] = "C:/world_block.bmp";
	objects[6]->sprite[1] = "C:/world_block.bmp";
	objects[6]->subImage = 0;
	objects[6]->subImageMax = 1;

	objects[7]->x = 80;
	objects[7]->y = 0;
	objects[7]->width = 16;
	objects[7]->height = 16;
	objects[7]->sprite[0] = "C:/world_block.bmp";
	objects[7]->sprite[1] = "C:/world_block.bmp";
	objects[7]->subImage = 0;
	objects[7]->subImageMax = 1;



	theSword->x = -16;
	theSword->y = -16;
	theSword->width = 8;
	theSword->height = 11;
	for (int i = 0; i < 15; i++)
		theSword->sprite[i] = "C:/sword_down_1.bmp";
	for(int i = 15; i < 23; i++)
		theSword->sprite[i] = "C:/sword_down_2.bmp";
	for (int i = 23; i < 30; i++)
		theSword->sprite[i] = "C:/sword_down_3.bmp";
	theSword->subImage = 0;
	theSword->subImageMax = 29;


	theEnemy->x = 512;
	theEnemy->y = 512;
	theEnemy->width = 16;
	theEnemy->height = 16;
	
	for (int i = 0; i < 15; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_down_1.bmp";
	}
	for (int i = 15; i < 30; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_down_2.bmp";
	}
	theEnemy->subImage = 0;
	theEnemy->subImageMax = 29;
	theEnemy->hp = 3;
	theEnemy->xspeed = 1;
	theEnemy->yspeed = 1;
	theEnemy->counter = 0;
	theEnemy->side = 0;

	return 1;
}

int collisionFreeDown(struct object* objects[], struct player* thePlayer)
{
	int thereIsNoCollision = 1;

	for (int i = 0; i < 8; i++)
	{
		if (thePlayer->dLine.y0 == objects[i]->y)
			if ( thePlayer->dLine.x0 < objects[i]->x + objects[i]->width && thePlayer->dLine.x1 > objects[i]->x)
			thereIsNoCollision = 0;
	}

	return thereIsNoCollision;
}

int collisionFreeUp(struct object* objects[], struct player* thePlayer)
{
	int thereIsNoCollision = 1;

	for (int i = 0; i < 8; i++)
	{
		if (thePlayer->uLine.y0 == objects[i]->y + objects[i]->height)
			if (thePlayer->uLine.x0 < objects[i]->x + objects[i]->width && thePlayer->uLine.x1 > objects[i]->x)
				thereIsNoCollision = 0;
	}

	return thereIsNoCollision;
}

int collisionFreeRight(struct object* objects[], struct player* thePlayer)
{
	int thereIsNoCollision = 1;

	for (int i = 0; i < 8; i++)
	{
		if (thePlayer->rLine.x0 == objects[i]->x)
			if (thePlayer->rLine.y0 < objects[i]->y + objects[i]->height && thePlayer->rLine.y1 > objects[i]->y)
				thereIsNoCollision = 0;
	}

	return thereIsNoCollision;
}

int collisionFreeLeft(struct object* objects[], struct player* thePlayer)
{
	int thereIsNoCollision = 1;

	for (int i = 0; i < 8; i++)
	{
		if (thePlayer->lLine.x0 == objects[i]->x + objects[i]->width)
			if (thePlayer->lLine.y0 < objects[i]->y + objects[i]->height && thePlayer->lLine.y1 > objects[i]->y)
				thereIsNoCollision = 0;
	}

	return thereIsNoCollision;
}

void spriteWalkDown(struct player* thePlayer)
{
	thePlayer->sprite[0] = "C:/link_walk_1.bmp";
	thePlayer->sprite[1] = "C:/link_walk_1.bmp";
	thePlayer->sprite[2] = "C:/link_walk_1.bmp";
	thePlayer->sprite[3] = "C:/link_walk_1.bmp";
	thePlayer->sprite[4] = "C:/link_walk_1.bmp";
	thePlayer->sprite[5] = "C:/link_walk_1.bmp";
	thePlayer->sprite[6] = "C:/link_walk_1.bmp";
	thePlayer->sprite[7] = "C:/link_walk_1.bmp";
	thePlayer->sprite[8] = "C:/link_walk_1.bmp";
	thePlayer->sprite[9] = "C:/link_walk_1.bmp";
	thePlayer->sprite[10] = "C:/link_walk_1.bmp";
	thePlayer->sprite[11] = "C:/link_walk_1.bmp";
	thePlayer->sprite[12] = "C:/link_walk_1.bmp";
	thePlayer->sprite[13] = "C:/link_walk_1.bmp";
	thePlayer->sprite[14] = "C:/link_walk_1.bmp";
	thePlayer->sprite[15] = "C:/link_walk_1.bmp";
	thePlayer->sprite[16] = "C:/link_walk_2.bmp";
	thePlayer->sprite[17] = "C:/link_walk_2.bmp";
	thePlayer->sprite[18] = "C:/link_walk_2.bmp";
	thePlayer->sprite[19] = "C:/link_walk_2.bmp";
	thePlayer->sprite[20] = "C:/link_walk_2.bmp";
	thePlayer->sprite[21] = "C:/link_walk_2.bmp";
	thePlayer->sprite[22] = "C:/link_walk_2.bmp";
	thePlayer->sprite[23] = "C:/link_walk_2.bmp";
	thePlayer->sprite[24] = "C:/link_walk_2.bmp";
	thePlayer->sprite[25] = "C:/link_walk_2.bmp";
	thePlayer->sprite[26] = "C:/link_walk_2.bmp";
	thePlayer->sprite[27] = "C:/link_walk_2.bmp";
	thePlayer->sprite[28] = "C:/link_walk_2.bmp";
	thePlayer->sprite[29] = "C:/link_walk_2.bmp";
}

void spriteWalkUp(struct player* thePlayer)
{
	thePlayer->sprite[0] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[1] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[2] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[3] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[4] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[5] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[6] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[7] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[8] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[9] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[10] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[11] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[12] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[13] = "C:/link_walk_up_1.bmp";
	thePlayer->sprite[14] = "C:/link_walk_up_1.bmp";

	thePlayer->sprite[15] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[16] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[17] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[18] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[19] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[20] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[21] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[22] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[23] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[24] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[25] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[26] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[27] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[28] = "C:/link_walk_up_2.bmp";
	thePlayer->sprite[29] = "C:/link_walk_up_2.bmp";
}

void spriteWalkRight(struct player* thePlayer)
{
	thePlayer->sprite[0] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[1] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[2] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[3] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[4] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[5] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[6] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[7] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[8] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[9] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[10] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[11] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[12] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[13] = "C:/link_walk_right_1.bmp";
	thePlayer->sprite[14] = "C:/link_walk_right_1.bmp";

	thePlayer->sprite[15] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[16] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[17] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[18] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[19] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[20] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[21] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[22] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[23] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[24] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[25] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[26] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[27] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[28] = "C:/link_walk_right_2.bmp";
	thePlayer->sprite[29] = "C:/link_walk_right_2.bmp";
}

void spriteWalkLeft(struct player* thePlayer)
{
	thePlayer->sprite[0] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[1] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[2] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[3] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[4] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[5] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[6] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[7] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[8] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[9] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[10] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[11] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[12] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[13] = "C:/link_walk_left_1.bmp";
	thePlayer->sprite[14] = "C:/link_walk_left_1.bmp";

	thePlayer->sprite[15] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[16] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[17] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[18] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[19] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[20] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[21] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[22] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[23] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[24] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[25] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[26] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[27] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[28] = "C:/link_walk_left_2.bmp";
	thePlayer->sprite[29] = "C:/link_walk_left_2.bmp";
}

void spriteStandDown(struct player* thePlayer)
{
	for (int i = 0; i < 30; i++)
	{
		thePlayer->sprite[i] = "C:/link_stand_down_1.bmp";
	}
}

void spriteStandUp(struct player* thePlayer)
{
	for (int i = 0; i < 30; i++)
	{
		thePlayer->sprite[i] = "C:/link_stand_up_1.bmp";
	}
}

void spriteStandRight(struct player* thePlayer)
{
	for (int i = 0; i < 30; i++)
	{
		thePlayer->sprite[i] = "C:/link_stand_right_1.bmp";
	}
}

void spriteStandLeft(struct player* thePlayer)
{
	for (int i = 0; i < 30; i++)
	{
		thePlayer->sprite[i] = "C:/link_stand_left_1.bmp";
	}
}

void objectAi(struct object* theObject, struct player* thePlayer)
{
	// collision with player from up
	if(theObject->y == thePlayer->dLine.y0)
	{
		if (theObject->x < thePlayer->dLine.x1 && theObject->x + theObject->width > thePlayer->dLine.x0)
		{
			//theObject->x = -16;
			//theObject->y = -16;
		}
	}
	else if (theObject->y + theObject->height == thePlayer->uLine.y0) // collision with player from down
	{
		if (theObject->x < thePlayer->dLine.x1 && theObject->x + theObject->width > thePlayer->dLine.x0)
		{
			//theObject->x = -16;
			//theObject->y = -16;
		}
	}
	else if (theObject->x + theObject->width == thePlayer->lLine.x0) // collision with player from right
	{
		if (theObject->y < thePlayer->rLine.y1 && theObject->y + theObject->height > thePlayer->rLine.y0)
		{
			//theObject->x = -16;
			//theObject->y = -16;
		}
	}
	else if (theObject->x  == thePlayer->rLine.x0) // collision with player from left
	{
		if (theObject->y < thePlayer->rLine.y1 && theObject->y + theObject->height > thePlayer->lLine.y0)
		{
			//theObject->x = -16;
			//theObject->y = -16;
		}
	}
}

void spriteAttackDown(struct player* thePlayer)
{
	for(int i = 0; i < 8; i++)
		thePlayer->sprite[i] = "C:/link_atk_down_1.bmp";

	for (int i = 8; i < 16; i++)
		thePlayer->sprite[i] = "C:/link_atk_down_2.bmp";

	for (int i = 16; i < 24; i++)
		thePlayer->sprite[i] = "C:/link_atk_down_3.bmp";

	for (int i = 24; i <= 29; i++)
		thePlayer->sprite[i] = "C:/link_atk_down_4.bmp";
}

void spriteAttackUp(struct player* thePlayer)
{
	for (int i = 0; i < 8; i++)
		thePlayer->sprite[i] = "C:/link_atk_up_1.bmp";

	for (int i = 8; i < 16; i++)
		thePlayer->sprite[i] = "C:/link_atk_up_2.bmp";

	for (int i = 16; i < 24; i++)
		thePlayer->sprite[i] = "C:/link_atk_up_3.bmp";

	for (int i = 24; i <= 29; i++)
		thePlayer->sprite[i] = "C:/link_atk_up_4.bmp";
}

void spriteAttackRight(struct player* thePlayer)
{
	for (int i = 0; i < 8; i++)
		thePlayer->sprite[i] = "C:/link_atk_right_1.bmp";

	for (int i = 8; i < 16; i++)
		thePlayer->sprite[i] = "C:/link_atk_right_2.bmp";

	for (int i = 16; i < 24; i++)
		thePlayer->sprite[i] = "C:/link_atk_right_3.bmp";

	for (int i = 24; i <= 29; i++)
		thePlayer->sprite[i] = "C:/link_atk_right_4.bmp";
}

void spriteAttackLeft(struct player* thePlayer)
{
	for (int i = 0; i < 8; i++)
		thePlayer->sprite[i] = "C:/link_atk_left_1.bmp";

	for (int i = 8; i < 16; i++)
		thePlayer->sprite[i] = "C:/link_atk_left_2.bmp";

	for (int i = 16; i < 24; i++)
		thePlayer->sprite[i] = "C:/link_atk_left_3.bmp";

	for (int i = 24; i <= 29; i++)
		thePlayer->sprite[i] = "C:/link_atk_left_4.bmp";
}

void swordDown(struct sword* theSword)
{
	for (int i = 0; i < 15; i++)
		theSword->sprite[i] = "C:/sword_down_1.bmp";
	for (int i = 15; i < 23; i++)
		theSword->sprite[i] = "C:/sword_down_2.bmp";
	for (int i = 23; i < 30; i++)
		theSword->sprite[i] = "C:/sword_down_3.bmp";
}

void swordUp(struct sword* theSword)
{
	for (int i = 0; i < 15; i++)
		theSword->sprite[i] = "C:/sword_up_1.bmp";
	for (int i = 15; i < 23; i++)
		theSword->sprite[i] = "C:/sword_up_2.bmp";
	for (int i = 23; i < 30; i++)
		theSword->sprite[i] = "C:/sword_up_3.bmp";
}

void swordRight(struct sword* theSword)
{
	for (int i = 0; i < 15; i++)
		theSword->sprite[i] = "C:/sword_right_1.bmp";
	for (int i = 15; i < 23; i++)
		theSword->sprite[i] = "C:/sword_right_2.bmp";
	for (int i = 23; i < 30; i++)
		theSword->sprite[i] = "C:/sword_right_3.bmp";
}

void swordLeft(struct sword* theSword)
{
	for (int i = 0; i < 15; i++)
		theSword->sprite[i] = "C:/sword_left_1.bmp";
	for (int i = 15; i < 23; i++)
		theSword->sprite[i] = "C:/sword_left_2.bmp";
	for (int i = 23; i < 30; i++)
		theSword->sprite[i] = "C:/sword_left_3.bmp";
}

void enemyAi(struct enemy* theEnemy)
{

	void EnemySpriteWalkRight(struct enemy* theEnemy);
	void EnemySpriteWalkDown(struct enemy* theEnemy);
	void EnemySpriteWalkLeft(struct enemy* theEnemy);
	void EnemySpriteWalkUp(struct enemy* theEnemy);

	theEnemy->counter++;

	// right
	if (theEnemy->side == 0)
	{
		theEnemy->xspeed = 1;
		theEnemy->yspeed = 0;
		EnemySpriteWalkRight(theEnemy);

		if (theEnemy->counter == 60)
		{
			theEnemy->counter = 0;
			theEnemy->side = 1;
		}

	}
	// down
	if (theEnemy->side == 1)
	{
		theEnemy->xspeed = 0;
		theEnemy->yspeed = 1;
		EnemySpriteWalkDown(theEnemy);

		if (theEnemy->counter == 60)
		{
			theEnemy->counter = 0;
			theEnemy->side = 2;
		}

	}
	// left
	if (theEnemy->side == 2)
	{
		theEnemy->xspeed = -1;
		theEnemy->yspeed = 0;
		EnemySpriteWalkLeft(theEnemy);

		if (theEnemy->counter == 60)
		{
			theEnemy->counter = 0;
			theEnemy->side = 3;
		}

	}
	// up
	if (theEnemy->side == 3)
	{
		theEnemy->xspeed = 0;
		theEnemy->yspeed = -1;
		EnemySpriteWalkUp(theEnemy);

		if (theEnemy->counter == 60)
		{
			theEnemy->counter = 0;
			theEnemy->side = 0;
		}

	}

	if (theEnemy->hp <= 0)
	{
		theEnemy->x = -64;
		theEnemy->y = -64;
	}

	if (theEnemy->hurt == 1)
	{
		theEnemy->hp -= 1;
		theEnemy->hurtCounter = 0;
		theEnemy->hurt = 2;
	}
	if (theEnemy->hurt == 2)
	{
		theEnemy->hurtCounter++;
	}
	if (theEnemy->hurtCounter == 60)
	{
		theEnemy->hurt = 0;
	}

	theEnemy->x = theEnemy->x + theEnemy->xspeed;
	theEnemy->y = theEnemy->y + theEnemy->yspeed;
}

void EnemySpriteWalkRight(struct enemy* theEnemy)
{
	for (int i = 0; i < 15; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_right_1.bmp";
	}
	for (int i = 15; i < 30; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_right_2.bmp";
	}
}

void EnemySpriteWalkDown(struct enemy* theEnemy)
{
	for (int i = 0; i < 15; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_down_1.bmp";
	}
	for (int i = 15; i < 30; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_down_2.bmp";
	}
}

void EnemySpriteWalkLeft(struct enemy* theEnemy)
{
	for (int i = 0; i < 15; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_left_1.bmp";
	}
	for (int i = 15; i < 30; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_left_2.bmp";
	}
}

void EnemySpriteWalkUp(struct enemy* theEnemy)
{
	for (int i = 0; i < 15; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_up_1.bmp";
	}
	for (int i = 15; i < 30; i++)
	{
		theEnemy->sprite[i] = "C:/enemy_up_2.bmp";
	}
}
