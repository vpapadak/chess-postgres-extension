#include <postgres.h>
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include "fmgr.h"
#include <string.h>

PG_MODULE_MAGIC;


typedef struct {
    char *san;//max size<800
} SanString;

typedef struct {
    char *fen;//max size<90
} FenString;

bool text_eq(text *text_one, text *text_two);

PG_FUNCTION_INFO_V1(san_input);
Datum san_input(PG_FUNCTION_ARGS) {
    char *input_text = PG_GETARG_CSTRING(0);
    char *result = (char *) palloc(strlen(input_text) + 1000);
    strcpy(result, input_text);
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(san_output);
Datum san_output(PG_FUNCTION_ARGS) {
    text *output_text = PG_GETARG_TEXT_PP(0); // Get input text as a text pointer
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
    char *input_text = PG_GETARG_CSTRING(0);
    FenString *result = (FenString *) palloc(sizeof(char[100]));
    result->fen = input_text;
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(fen_output);
Datum fen_output(PG_FUNCTION_ARGS) {
    text *output_text = PG_GETARG_TEXT_PP(0); // Get input text as a text pointer
    char *output_str = text_to_cstring(output_text); // Convert text to C string
    FenString *output = (FenString *) PG_GETARG_POINTER(0);
    output->fen = output_str;
    PG_RETURN_CSTRING(output);
}

bool text_eq(text *text_one, text *text_two) {
    char *string_one = text_to_cstring(text_one); // Convert text to C string
    char *string_two = text_to_cstring(text_two); // Convert text to C string
    int result = strcmp(string_one, string_two);
    bool areEqual = (result == 0);
    return areEqual;
}
