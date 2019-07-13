#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
int width, height;

const int FPS = 60;
const int MaxFPS = 1000 / FPS;
int pressedKeys[SDL_NUM_SCANCODES];
int releasedKeys[SDL_NUM_SCANCODES];
int slowFallTime, fastFallTime, fallTime;
int timeElapsed, quit, level;
int temp[4][4];
long score, totalLinesCleared;

int bgInPlay[21][12];
int bg[21][12] = {
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 1
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 2
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 3
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 4
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 5
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 6
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 7
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 8
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 9
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 10
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 11
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 12
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 13
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 14
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 15
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 16
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 17
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 18
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 19
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 20
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 21
};

int blocks[7][4][4] = {
  { // o
          {0, 0, 0, 0},
          {0, 1, 1, 0},
	  {0, 1, 1, 0},
	  {0, 0, 0, 0}
  },
  { // i
          {0, 0, 1, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0}
  },
  { // l
          {0, 1, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 0, 0},
	  {0, 1, 1, 0}
  },
  { // j
          {0, 0, 1, 0},
	  {0, 0, 1, 0},
	  {0, 0, 1, 0}, 
	  {0, 1, 1, 0}
  },
  { // s
          {0, 0, 0, 0},
	  {0, 0, 1, 1},
	  {0, 1, 1, 0},
	  {0, 0, 0, 0}
  },
  { // z
          {0, 0, 0, 0},
	  {0, 1, 1, 0},
	  {0, 0, 1, 1},
	  {0, 0, 0, 0}
  },
  { // t
          {0, 0, 0, 0},
	  {0, 1, 1, 1},
	  {0, 0, 1, 0},
	  {0, 0, 0, 0}
  }
};

int currentBlock;
struct Block { int x, y, color, shape[4][4]; };
struct Block *blocksInPlay[2];
struct Block *newBlock(int i, int j)
{
  struct Block *p = malloc(sizeof(struct Block));
  p->x = i;
  p->y = j;
  p->color = (rand() % 7) + 1;
  memcpy(p->shape, blocks[p->color - 1], sizeof(int) * 16);
  return p;
}

void fill(int x1, int y1, int x2, int y2)
{
  for (int i = x1; i < x2; i++) {
          for (int j = y1; j < y2; j++) {
	          SDL_RenderDrawPoint(renderer, i, j);
	  }
  }
}

void init()
{
  srand(time(0));
  width = height = 1000;
  fallTime = slowFallTime = 300;
  fastFallTime = 50;
  totalLinesCleared = level = quit = 0;
  memcpy(bgInPlay, bg, sizeof(int) * 21 * 12);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
          printf("SDL Init Error\n");
  } else {
          printf("SDL Init Success\n");
  }
  if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) != 0) {
          printf("SDL Create Window Error\n");
  } else {
          printf("SDL Create Window Success\n");
  }
}

int legalMove(int xpos, int ypos, int shape[4][4])
{
  int bgx, bgy;
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          bgy = j + ypos;
		  bgx = i + xpos + 1;
	          if (shape[j][i] && bgInPlay[bgy][bgx]) {
		          return 0;
		  }
	  }
  }
  return 1;
}

void rotate(int shape[4][4], int direction)
{
  memset(temp, 0, sizeof(int) * 16);
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          if (direction == 0) {
		          temp[j][3 - i] = shape[i][j];
		  } else {
		          temp[i][j] = shape[j][3 - i];
		  }
	  }
  }
}
void freezeBlock()
{
  int bgx, bgy;
  struct Block *b = blocksInPlay[currentBlock];
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          bgy = j + b->y;
		  bgx = i + b->x + 1;
		  if (b->shape[j][i]) {
		          bgInPlay[bgy][bgx] = b->color;
		  }
	  }
  }
  free(b);
  blocksInPlay[currentBlock] = NULL;
}

void gameOver()
{
  printf("Game over\n");
  for (;;) {
	    if (SDL_PollEvent(&event) && 
		event.type == SDL_QUIT) {
	              SDL_Quit();
		      quit = 1;
		      return;
	    }
  }
}

