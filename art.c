#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>

#define LENGTH 10
#define HEIGHT 10

#define P_WORD 4
#define P_TURN 1

#define CLEAR_SCREEN printf("\e[1;1H\e[2J")
#define RAND_IN_RANGE_0(min0, max0) rand() % (max0+1) + min0
#define RAND_IN_RANGE(min, max) rand() % max + min
#define ARRAY_SIZE(a) (sizeof &a / sizeof a[0])

// TODO:
// - git repo
// - include words from quote 
// 	- cant turn during word ?
// - restart loop if stuck
// - random arrows throughout
// - allow loops
// - pathfinding ? (dont get stuck)

enum direction{up, down, left, right};
enum direction random_move(int * valid){
  int num = RAND_IN_RANGE_0(0, 3);
  if(valid[num] == 1) return num;

  return random_move(valid); // this is bad but eh
}

void print_grid(wchar_t ** grid){
  for(int row = 0; row < HEIGHT; row++){
    for(int col = 0; col < LENGTH; col++) printf("%lc", grid[row][col]);
    printf("\n");
  }
}

int main(int argc, char ** argv){
  srand(time(NULL));
  setlocale(LC_ALL, "");
  
  // initialize grid
  wchar_t ** grid = malloc(sizeof(void*) * HEIGHT);
  for(int row = 0; row < HEIGHT; row++){
    grid[row] = malloc(sizeof(wchar_t) * LENGTH);
    for(int col = 0; col < LENGTH; col++) grid[row][col] = L' ';
  }

  int row = 1;
  int col = 1;
  char * quote = "hello world!";
  enum direction move = RAND_IN_RANGE_0(0, 3);
  enum direction past = move;

  int count = 0;
  while(1){
    past = move;

    // detect valid moves
    int valid_moves[4] = {0};
    int possible = 0;
    for(int i = row - 1; i <= row + 1; i++){
      if(i < 0 || i >= HEIGHT) continue;   // within height bounds
      for(int j = col - 1; j <= col + 1; j++){
	if(j < 0 || j >= LENGTH) continue; // within length bounds
	if(i == row && j == col) continue; // cannot be same cell
	if(i != row && j != col) continue; // cannot be diagonal

	if(grid[i][j] == ' '){
	  if(i < row) valid_moves[0] = 1;
	  if(i > row) valid_moves[1] = 1;
	  if(j < col) valid_moves[2] = 1;
	  if(j > col) valid_moves[3] = 1;

	  possible++;
	}
      }
    }

    if(possible == 0) break;

    // turn if: by chance OR cannot keep going straight
    if(RAND_IN_RANGE(1, P_TURN) == 1 || valid_moves[move] == 0){ 
     
      // if want to turn and current direction isn't only valid one, exclude current direction
      // so if it wants to turn, it will go in a DIFFERENT direction as long as it can
      if(possible > 1 && valid_moves[move] == 1){
	valid_moves[move] = 0;
	possible--;
      }
  
      move = random_move(valid_moves);
    }

    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
    // 00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33
    // │  │  ╰  ╯  │  │  ╭  ╮  ╮  ╯  ─  ─  ╭  ╰  ─  ─

    wchar_t * curves = L"││╰╯││╭╮╮╯──╭╰──";

    grid[row][col] = curves[4 * move + past];
    if(move == up) 	row--;
    if(move == down) 	row++;
    if(move == left) 	col--;
    if(move == right) 	col++;
    count++;

    CLEAR_SCREEN;
    grid[row][col] = L'x';
    print_grid(grid);
    
    usleep(0.1 * 1000 * 1000);
  }

  //free grid
  for (int i = 0; i < HEIGHT; i++) free(grid[i]);
  free(grid);
}
