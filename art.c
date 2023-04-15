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
#define P_TURN 2

#define CLEAR_SCREEN printf("\e[1;1H\e[2J")
#define RAND_IN_RANGE_0(min0, max0) rand() % (max0+1) + min0
#define RAND_IN_RANGE(min, max) rand() % max + min


enum direction{up, down, left, right};
enum direction random_move(int * valid){
  int num = RAND_IN_RANGE_0(0, 3);
  if(valid[num] == 1) return num;

  return random_move(valid); // this is bad but eh
}

struct QUOTEINFO{
  int word_count;
  int * letter_count;
  char ** words;

  // used in main loop
  int current_letter;
  int current_word;
};

struct QUOTEINFO string_to_quote(char * string){

  int length = strlen(string);
  struct QUOTEINFO quote;

  quote.word_count= 1;
  for(int i = 0; i < length; i++){
    if(string[i] == ' ') quote.word_count++;
  }

  int cur_word = 0;
  quote.letter_count = calloc(quote.word_count, sizeof(int));
  for(int i = 0; i < length; i++){
    if(string[i] == ' ') { cur_word++; continue; }
    quote.letter_count[cur_word]++;
  }

  quote.words = malloc(sizeof(void*) * quote.word_count);
  for(int i = 0; i < quote.word_count; i++) quote.words[i] = malloc(sizeof(char) * quote.letter_count[i]);

  int sum_past_letters = 0;
  for(int i = 0; i < quote.word_count; i++){
     for(int j = 0; j < quote.letter_count[i]; j++){

       // absolute index = sum of last words letter count + j + i (spaces)
       quote.words[i][j] = string[sum_past_letters + j + i];
     }

     sum_past_letters += quote.letter_count[i];
  }

  // 00 01 02 03 04    10 11 12 13	(char ** words)
  // h  e  l  l  o     g  u  y  s	(char * string)
  // 0  0  0  0  0     5  5  5  5	(sum_past_letters)
  // 0  0  0  0  0     1  1  1  1	(int i)
  // 0  1  2  3  4     0  1  2  3	(int j)

  return quote;
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

  char * string = "this is a beautiful quote";
  int word = 1;
  struct QUOTEINFO quote = string_to_quote(string);
  quote.current_word = 0;
  quote.current_letter = 0;

  wchar_t * curves = L"││╰╯││╭╮╮╯──╭╰──";

  enum direction move = RAND_IN_RANGE_0(0, 3);
  enum direction past = move;

  int count = 0;
  while(quote.current_word != quote.word_count){
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

    // turn if: (by chance AND not spelling) OR cannot keep going straight
    if((!word && RAND_IN_RANGE(1, P_TURN) == 1) || valid_moves[move] == 0){ 
     
      // if want to turn and current direction isn't only valid one, exclude current direction
      // so if it wants to turn, it will go in a DIFFERENT direction as long as it can
      if(possible > 1 && valid_moves[move] == 1){
	valid_moves[move] = 0;
	possible--;
      }
  
      move = random_move(valid_moves);
    }

  if(RAND_IN_RANGE(1, P_WORD) == 1){
    word = 1;
  }

    // 00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33
    // │  │  ╰  ╯  │  │  ╭  ╮  ╮  ╯  ─  ─  ╭  ╰  ─  ─

    if(!word) grid[row][col] = curves[4 * move + past];
    else{
      grid[row][col] = quote.words[quote.current_word][quote.current_letter];
      quote.current_letter++;
      // if last letter of current word;
      if(quote.current_letter  == quote.letter_count[quote.current_word]){
	quote.current_word++;
	quote.current_letter = 0;
	word = 0;
      }
    };

    if(move == up) 	row--;
    if(move == down) 	row++;
    if(move == left) 	col--;
    if(move == right) 	col++;
    count++;

    CLEAR_SCREEN;
    print_grid(grid);
    
    usleep(0.1 * 1000 * 1000);
  }

  //free quote
  for(int i = 0; i < quote.word_count; i++) free(quote.words[i]);
  free(quote.words);
  free(quote.letter_count);

  //free grid
  for (int i = 0; i < HEIGHT; i++) free(grid[i]);
  free(grid);
}
