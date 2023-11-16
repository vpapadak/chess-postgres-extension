#include <postgres.h>
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "fmgr.h"
#include "smallchesslib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

PG_MODULE_MAGIC;

#define DatumGetFenP(X) ((Fen *) DatumGetPointer(X))
#define FenPGetDatum(X) PointerGetDatum(X)
#define PG_GETARG_FEN_P(n) DatumGetFenP(PG_GETARG_DATUM(n))
#define PG_RETURN_FEN_P(x) return FenPGetDatum(x)

typedef struct {
    char board[8][9];//max size<90
    char turn[2];
    char rock[5];
    char en_passant[3];
    char half_move[3];
    char full_move[3];
} Fen;

bool text_eq(text *text_one, text *text_two);
Fen *fen_constructor(char *fen_char);
char* internal_get_first_moves(const char* PGN, int N);
char* internal_get_board(const char* PGN, int N);
bool internal_has_opening(const char* PGNone, const char* PGNtwo);
bool internal_has_board(const char* PGN, const char* FEN, int N);
char ** str_matrix_allocation(int rows, int columns);
void free_str_matrix(char **matrix, int rows);

PG_FUNCTION_INFO_V1(san_input);
Datum san_input(PG_FUNCTION_ARGS) {
    char *input_str = PG_GETARG_CSTRING(0);
    text *input_text = cstring_to_text(input_str); // Convert char * to text
    PG_RETURN_TEXT_P(input_text); // Return the text data type
}

PG_FUNCTION_INFO_V1(san_output);
Datum san_output(PG_FUNCTION_ARGS) {
    text *output_text = PG_GETARG_TEXT_P(0); // Get input text as a text pointer
    char *output = text_to_cstring(output_text); // Convert text to C string
    PG_RETURN_CSTRING(output);
}

PG_FUNCTION_INFO_V1(san_eq);
Datum san_eq(PG_FUNCTION_ARGS) {
    PG_RETURN_BOOL(text_eq(PG_GETARG_TEXT_PP(0),PG_GETARG_TEXT_PP(1)));
}
PG_FUNCTION_INFO_V1(san_ne);
Datum san_ne(PG_FUNCTION_ARGS) {
    PG_RETURN_BOOL(!text_eq(PG_GETARG_TEXT_PP(0),PG_GETARG_TEXT_PP(1)));
}

PG_FUNCTION_INFO_V1(fen_input);
Datum fen_input(PG_FUNCTION_ARGS) {
    char *input_str = PG_GETARG_CSTRING(0);
    Fen *fen = fen_constructor(input_str);
    PG_RETURN_FEN_P(fen);// Return the fen data type
}

