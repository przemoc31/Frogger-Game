#include<math.h>
#include<stdio.h>
#include<string.h>
#include <time.h>
#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;

extern "C" {
#include"SDL2-2.0.10\include\SDL.h"
#include"SDL2-2.0.10\include\SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define LOG_WIDTH 126
#define LOG_SPEED_1 -1.1
#define LOG_SPEED_2  0.3 
#define LOG_SPEED_3  0.6 
#define LOG_SPEED_4 -1.0 
#define LOG_SPEED_5  0.7 
#define CAR_SPEED_1  1.1 
#define CAR_SPEED_2  -0.8
#define CAR_SPEED_3  0.3 
#define CAR_SPEED_4 -0.6 
#define CAR_SPEED_5  0.9 
#define LANE_HEIGHT 30
#define FROGGER_WIDTH 30
#define AMOUNT_OF_LIFES 5
#define GAME_TIME 50
#define DISTANCE_BEETWEEN_META_HOLES 138
#define META_HOLE_WIDTH 60
#define META_HOLE_Y 1.5 * LANE_HEIGHT
#define META_HOLE1_X SCREEN_WIDTH / 2 - 2 * DISTANCE_BEETWEEN_META_HOLES
#define META_HOLE2_X SCREEN_WIDTH / 2 - DISTANCE_BEETWEEN_META_HOLES
#define META_HOLE3_X SCREEN_WIDTH / 2
#define META_HOLE4_X SCREEN_WIDTH / 2 + DISTANCE_BEETWEEN_META_HOLES
#define META_HOLE5_X SCREEN_WIDTH / 2 + 2 * DISTANCE_BEETWEEN_META_HOLES

// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

void resetFrogPosition(double& frog_X, double& frog_Y)
{
	frog_X = SCREEN_WIDTH / 2;
	frog_Y = SCREEN_HEIGHT - 2.5 * LANE_HEIGHT;
}
void froggerDeath(double& frog_X, double& frog_Y, bool& frogDead, int& lifes, double& worldTime)
{
	resetFrogPosition(frog_X, frog_Y);
	worldTime = GAME_TIME;
	lifes--;
	if (!lifes)
		frogDead = true;
}
int outScreen(int objectPos)
{
	if (objectPos > 1310)
		objectPos = objectPos % 1280 - 60;
	else
		if (objectPos < -30)
			objectPos = objectPos % 1280 + 1200;		

	return objectPos;
}
void frogOut(double &posX, double &posY)
{
	if (posX > SCREEN_WIDTH)
		posX = SCREEN_WIDTH - (0.5 * LANE_HEIGHT);
	else
		if (posX < 0)
			posX = (0.5 * LANE_HEIGHT);
		else
			if (posY > SCREEN_HEIGHT - 2 * LANE_HEIGHT)
				posY = SCREEN_HEIGHT - (2.5 * LANE_HEIGHT);
}
void Crash(double car_X, double car_Y, double &frog_X, double &frog_Y, bool &frogDead, int& lifes, double& worldTime)
{
	if (car_X + (0.5 * LANE_HEIGHT) >= frog_X - (0.5 * FROGGER_WIDTH) && car_X - (0.5 * LANE_HEIGHT) <= frog_X + (0.5 * FROGGER_WIDTH) && car_Y == frog_Y)
	{
		froggerDeath(frog_X, frog_Y, frogDead, lifes, worldTime);
	}
}
void logJump(double log_X, double log_Y, double logSpeed, double &frog_X, double &frog_Y, double& froggerSpeed, bool &frogOnLog)
{
	if (frog_Y >= SCREEN_HEIGHT - (13.5 * LANE_HEIGHT) && frog_Y <= SCREEN_HEIGHT - (9.5 * LANE_HEIGHT))
	{
		if (log_Y == frog_Y && log_X + 0.5 * LOG_WIDTH >= frog_X && log_X - 0.5 * LOG_WIDTH <= frog_X)
		{
			froggerSpeed += logSpeed;
			frogOnLog = true;
		}
		
	}
}
void showBonus(SDL_Surface* screen, char* text, SDL_Surface* charset, double frog_X, double frog_Y, int bonus)
{
	sprintf(text, "+ %d", bonus);
	DrawString(screen, frog_X - strlen(text) * 8 / 2, frog_Y - 25, text, charset);
}
void froggerOnMeta(SDL_Surface* screen, char* text, SDL_Surface* charset, double& frog_X, double& frog_Y, bool& frogDead, bool& hole, double& worldTime, int& score)
{
	int bonus = 50 + (worldTime * 10);
	score += bonus;
	showBonus(screen, text, charset, frog_X, frog_Y, bonus);
	hole = true;
	resetFrogPosition(frog_X, frog_Y);	
	worldTime = GAME_TIME;	
}
int checkFrogOnMeta(SDL_Surface* screen, char* text, SDL_Surface* charset, double& frog_X, double& frog_Y, bool& frogDead, bool& hole1, bool& hole2, bool& hole3, bool& hole4, bool& hole5, int& lifes, double& worldTime, int& score)
{
	if (frog_Y == (META_HOLE_Y))
	{
		if (frog_X <= META_HOLE1_X + (0.5 * META_HOLE_WIDTH) && frog_X >= META_HOLE1_X - (0.5 * META_HOLE_WIDTH) && !hole1)
		{
			froggerOnMeta(screen, text, charset, frog_X, frog_Y, frogDead, hole1, worldTime, score);
			return 1;
		}
		else if (frog_X <= META_HOLE2_X + (0.5 * META_HOLE_WIDTH) && frog_X >= META_HOLE2_X - (0.5 * META_HOLE_WIDTH) && !hole2)
		{
			froggerOnMeta(screen, text, charset, frog_X, frog_Y, frogDead, hole2, worldTime, score);
			return 1;
		}
		else if (frog_X <= META_HOLE3_X + (0.5 * META_HOLE_WIDTH) && frog_X >= META_HOLE3_X - (0.5 * META_HOLE_WIDTH) && !hole3)
		{
			froggerOnMeta(screen, text, charset, frog_X, frog_Y, frogDead, hole3, worldTime, score);
			return 1;
		}
		else if (frog_X <= META_HOLE4_X + (0.5 * META_HOLE_WIDTH) && frog_X >= META_HOLE4_X - (0.5 * META_HOLE_WIDTH) && !hole4)
		{
			froggerOnMeta(screen, text, charset, frog_X, frog_Y, frogDead, hole4, worldTime, score);
			return 1;
		}
		else if (frog_X <= META_HOLE5_X + (0.5 * META_HOLE_WIDTH) && frog_X >= META_HOLE5_X - (0.5 * META_HOLE_WIDTH) && !hole5)
		{
			froggerOnMeta(screen, text, charset, frog_X, frog_Y, frogDead, hole5, worldTime, score);
			return 1;
		}
		else
		{
			froggerDeath(frog_X, frog_Y, frogDead, lifes, worldTime);
			return 0;
		}

	}
	else
		return 0;
}
void resetGame(bool& quitQuery, bool& pause, double& Pos_X, double& Pos_Y, double carPos_X[], double logPos_X[], bool& hole1, bool& hole2, bool& hole3, bool& hole4, bool& hole5, int& lifes, double& worldTime, int& score, int& closestPos_Y, bool& frogDead, bool& win)
{
	quitQuery = false;
	pause = false;
	Pos_X = SCREEN_WIDTH / 2;
	Pos_Y = SCREEN_HEIGHT - 2.5 * LANE_HEIGHT;
	double temp = -200;
	for (int i = 0; i < 5; i++)
	{
		carPos_X[i] = temp;
		logPos_X[i] = temp;
		temp += 200;
	}
	hole1 = false;
	hole2 = false;
	hole3 = false;
	hole4 = false;
	hole5 = false;
	lifes = AMOUNT_OF_LIFES;
	worldTime = GAME_TIME;
	score = 0;
	closestPos_Y = SCREEN_HEIGHT - (2.5 * LANE_HEIGHT);
	frogDead = false;
	win = false;
}
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int t1, t2, quit, frames, rc, lifes = AMOUNT_OF_LIFES, heartPos_X = 15, score = 0, closestPos_Y = SCREEN_HEIGHT - (2.5 * LANE_HEIGHT), menu = 1, flashingText = 0;
	double delta, worldTime, fpsTimer, fps;
	SDL_Event event;
	SDL_Surface *screen, *charset, *frogger, *car, *car2, *log, *turtle, *meta_holes, *gameOver, *victory, *heart, *timeString, *pauseBMP, *scoreString, *quitBMP, *continueScreen;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	// okno konsoli nie jest widoczne, je¿eli chcemy zobaczyæ
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmieniæ na "Console"
	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Frogger");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("../template-project-2/cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);

	frogger = SDL_LoadBMP("../template-project-2/frogger.bmp");
	if(frogger == NULL) {
		printf("SDL_LoadBMP(frogger.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};

	car = SDL_LoadBMP("../template-project-2/auto.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(auto.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	car2 = SDL_LoadBMP("../template-project-2/auto1.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(auto1.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	log = SDL_LoadBMP("../template-project-2/kloda.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(kloda.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	turtle = SDL_LoadBMP("../template-project-2/zolw.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(zolw.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	meta_holes = SDL_LoadBMP("../template-project-2/hole_grass.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(hole_grass.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	gameOver = SDL_LoadBMP("../template-project-2/game_over.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(game_over.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	victory = SDL_LoadBMP("../template-project-2/victory.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(victory.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	heart = SDL_LoadBMP("../template-project-2/heart.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(heart.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	timeString = SDL_LoadBMP("../template-project-2/TIME.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(TIME.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	scoreString = SDL_LoadBMP("../template-project-2/SCORE.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(SCORE.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	pauseBMP = SDL_LoadBMP("../template-project-2/pause.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(pause.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	quitBMP = SDL_LoadBMP("../template-project-2/QUITGAME.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(QUITGAME.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	continueScreen = SDL_LoadBMP("../template-project-2/continueScreen.bmp");
	if (frogger == NULL) {
		printf("SDL_LoadBMP(continueScreen.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int perfectblue = SDL_MapRGB(screen->format, 0x0, 0x0, 0xFF);
	int fioletowy = SDL_MapRGB(screen->format, 0xB2, 0x46, 0xD1);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = GAME_TIME;
	double Pos_X = SCREEN_WIDTH / 2, Pos_Y = SCREEN_HEIGHT - 2.5 * LANE_HEIGHT, froggerSpeed = 0;
	double carPos_X[5] = {-200, 0, 200, 400, 600};
	double carPos_Y[5] = { SCREEN_HEIGHT - (3.5 * LANE_HEIGHT), SCREEN_HEIGHT - (4.5 * LANE_HEIGHT), SCREEN_HEIGHT - (5.5 * LANE_HEIGHT), SCREEN_HEIGHT - (6.5 * LANE_HEIGHT), SCREEN_HEIGHT - (7.5 * LANE_HEIGHT) };
	double logPos_X[5] = { -200, 0, 200, 400, 600 };
	double logPos_Y[5] = { SCREEN_HEIGHT - (9.5 * LANE_HEIGHT), SCREEN_HEIGHT - (10.5 * LANE_HEIGHT), SCREEN_HEIGHT - (11.5 * LANE_HEIGHT), SCREEN_HEIGHT - (12.5 * LANE_HEIGHT), SCREEN_HEIGHT - (13.5 * LANE_HEIGHT) };
	bool frogOnLog = false, frogDead = false, hole1 = false, hole2 = false, hole3 = false, hole4 = false, hole5 = false, pause = false, quitQuery = false, win = false, highscoresTable = false;

	while (!quit) {
		if (menu)
		{
			resetGame(quitQuery, pause, Pos_X, Pos_Y, carPos_X, logPos_X, hole1, hole2, hole3, hole4, hole5, lifes, worldTime, score, closestPos_Y, frogDead, win);

			if (flashingText < 60)
				flashingText++;
			else
				flashingText = 0;

			t2 = SDL_GetTicks();

			delta = (t2 - t1) * 0.001;
			t1 = t2;

			SDL_FillRect(screen, NULL, czarny);

			fpsTimer += delta;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};

			SDL_FillRect(screen, NULL, czarny);
			DrawSurface(screen, meta_holes, SCREEN_WIDTH / 2, LANE_HEIGHT);
			DrawRectangle(screen, 0, 2 * LANE_HEIGHT, SCREEN_WIDTH, 5 * LANE_HEIGHT, niebieski, niebieski);
			DrawRectangle(screen, 0, 7 * LANE_HEIGHT, SCREEN_WIDTH, LANE_HEIGHT, fioletowy, fioletowy);
			DrawRectangle(screen, 0, 8 * LANE_HEIGHT, SCREEN_WIDTH, 5 * LANE_HEIGHT, czarny, czarny);
			DrawRectangle(screen, 0, 13 * LANE_HEIGHT, SCREEN_WIDTH, LANE_HEIGHT, fioletowy, fioletowy);

			if (flashingText < 30 && (!highscoresTable))
			{
				sprintf(text, "Press Spacebar to start a game, Esc to exit or h to see highscores");
				DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, charset);
			}		

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_SPACE) menu = 0;
					else if (event.key.keysym.sym == SDLK_h)
					{
						if (!highscoresTable) highscoresTable = true;
						else if (highscoresTable) highscoresTable = false;			
					}
					else if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
			if (highscoresTable) {

				ifstream wejscie("highscores.txt");
				string names[10];
				int highscores[10];
				int max_l;

				sprintf(text, "%s", "HIGHSCORES");
				DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 2) - 140, text, charset);

				for (int i = 0; i < 10; i++)
				{
					wejscie >> names[i];
					wejscie >> highscores[i];

					sprintf(text, "%d. ", i+1);
					DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2 - 60, (SCREEN_HEIGHT / 2) - 100 + 20 * i, text, charset);

					for (int l = 0; names[i][l] != NULL; l++)
					{
						sprintf(text, "%c", names[i][l]);
						DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2 + 8 * l - 40, (SCREEN_HEIGHT / 2) - 100 + 20 * i, text, charset);
						max_l = l;
					}				
					sprintf(text, " - %d", highscores[i]);
					DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2 + 8 * (max_l+4) - 40, (SCREEN_HEIGHT / 2) - 100 + 20 * i, text, charset);
				}

			}

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
		else
		{

			t2 = SDL_GetTicks();

			// w tym momencie t2-t1 to czas w milisekundach,
			// jaki uplyna³ od ostatniego narysowania ekranu
			// delta to ten sam czas w sekundach
			// here t2-t1 is the time in milliseconds since
			// the last screen was drawn
			// delta is the same time in seconds
			delta = (t2 - t1) * 0.001;
			t1 = t2;

			worldTime -= delta;
			if (worldTime <= 0)
			{
				froggerDeath(Pos_X, Pos_Y, frogDead, lifes, worldTime);
				worldTime = GAME_TIME;
			}

			SDL_FillRect(screen, NULL, czarny);

			fpsTimer += delta;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};

			// tekst informacyjny / info text
			DrawRectangle(screen, 0, 14 * LANE_HEIGHT, SCREEN_WIDTH, 60, perfectblue, perfectblue);
			//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
			sprintf(text, "Pozostaly czas = %.1lf s", worldTime);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - 50, text, charset);
			//	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - 35, text, charset);

			DrawSurface(screen, meta_holes, SCREEN_WIDTH / 2, LANE_HEIGHT);
			DrawRectangle(screen, 0, 2 * LANE_HEIGHT, SCREEN_WIDTH, 5 * LANE_HEIGHT, niebieski, niebieski);
			DrawRectangle(screen, 0, 7 * LANE_HEIGHT, SCREEN_WIDTH, LANE_HEIGHT, fioletowy, fioletowy);
			DrawRectangle(screen, 0, 8 * LANE_HEIGHT, SCREEN_WIDTH, 5 * LANE_HEIGHT, czarny, czarny);
			DrawRectangle(screen, 0, 13 * LANE_HEIGHT, SCREEN_WIDTH, LANE_HEIGHT, fioletowy, fioletowy);

			DrawSurface(screen, timeString, (SCREEN_WIDTH / 2) + 80, SCREEN_HEIGHT - 15);					//rysowanie paska czasu i score
			if (worldTime > 10)
				DrawRectangle(screen, (SCREEN_WIDTH / 2) + 120, SCREEN_HEIGHT - 25, worldTime * 3, 20, czarny, zielony);
			else
				DrawRectangle(screen, (SCREEN_WIDTH / 2) + 120, SCREEN_HEIGHT - 25, worldTime * 3, 20, czarny, czerwony);

			DrawSurface(screen, scoreString, (SCREEN_WIDTH / 2) - 80, SCREEN_HEIGHT - 15);
			sprintf(text, "%.d pkt", score);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT - 20, text, charset);

			DrawSurface(screen, car, outScreen(carPos_X[0]), carPos_Y[0]);					//rysowanie aut
			Crash(outScreen(carPos_X[0]), carPos_Y[0], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[0] + 300), carPos_Y[0]);
			Crash(outScreen(carPos_X[0] + 300), carPos_Y[0], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[0] + 500), carPos_Y[0]);
			Crash(outScreen(carPos_X[0] + 500), carPos_Y[0], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[0] - 250), carPos_Y[0]);
			Crash(outScreen(carPos_X[0] - 250), carPos_Y[0], Pos_X, Pos_Y, frogDead, lifes, worldTime);

			DrawSurface(screen, car2, outScreen(carPos_X[1]), carPos_Y[1]);
			Crash(outScreen(carPos_X[1]), carPos_Y[1], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car2, outScreen(carPos_X[1] + 300), carPos_Y[1]);
			Crash(outScreen(carPos_X[1] + 300), carPos_Y[1], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car2, outScreen(carPos_X[1] + 500), carPos_Y[1]);
			Crash(outScreen(carPos_X[1] + 500), carPos_Y[1], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car2, outScreen(carPos_X[1] - 250), carPos_Y[1]);
			Crash(outScreen(carPos_X[1] - 250), carPos_Y[1], Pos_X, Pos_Y, frogDead, lifes, worldTime);

			DrawSurface(screen, car, outScreen(carPos_X[2]), carPos_Y[2]);
			Crash(outScreen(carPos_X[2]), carPos_Y[2], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[2] + 300), carPos_Y[2]);
			Crash(outScreen(carPos_X[2] + 300), carPos_Y[2], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[2] + 500), carPos_Y[2]);
			Crash(outScreen(carPos_X[2] + 500), carPos_Y[2], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[2] - 250), carPos_Y[2]);
			Crash(outScreen(carPos_X[2] - 250), carPos_Y[2], Pos_X, Pos_Y, frogDead, lifes, worldTime);

			DrawSurface(screen, car2, outScreen(carPos_X[3]), carPos_Y[3]);
			Crash(outScreen(carPos_X[3]), carPos_Y[3], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car2, outScreen(carPos_X[3] + 300), carPos_Y[3]);
			Crash(outScreen(carPos_X[3] + 300), carPos_Y[3], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car2, outScreen(carPos_X[3] + 500), carPos_Y[3]);
			Crash(outScreen(carPos_X[3] + 500), carPos_Y[3], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car2, outScreen(carPos_X[3] - 250), carPos_Y[3]);
			Crash(outScreen(carPos_X[3] - 250), carPos_Y[3], Pos_X, Pos_Y, frogDead, lifes, worldTime);

			DrawSurface(screen, car, outScreen(carPos_X[4]), carPos_Y[4]);
			Crash(outScreen(carPos_X[4]), carPos_Y[4], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[4] + 300), carPos_Y[4]);
			Crash(outScreen(carPos_X[4] + 300), carPos_Y[4], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[4] + 500), carPos_Y[4]);
			Crash(outScreen(carPos_X[4] + 500), carPos_Y[4], Pos_X, Pos_Y, frogDead, lifes, worldTime);
			DrawSurface(screen, car, outScreen(carPos_X[4] - 250), carPos_Y[4]);
			Crash(outScreen(carPos_X[4] - 250), carPos_Y[4], Pos_X, Pos_Y, frogDead, lifes, worldTime);

			DrawSurface(screen, turtle, outScreen(logPos_X[0]), logPos_Y[0]);					//rysowanie k³ód i ¿ó³wi
			logJump(outScreen(logPos_X[0]), logPos_Y[0], LOG_SPEED_1, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, turtle, outScreen(logPos_X[0] + 300), logPos_Y[0]);
			logJump(outScreen(logPos_X[0] + 300), logPos_Y[0], LOG_SPEED_1, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, turtle, outScreen(logPos_X[0] + 500), logPos_Y[0]);
			logJump(outScreen(logPos_X[0] + 500), logPos_Y[0], LOG_SPEED_1, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, turtle, outScreen(logPos_X[0] - 250), logPos_Y[0]);
			logJump(outScreen(logPos_X[0] - 250), logPos_Y[0], LOG_SPEED_1, Pos_X, Pos_Y, froggerSpeed, frogOnLog);

			DrawSurface(screen, log, outScreen(logPos_X[1]), logPos_Y[1]);
			logJump(outScreen(logPos_X[1]), logPos_Y[1], LOG_SPEED_2, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[1] + 300), logPos_Y[1]);
			logJump(outScreen(logPos_X[1] + 300), logPos_Y[1], LOG_SPEED_2, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[1] + 500), logPos_Y[1]);
			logJump(outScreen(logPos_X[1] + 500), logPos_Y[1], LOG_SPEED_2, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[1] - 250), logPos_Y[1]);
			logJump(outScreen(logPos_X[1] - 250), logPos_Y[1], LOG_SPEED_2, Pos_X, Pos_Y, froggerSpeed, frogOnLog);


			DrawSurface(screen, log, outScreen(logPos_X[2]), logPos_Y[2]);
			logJump(outScreen(logPos_X[2]), logPos_Y[2], LOG_SPEED_3, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[2] + 300), logPos_Y[2]);
			logJump(outScreen(logPos_X[2] + 300), logPos_Y[2], LOG_SPEED_3, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[2] + 500), logPos_Y[2]);
			logJump(outScreen(logPos_X[2] + 500), logPos_Y[2], LOG_SPEED_3, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[2] - 250), logPos_Y[2]);
			logJump(outScreen(logPos_X[2] - 250), logPos_Y[2], LOG_SPEED_3, Pos_X, Pos_Y, froggerSpeed, frogOnLog);

			DrawSurface(screen, turtle, outScreen(logPos_X[3]), logPos_Y[3]);
			logJump(outScreen(logPos_X[3]), logPos_Y[3], LOG_SPEED_4, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, turtle, outScreen(logPos_X[3] + 300), logPos_Y[3]);
			logJump(outScreen(logPos_X[3] + 300), logPos_Y[3], LOG_SPEED_4, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, turtle, outScreen(logPos_X[3] + 500), logPos_Y[3]);
			logJump(outScreen(logPos_X[3] + 500), logPos_Y[3], LOG_SPEED_4, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, turtle, outScreen(logPos_X[3] - 250), logPos_Y[3]);
			logJump(outScreen(logPos_X[3] - 250), logPos_Y[3], LOG_SPEED_4, Pos_X, Pos_Y, froggerSpeed, frogOnLog);

			DrawSurface(screen, log, outScreen(logPos_X[4]), logPos_Y[4]);
			logJump(outScreen(logPos_X[4]), logPos_Y[4], LOG_SPEED_5, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[4] + 300), logPos_Y[4]);
			logJump(outScreen(logPos_X[4] + 300), logPos_Y[4], LOG_SPEED_5, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[4] + 500), logPos_Y[4]);
			logJump(outScreen(logPos_X[4] + 500), logPos_Y[4], LOG_SPEED_5, Pos_X, Pos_Y, froggerSpeed, frogOnLog);
			DrawSurface(screen, log, outScreen(logPos_X[4] - 250), logPos_Y[4]);
			logJump(outScreen(logPos_X[4] - 250), logPos_Y[4], LOG_SPEED_5, Pos_X, Pos_Y, froggerSpeed, frogOnLog);

			if (hole1)
				DrawSurface(screen, frogger, (SCREEN_WIDTH / 2) - (2 * DISTANCE_BEETWEEN_META_HOLES), (1.5 * LANE_HEIGHT));
			if (hole2)
				DrawSurface(screen, frogger, (SCREEN_WIDTH / 2) - DISTANCE_BEETWEEN_META_HOLES, (1.5 * LANE_HEIGHT));
			if (hole3)
				DrawSurface(screen, frogger, (SCREEN_WIDTH / 2), (1.5 * LANE_HEIGHT));
			if (hole4)
				DrawSurface(screen, frogger, (SCREEN_WIDTH / 2) + DISTANCE_BEETWEEN_META_HOLES, (1.5 * LANE_HEIGHT));
			if (hole5)
				DrawSurface(screen, frogger, (SCREEN_WIDTH / 2) + (2 * DISTANCE_BEETWEEN_META_HOLES), (1.5 * LANE_HEIGHT));

			DrawSurface(screen, frogger, Pos_X, Pos_Y);


			if (pause)					//pauza na P
			{
				DrawSurface(screen, pauseBMP, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				carPos_X[0] -= CAR_SPEED_1;
				carPos_X[1] -= CAR_SPEED_2;
				carPos_X[2] -= CAR_SPEED_3;
				carPos_X[3] -= CAR_SPEED_4;
				carPos_X[4] -= CAR_SPEED_5;

				logPos_X[0] -= LOG_SPEED_1;
				logPos_X[1] -= LOG_SPEED_2;
				logPos_X[2] -= LOG_SPEED_3;
				logPos_X[3] -= LOG_SPEED_4;
				logPos_X[4] -= LOG_SPEED_5;
				froggerSpeed = 0;


				worldTime += delta;

			}
			if (quitQuery)
			{
				DrawSurface(screen, quitBMP, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				if (!pause)
				{
					carPos_X[0] -= CAR_SPEED_1;
					carPos_X[1] -= CAR_SPEED_2;
					carPos_X[2] -= CAR_SPEED_3;
					carPos_X[3] -= CAR_SPEED_4;
					carPos_X[4] -= CAR_SPEED_5;

					logPos_X[0] -= LOG_SPEED_1;
					logPos_X[1] -= LOG_SPEED_2;
					logPos_X[2] -= LOG_SPEED_3;
					logPos_X[3] -= LOG_SPEED_4;
					logPos_X[4] -= LOG_SPEED_5;

					worldTime += delta;
				}
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
					case SDL_KEYDOWN:
						if (event.key.keysym.sym == SDLK_y)
						{
							menu = 1;
						}
						else if (event.key.keysym.sym == SDLK_n)
						{
							quitQuery = false;
						}
						break;
					case SDL_QUIT:
						quit = 1;
						break;
					};
				};

			}

			if (Pos_Y >= SCREEN_HEIGHT - (13.5 * LANE_HEIGHT) && Pos_Y <= SCREEN_HEIGHT - (9.5 * LANE_HEIGHT) && (!frogOnLog)) //¿aba wpada do wody
			{
				froggerDeath(Pos_X, Pos_Y, frogDead, lifes, worldTime);
			}

			carPos_X[0] += CAR_SPEED_1;
			carPos_X[1] += CAR_SPEED_2;
			carPos_X[2] += CAR_SPEED_3;
			carPos_X[3] += CAR_SPEED_4;
			carPos_X[4] += CAR_SPEED_5;

			logPos_X[0] += LOG_SPEED_1;
			logPos_X[1] += LOG_SPEED_2;
			logPos_X[2] += LOG_SPEED_3;
			logPos_X[3] += LOG_SPEED_4;
			logPos_X[4] += LOG_SPEED_5;
			Pos_X += froggerSpeed;


			if (checkFrogOnMeta(screen, text, charset, Pos_X, Pos_Y, frogDead, hole1, hole2, hole3, hole4, hole5, lifes, worldTime, score))			//frogger przechodzi za rzeke
			{
				closestPos_Y = SCREEN_HEIGHT - (2.5 * LANE_HEIGHT);
				SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
				SDL_RenderCopy(renderer, scrtex, NULL, NULL);
				SDL_RenderPresent(renderer);
				Sleep(1000);

				if (hole1 && hole2 && hole3 && hole4 && hole5)					//VICTORY GAME
				{

					DrawSurface(screen, victory, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
					SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
					SDL_RenderCopy(renderer, scrtex, NULL, NULL);
					SDL_RenderPresent(renderer);
					Sleep(3000);
					win = true;
				}
			}

			for (int i = 0; i < lifes; i++)					//rysowanie ¿yæ
			{
				DrawSurface(screen, heart, heartPos_X, SCREEN_HEIGHT - 15);
				heartPos_X += 38;
			}

			if (frogDead)					//Game Over
			{
				DrawSurface(screen, gameOver, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
				SDL_RenderCopy(renderer, scrtex, NULL, NULL);
				SDL_RenderPresent(renderer);
				Sleep(3000);
			}

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			//		SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q)
					{
						quitQuery = true;
					}
					if (!pause && !quitQuery)
					{
						if (event.key.keysym.sym == SDLK_UP) Pos_Y -= LANE_HEIGHT;
						else if (event.key.keysym.sym == SDLK_DOWN) Pos_Y += LANE_HEIGHT;
						else if (event.key.keysym.sym == SDLK_RIGHT) Pos_X += LANE_HEIGHT;
						else if (event.key.keysym.sym == SDLK_LEFT) Pos_X -= LANE_HEIGHT;
					}
					if (event.key.keysym.sym == SDLK_p)
					{
						if (pause == false)
							pause = true;
						else
							pause = false;
					}
					frogOut(Pos_X, Pos_Y);
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};

			if (Pos_Y < closestPos_Y)
			{
				closestPos_Y = Pos_Y;
				score += 10;
			}


			frogOnLog = false;
			froggerSpeed = 0;

			heartPos_X = 15;
			frames++;

			if (win || frogDead)
			{
				SDL_FillRect(screen, NULL, czarny);
				sprintf(text, "Enter your nickname, press Spacebar to confirm");
				DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, charset);
				SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
				SDL_RenderCopy(renderer, scrtex, NULL, NULL);
				SDL_RenderPresent(renderer);
				string name = "";
				bool stop = false;
				while (!stop)
				{

					while (SDL_PollEvent(&event)) {
						switch (event.type) {
						case SDL_KEYDOWN:
							if (event.key.keysym.sym != SDLK_SPACE)
								name += char(event.key.keysym.sym);
							else if (event.key.keysym.sym == SDLK_SPACE) {
								stop = true;  break;
							}
						case SDL_QUIT: {
							quit = 1;
							break;
						}

						};
					};

				}

				ifstream wejscie("highscores.txt");
				string names[10];
				int highscores[10];

				for (int i = 0; i < 10; i++)
				{
					names[i] = "";
					highscores[i] = 0;
					wejscie >> names[i];
					wejscie >> highscores[i];
				}
				wejscie.close();
				for (int i = 0; i < 10; i++)
				{
					if (score >= highscores[i])
					{
						for (int j = 9; j > i; j--)
						{
							names[j] = names[j - 1];
							highscores[j] = highscores[j - 1];
						}
						names[i] = name;
						highscores[i] = score;
						sprintf(text, "Well Done! You got on highscores list, check this in our highscores.txt file :)");
						DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 2) + 20, text, charset);
						sprintf(text, "BYE!!!");
						DrawString(screen, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, (SCREEN_HEIGHT / 2) + 40, text, charset);
						SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
						SDL_RenderCopy(renderer, scrtex, NULL, NULL);
						SDL_RenderPresent(renderer);
						Sleep(3000);
						break;
					}

				}
				ofstream wyjscie("highscores.txt");
				for (int i = 0; i < 10; i++)
				{
					wyjscie << names[i] << " " << highscores[i] << endl;

				}
				wyjscie.close();

				DrawSurface(screen, continueScreen, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
				SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
				SDL_RenderCopy(renderer, scrtex, NULL, NULL);
				SDL_RenderPresent(renderer);
				stop = false;
				while (!stop)
				{
					while (SDL_PollEvent(&event)) {
						switch (event.type) {
						case SDL_KEYDOWN:
							if (event.key.keysym.sym == SDLK_y) { menu = 1; quit = 0;  stop = true; break; }
							else if (event.key.keysym.sym == SDLK_n) { quit = 1; stop = true;  break; }
						case SDL_QUIT: {
							quit = 1;
							break;
							}

						};
					};

				}

			}
		}

		};

		// zwolnienie powierzchni / freeing all surfaces
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		SDL_Quit();
		return 0;
	};
