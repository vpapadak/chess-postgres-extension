#include <postgres.h>
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "fmgr.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    /*ereport(ERROR,
            (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("print: %s", output),
                    errdetail("Additional details about the error")));*/
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
