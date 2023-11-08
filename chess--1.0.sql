/*
* I/O
*/

CREATE OR REPLACE FUNCTION san_input(cstring)
    RETURNS san
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION san_output(san)
    RETURNS cstring
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION fen_input(cstring)
    RETURNS fen
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION fen_output(fen)
    RETURNS cstring
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*
* DATA TYPES
*/
CREATE TYPE san (
    internallength = 850,
    input = san_input,
    output = san_output);

CREATE TYPE fen (
    internallength = 850,
    input = fen_input,
    output = fen_output);

/*
* operators
*/

CREATE OR REPLACE FUNCTION san_eq(san, san)
    RETURNS boolean
    AS 'MODULE_PATHNAME','san_eq'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION san_ne(san, san)
    RETURNS boolean
    AS 'MODULE_PATHNAME','san_ne'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

--operator for two san
CREATE OPERATOR = (
    LEFTARG = san, RIGHTARG = san,
    PROCEDURE = san_eq,
    COMMUTATOR = =, NEGATOR = <>
    );

--operator for two san
CREATE OPERATOR <> (
    LEFTARG = san, RIGHTARG = san,
    PROCEDURE = san_ne,
    COMMUTATOR = <>, NEGATOR = =
    );