#include "postgres.h"
#include "fmgr.h"

PG_MODULE_MAGIC;

typedef struct {
    char *san;//max size<800
} SanString;

typedef struct {
    char *fen;//max size<90
} FenString;

Datum my_san_string_type_input(PG_FUNCTION_ARGS);
Datum my_san_string_type_output(PG_FUNCTION_ARGS);

Datum my_fen_string_type_input(PG_FUNCTION_ARGS);
Datum my_fen_string_type_output(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(my_san_string_type_input);
PG_FUNCTION_INFO_V1(my_san_string_type_output);

PG_FUNCTION_INFO_V1(my_fen_string_type_input);
PG_FUNCTION_INFO_V1(my_fen_string_type_output);

Datum my_san_string_type_input(PG_FUNCTION_ARGS) {
    char *input_text = PG_GETARG_CSTRING(0);
    SanString *result = (SanString *) malloc(sizeof(SanString));
    result->san = input_text;
    PG_RETURN_POINTER(result);
}

Datum my_san_string_type_output(PG_FUNCTION_ARGS) {
    text *output_text = PG_GETARG_TEXT_PP(0); // Get input text as a text pointer
    char *output_str = text_to_cstring(output_text); // Convert text to C string
    SanString *output = (SanString *) PG_GETARG_POINTER(0);
    output->san = output_str;
    PG_RETURN_CSTRING(output);
}

Datum my_fen_string_type_input(PG_FUNCTION_ARGS) {
    char *input_text = PG_GETARG_CSTRING(0);
    FenString *result = (FenString *) malloc(sizeof(FenString));
    result->fen = input_text;
    PG_RETURN_POINTER(result);
}

Datum my_fen_string_type_output(PG_FUNCTION_ARGS) {
    text *output_text = PG_GETARG_TEXT_PP(0); // Get input text as a text pointer
    char *output_str = text_to_cstring(output_text); // Convert text to C string
    FenString *output = (FenString *) PG_GETARG_POINTER(0);
    output->fen = output_str;
    PG_RETURN_CSTRING(output);
}