void dropBlock()
{
  int i = -1;
  while (blocksInPlay[++i] != NULL);
  struct Block *b = newBlock(4, -1);
  if (legalMove(b->x, b->y, b->shape)) {
          currentBlock = i;
          blocksInPlay[currentBlock] = b;
  } else {
          free(b);
          gameOver();
  }
}

void reset()
{
  memcpy(bgInPlay, bg, sizeof(int) * 21 * 12);
  fallTime = 300;
  for (int i = 0; i < 2; i++) {
          if (blocksInPlay[i] != NULL) {
                  free(blocksInPlay[i]);
		  blocksInPlay[i] = NULL;
	  }
  }
  dropBlock();
}

struct Block *swap(struct Block *b)
{
  int toSwap = (currentBlock + 1) % 2;
  if (blocksInPlay[toSwap] == NULL) {
          dropBlock();
  } else {
          blocksInPlay[toSwap]->x = b->x;
	  blocksInPlay[toSwap]->y = b->y;
  }
  b->x = -6;
  b->y = 0;
  currentBlock = toSwap;
  return blocksInPlay[currentBlock];
}

void shiftRow(int row)
{
  for (int j = row - 1; j > 0; j--) {
          for (int i = 1; i < 11; i++) {
	          bgInPlay[j + 1][i] = bgInPlay[j][i];
	  }
  }
}

void displayScore()
{
  printf("Current Score: %i\n", score);
}


void checkRows()
{
  int rowComplete, linesCleared = 0;
  for (int j = 19; j > 0; j--) {
            rowComplete = 1;
            for (int i = 1; i < 11; i++) {
	            if (bgInPlay[j][i] == 0) {
		            rowComplete = 0;
		    }
	    }
	    if (rowComplete) {
	            linesCleared++;
	            shiftRow(j++);
	    }
  }
  if (linesCleared) {
          switch (linesCleared) {
	  case 1:
	          score += 40 * (level + 1);
		  break;
	  case 2:
	          score += 100 * (level + 1);
	          break;
	  case 3:
	          score += 300 * (level + 1);
		  break;
	  case 4:
	          score += 1200 * (level + 1);
		  break;
	  }
          displayScore();
          totalLinesCleared += linesCleared;
  }
  
}

void updateLevel()
{
  int temp = level;
  level = totalLinesCleared / 10;
  if (level != temp) {
          printf("level: %i\n", level);
  }
  slowFallTime = 300 - (level * 25);
  fastFallTime = 50 - (level * 5);
  slowFallTime = 50 > slowFallTime ? 50 : slowFallTime;
  fastFallTime = 10 > fastFallTime ? 10 : fastFallTime;
}

void handleLogic()
{
  if (SDL_PollEvent(&event)) {
          switch(event.type) {
	  case SDL_KEYDOWN:
	          pressedKeys[event.key.keysym.scancode] = 1;
		  break;
	  case SDL_KEYUP:
	          releasedKeys[event.key.keysym.scancode] = 1;
		  break;
	  case SDL_QUIT:
	          SDL_Quit();
		  quit = 1;
		  break;
	  }
  }
  
  struct Block *b = blocksInPlay[currentBlock];
  if (pressedKeys[SDL_SCANCODE_A]) {
          if (legalMove(b->x - 1, b->y, b->shape)) {
	          b->x--;
	  }
	  pressedKeys[SDL_SCANCODE_A] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_D]) {
          if (legalMove(b->x + 1, b->y, b->shape)) {
	          b->x++;
	  }
	  pressedKeys[SDL_SCANCODE_D] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_W]) {
          rotate(b->shape, 0);
	  if (legalMove(b->x, b->y, temp)) {
	          memcpy(b->shape, temp, sizeof(int) * 16);
	  }
	  pressedKeys[SDL_SCANCODE_W] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_S]) {
          rotate(b->shape, 1);
	  if (legalMove(b->x, b->y, temp)) {
	          memcpy(b->shape, temp, sizeof(int) * 16);
	  }
	  pressedKeys[SDL_SCANCODE_S] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_F]) {
          b = swap(b);
	  if (!legalMove(b->x, b->y, b->shape)) {
	          swap(b);
	  }
	  pressedKeys[SDL_SCANCODE_F] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_SPACE]) {
	  fallTime = fastFallTime;
	  pressedKeys[SDL_SCANCODE_SPACE] = 0;
  }
  if (releasedKeys[SDL_SCANCODE_SPACE]) {
          fallTime = slowFallTime;
	  releasedKeys[SDL_SCANCODE_SPACE] = 0;
  }
  if (pressedKeys[SDL_SCANCODE_ESCAPE]) {
          reset();
	  pressedKeys[SDL_SCANCODE_ESCAPE] = 0;
  }
  if (timeElapsed > fallTime && legalMove(b->x, b->y + 1, b->shape)) {
          timeElapsed = 0;
          b->y++;
  } else if (timeElapsed > fallTime) {
          freezeBlock(b);
	  checkRows();
	  updateLevel();
	  dropBlock();
  }
}

