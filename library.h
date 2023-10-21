#ifndef PGCHESSEXTENTION_LIBRARY_H
#define PGCHESSEXTENTION_LIBRARY_H
#include <stdbool.h>

/*getBoard(chessgame, integer) -> chessboard
 *Return the board state at a given half-move*/
char* getBoard(const char*, int);

/*getFirstMoves(chessgame, integer) -> chessgame
 *Returns the chessgame truncated to its first N half-moves.*/
const char* getFirstMoves(const char*, int);

/*hasOpening(chessgame, chessgame) -> chessgame
 *Returns true if the first chessgame starts with the exact same
 *set of moves as the second chess game.*/
bool hasOpening(const char*, const char*);

/*hasBoard(chessgame, chessboard, integer) -> bool
 *Returns true if the chessgame contains the given board
 *state in its first N half-moves.*/
bool hasBoard(const char*, const char*, int);

void testFunctions();
#endif //PGCHESSEXTENTION_LIBRARY_H