PG_FUNCTION_INFO_V1(fen_output);
Datum fen_output(PG_FUNCTION_ARGS) {
    Fen *fen = PG_GETARG_FEN_P(0); // Get input text as a text pointer
    char *result = palloc0(sizeof(100));
    int len = 100;//87
    snprintf(result,len,"%s/%s/%s/%s/%s/%s/%s/%s %s %s %s %s %s", fen->board[0],fen->board[1],fen->board[2],fen->board[3],fen->board[4],fen->board[5]
             ,fen->board[6],fen->board[7],fen->turn,fen->rock,fen->en_passant,fen->half_move,fen->full_move);
    PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(get_first_moves);
Datum get_first_moves(PG_FUNCTION_ARGS) {
    text *input_text = PG_GETARG_TEXT_P(0); // Get input text as a text pointer
    char *input_str = text_to_cstring(input_text); // Convert text to C string
    int input_int = PG_GETARG_INT32(1);
    PG_RETURN_CSTRING(internal_get_first_moves(input_str,input_int));
}

PG_FUNCTION_INFO_V1(get_board);
Datum get_board(PG_FUNCTION_ARGS) {
    text *input_text = PG_GETARG_TEXT_P(0); // Get input text as a text pointer
    char *input_str = text_to_cstring(input_text); // Convert text to C string
    int input_int = PG_GETARG_INT32(1);
    PG_RETURN_CSTRING(internal_get_board(input_str,input_int));
}

PG_FUNCTION_INFO_V1(has_opening);
Datum has_opening(PG_FUNCTION_ARGS) {
    char *input_str_one = text_to_cstring(PG_GETARG_TEXT_P(0)); // Convert text to C string
    char *input_str_two = text_to_cstring(PG_GETARG_TEXT_P(1));
    PG_RETURN_BOOL(internal_has_opening(input_str_one,input_str_two));
}

PG_FUNCTION_INFO_V1(has_board);
Datum has_board(PG_FUNCTION_ARGS) {
    char *input_san = text_to_cstring(PG_GETARG_TEXT_P(0)); // Convert text to C string
    Fen *fen = PG_GETARG_FEN_P(1); // Get input text as a text pointer
    char *input_fen = palloc0(sizeof(100));
    int len = 100;//87
    snprintf(input_fen,len,"%s/%s/%s/%s/%s/%s/%s/%s", fen->board[0],fen->board[1],fen->board[2],fen->board[3],fen->board[4],fen->board[5]
            ,fen->board[6],fen->board[7]);
    int input_int = PG_GETARG_INT32(1);
    PG_RETURN_BOOL(internal_has_board(input_san,input_fen,input_int));
}

bool text_eq(text *text_one, text *text_two) {
    char *string_one = text_to_cstring(text_one); // Convert text to C string
    char *string_two = text_to_cstring(text_two); // Convert text to C string
    int result = strcmp(string_one, string_two);
    bool areEqual = (result == 0);
    return areEqual;
}

Fen *fen_constructor(char *fen_char){
    Fen *my_fen = palloc0(sizeof(Fen)+1);// todo remove the +1 on every char of the struct Fen and retry
    const char delimiters[] = " /";
    char *token;
    int count = 0;

    // Get the first token
    token = strtok(fen_char, delimiters);
    // Keep printing tokens until no more tokens are available
    while (token != NULL && count <= 12) {
        if(count <= 7){
            strcpy(my_fen->board[count],token);
        }
        switch(count) {
            case 8:
                strcpy(my_fen->turn,token);
                //my_fen->turn = (strcmp(token, "b") == 0);//black 0 white 1
                break;
            case 9:
                strcpy(my_fen->rock,token);
                break;
            case 10:
                strcpy(my_fen->en_passant,token);
                break;
            case 11:
                strcpy(my_fen->half_move,token);
                //my_fen->half_move = atoi(token);
                break;
            case 12:
                strcpy(my_fen->full_move,token);
                //my_fen->full_move = atoi(token);
                break;
        }
        // Get the next token
        token = strtok(NULL, delimiters);
        count += 1;
    }

    return my_fen;
}

char ** str_matrix_allocation(int rows, int columns){
    char **result;
    // Allocate memory for rows
    result = (char **)malloc(rows * sizeof(char *));
    for (int i = 0; i < rows; i++) {
        result[i] = (char *)malloc(columns * sizeof(char));
    }
    return result;
}

void free_str_matrix(char **matrix, int rows){
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

char* internal_get_first_moves(const char* PGN, int N){
    if(N<0){N=0;}//Negative numbers do not make sense
    int hMovesRemaining = N % 2;
    int fullMoves = (N / 2) + hMovesRemaining;//Moves to take into account
    if (hMovesRemaining == 0) { hMovesRemaining = 2; }//Number of half moves to take into account for the last move to account for

    //char firstMoves[N+fullMoves][7];//list of moves (Move #, half move white, half move black)
    char **firstMoves = str_matrix_allocation(N+fullMoves,7);//list of moves (Move #, half move white, half move black)
    char* firstMovesSolution = (char*)malloc(((N+fullMoves)*7) * sizeof(char));
    strcpy(firstMovesSolution, "");//Copy empty stream into the char* to remove unwanted artifacts

    if(N>0) {//only if we need at least one half-move
        size_t length = strlen(PGN);
        char *PGNcopy = (char*)malloc(length+1);
        strcpy(PGNcopy, PGN);//copy the const PGN so whe can modify it

        const char *separators = " .";//separators are " " and ".", so the notation accepted are {1. e3 E5 2. a3 ...} and {1.e3 E5 2.a3 ...}
        char strFullMoves[10];
        sprintf(strFullMoves, "%d", fullMoves);//string containing the move where we are stopping, converted to string

        char *strToken = strtok(PGNcopy, separators);//recuperating the first character before a point or a space
        int finalMove = strcmp(strToken, strFullMoves);

        char period[] = ".";
        char space[] = " ";
        int count = 0;
        while (strToken != NULL &&
               hMovesRemaining > 0) {//stop when we read all the PGN or we read all the half moves we needed
            strcpy(firstMoves[count], strToken);//adding Move number to the list
            strcat(firstMoves[count], period);//adding point after the move number to return in PGN notation

            strToken = strtok(NULL, separators);//recuperating the first half move
            if (strToken != NULL) {
                count += 1;
                strcpy(firstMoves[count], strToken);//adding First half move to the list
                strcat(firstMoves[count], space);//adding space between two half moves
                if (finalMove == 0) {//if we have arrived at the last move
                    hMovesRemaining -= 1;//remove a half move
                }

                if (hMovesRemaining >= 1) {//verify that we still have half moves left to copy
                    strToken = strtok(NULL, separators);//recuperating the second half move
                    if (strToken != NULL) {
                        count += 1;
                        strcpy(firstMoves[count], strToken);//adding Second half move to the list
                        strcat(firstMoves[count], space);//adding space between second half move and next move number

                        if (finalMove == 0) {//if we have arrived at the last move
                            hMovesRemaining -= 1;//remove a half move
                        } else {
                            strToken = strtok(NULL, separators);//recuperating the next Move number
                            finalMove = strcmp(strToken,
                                               strFullMoves);//compare the next move number with the one where we should stop
                            count += 1;//+3 in total because we have (move number, first half move, second half move)
                        }
                    }
                }
            }
        }
        for (int i = 0; i < count + 1; i++) {//iteration to concatenate the list of move numbers and half moves into a single string
            strcat(firstMovesSolution, firstMoves[i]);
        }
        free(PGNcopy);
    }
    free_str_matrix(firstMoves,N+fullMoves);
    return firstMovesSolution;
}

char* get_only_board(SCL_Board board){
    char* solution;
    solution = (char*)malloc(85 * sizeof(char));
    int t = SCL_boardToFEN(board, solution);
    strcpy(solution, board);
    char* parseSolution = (char*)malloc(85 * sizeof(char)); 
    strcpy(parseSolution, "");
    int count = 0;
    char strcount[2];
    char temp[2];
    for(int i=0; i<8; i++){
        count = 0;
        for(int k = i*8; k<(i+1)*8; k++){
            sprintf(temp, "%c", solution[k]);
            if (strcmp(temp, ".")==0){
                count++;
            }
            else{
                if (count>0){
                    sprintf(strcount, "%d", count);
                    strcat(parseSolution, strcount);
                    count = 0;
                }
                strcat(parseSolution, temp);
                
            }
        }
        if (count>0){
            sprintf(strcount, "%d", count);
            strcat(parseSolution, strcount);
                    
        }        
        if(i<7){
            strcat(parseSolution, "/");
            
        }  
        
    }
    free(solution);
    return parseSolution;
}

char* internal_get_board(const char* PGN, int N){
    char* solution;
    solution = (char*)malloc(85 * sizeof(char));//allocation of size 85 char(FEN notation does not exceed 85 character)
    strcpy(solution,"");//Copy empty stream into the char* to remove unwanted artifacts
    SCL_Record record;//Hold record of the game
    char* firstMoves = internal_get_first_moves(PGN,N);
    SCL_recordFromPGN(record,firstMoves);
    SCL_Board board;//Holds state of the board
    SCL_boardInit(board);
    uint16_t halfMoves = (uint16_t)N;
    SCL_recordApply(record, board, halfMoves);//Applies the record of moves one after another on a board    
    char* parseSolution = get_only_board(board);
    ereport(INFO, errmsg("%x", board[64]));
    //int en_passant_castling = strtol(board[64], NULL, 16);
    free(firstMoves);
    return parseSolution;
}

bool internal_has_opening(const char* PGNone, const char* PGNtwo){
    bool solution = true;

    size_t lengthOne = strlen(PGNone);
    char *PGNoneCopy = (char*)malloc(lengthOne+1);
    strcpy(PGNoneCopy, PGNone);//copy the const PGN so whe can modify it
    char* pointerOne = NULL;

    size_t lengthTwo = strlen(PGNtwo);
    char *PGNtwoCopy = (char*)malloc(lengthTwo+1);
    strcpy(PGNtwoCopy, PGNtwo);//copy the const PGN so whe can modify it
    char* pointerTwo = NULL;

    const char *separators = " .";//separators are " " and ".", so the notation accepted are {1. e3 E5 2. a3 ...} and {1.e3 E5 2.a3 ...}

    char *strTokenOne = strtok_r(PGNoneCopy, separators,&pointerOne);//recuperating the first character before a point or a space
    char *strTokenTwo = strtok_r(PGNtwoCopy, separators,&pointerTwo);//recuperating the first character before a point or a space
    while (strTokenOne != NULL && strTokenTwo != NULL && solution) {//stop when we read all the PGN or we read all the half moves we needed
        if(strcmp(strTokenOne, strTokenTwo) != 0) {//compare the next move number with the one where we should stop
            solution = false;
        }
        strTokenOne = strtok_r(NULL, separators, &pointerOne);//recuperating the next character before a point or a space
        strTokenTwo = strtok_r(NULL, separators, &pointerTwo);//recuperating the next character before a point or a space
    }
    if(strTokenOne == NULL && strTokenTwo != NULL && solution){//if the chess game we are looking into finishes before the reference chess game
        solution = false;
    }
    free(PGNoneCopy);
    free(PGNtwoCopy);
    return solution;
}



/*TODO Function can be greatly optimized by registering and playing every movement one
 * at a time instead of recreating the board for the PGN notation for every half moves
 * until we arrive at N*/
bool internal_has_board(const char* PGN, const char* FEN, int N){
    char *token = (char*)malloc(72 * sizeof(char));
    bool solution = true;
    int countHalfMoves = 0;//start the half moves counter at one
    int found = 1;//found the game state is false by default
    while((countHalfMoves<N+1 && found != 0)){//stop when we have verified all the half moves or if we find the board state
        char* tempFirstMovesBoard = internal_get_board(PGN, countHalfMoves);//getting the FEN of the board state for every half move played
        token = strtok(tempFirstMovesBoard, " ");
        found = strcmp(FEN, token);//compare the board state every time a move is played
        countHalfMoves += 1;
        free(tempFirstMovesBoard);
    }
    if((countHalfMoves == N+1 && found != 0) || N<0){//verify if we have found the board state or if we have finished iterating without finding anything
        solution = false;
    }
    free(token);
    return solution;
}