void renderBG()
{
  // boundary
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  fill(290, 100, 300, height - 100);
  fill(width - 300, 100, width - 290, height - 100); 
  fill(290, height - 100, width - 290, height - 90);

  // grid
  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
  for (int i = 340; i < width - 300; i += 40) {
          fill(i, 100, i + 1, height - 100);
  }
  for (int j = 100; j < height - 100; j += 40) {
          fill(300, j, width - 300, j + 1);
  }

  // hold
  SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
  fill(60, 100, 220, 101);
  fill(60, 260, 220, 261);
  fill(60, 100, 61, 261);
  fill(220, 100, 221, 261);
}

void setColor(int color)
{
  int r, g, b;
  r = g = b = 0;
  switch (color) {
  case 1: // o
          r = g = 255;
          break;
  case 2: // i
          g = b = 255;
	  break;
  case 3: // l
          r = 255;
	  g = 128;
          break;
  case 4: // j
          b = 255;
          break;
  case 5: // s
          g = 255;
	  break;
  case 6: // z
          r = 255;
          break;
  case 7: // t
          r = b = 150;
          break;
  }
  SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

void drawBlock(struct Block *block)
{
  setColor(block->color);
  int x = block->x;
  int y = block->y;
  x = 300 + (x * 40);
  y = 100 + (y * 40);
  int x1, y1, x2, y2;
  for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
	          if (block->shape[j][i]) {
		          x1 = x + (i * 40);
			  y1 = y + (j * 40);
			  x2 = x1 + 40;
			  y2 = y1 + 40;
			  fill(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
		  }
	  }
  }
}

void renderLiveBlocks()
{
  for (int i = 0; i < 2; i++) {
          if (blocksInPlay[i] != NULL) {
	          drawBlock(blocksInPlay[i]);
	  }
  }
}

void renderDeadBlocks()
{
  int x = 300, y = 100;
  int x1, y1, x2, y2;
  for (int i = 1; i < 11; i++) {
          for (int j = 0; j < 20; j++) {
	          if (bgInPlay[j][i]) {
		    setColor(bgInPlay[j][i]);
		    x1 = x + ((i - 1) * 40);
		    y1 = y + (j * 40);
		    x2 = x1 + 40;
		    y2 = y1 + 40;
		    fill(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
		  }
	  }
  }
}

void renderImage()
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  renderBG();
  renderLiveBlocks();
  renderDeadBlocks();
  SDL_RenderPresent(renderer);
}

int main()
{
  init();
  int startTime = SDL_GetTicks(), endTime, elapsed;
  dropBlock();
  for (;;) {
          handleLogic();
          renderImage();

	  endTime = SDL_GetTicks();
	  elapsed = endTime - startTime;
	  elapsed = elapsed < MaxFPS ? elapsed : MaxFPS;
	  startTime = endTime;
	  timeElapsed += elapsed;

	  if (quit) {
	          return 0;
	  }
  }
  return 0;
}
