#ifndef CONST_IN_FRONT_END_H
#define CONST_IN_FRONT_END_H

//------------------------------------------------------------------------------
/*errors*/

enum front_end_error_t
{
	NOT_ERROR 				               = 0,
	NOT_MEMORY_FOR_LIST_OF_FUNC            = 1,
	NOT_MEMORY_FOR_REALLOC_LIST_OF_FUNC    = 2,
	NOT_FIND_FILE_WITH_PROGRAM             = 3,
	NOT_MEMORY_FOR_STR_WITH_PROGRAM        = 4,
	NOT_MEMORY_FOR_NAME_TABLE              = 5,
	NOT_MEMORY_FOR_REALLOC_NAME_TABLE      = 6,
	NOT_MEMORY_FOR_ARRAY_OF_TOKENS         = 7,
	NOT_MEMORY_FOR_REALLOC_ARRAY_OF_TOKENS = 8
};

//--------------------------------------------------------------------------------
/*list_of_func*/

const size_t SIZE_LIST_OF_FUNC = 5;
const size_t MAX_VALUE_OF_ENUM = 40;
const long   WORD_IS_NOT_FUNC  = -1;

enum base_func_t
{
	ADD = 0,   // +
	SUB = 1,   // -
	MUL = 2,   // *
	DIV = 3,   // /

	SIN = 4,   // sin
	COS = 5,   // cos
	SH  = 6,   // sinh
	CH  = 7,   // cosh

	SQRT = 8,    // sqrt
	LOG  = 9,    // log
	LN   = 10,   // ln
	DEG  = 11,   // ^

	IF       = 12,      // if 
	WHILE    = 13,      // while
	MAIN     = 14,      // main
	INT      = 15,      // int/long
	ASSIGN   = 16,      // =
	SCANF    = 17,      // scanf
	PRINTF   = 18,      // printf
	OPERATOR = 19,      // ;
	RETURN   = 20,      // return

	ROUND_BEGIN = 21,   // (
	ROUND_END   = 22,   // )
	CURLY_BEGIN = 23,   // {
	CURLY_END   = 24,   // }

	COMMENT     = 25    // //
};

struct name_base_func_t
{
	const char* name_func;
	base_func_t code_func;
};

struct list_of_func_t
{
	name_base_func_t* array_of_func;
	size_t            size_of_list;
	size_t            free_index;
};

//---------------------------------------------------------------------------------------
/*operations_with_files*/

const int POSITION_NAME_FILE_IN_ARGV = 1;

//---------------------------------------------------------------------------------------
/*name_table*/

const size_t SIZE_NAME_TABLE = 5;

struct name_t
{
	size_t index_to_name_in_str;
	size_t len_name;
};

struct name_table_t
{
	name_t* name_table;
	size_t  size_of_name_table;
	size_t  free_index;
};

//--------------------------------------------------------------------------------------
/*tokens*/

const size_t SIZE_ARRAY_OF_TOKENS = 5;

enum type_value_t 
{
	NUM = 1,
	OP  = 2,
	ID  = 3
};

union value_t
{
	double      value_num;
	base_func_t value_op;
	size_t      value_id; 
};

struct token_t 
{
	type_value_t  type;
	union value_t value;
};

struct array_of_tokens_t
{
	token_t* array_of_tokens;
	name_t*  name_table;
	size_t   size_of_array;
	size_t   index_free;
};

//---------------------------------------------------------------------------------------
/*lexical_analysis*/

const size_t MAX_LEN_WORD_FROM_STR = 50;

//---------------------------------------------------------------------------------------

#endif