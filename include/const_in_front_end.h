#ifndef CONST_IN_FRONT_END_H
#define CONST_IN_FRONT_END_H

enum front_end_error_t
{
	NOT_ERROR 				             = 0,
	NOT_MEMORY_FOR_LIST_OF_FUNC          = 1,
	NOT_MEMORY_FOR_REALLOC__LIST_OF_FUNC = 2,
	NOT_FIND_FILE_WITH_PROGRAM           = 3,
	NOT_MEMORY_FOR_STR_WITH_PROGRAM      = 4
};

//--------------------------------------------------------------------------------

const size_t SIZE_LIST_OF_FUNC = 5;

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

//const char  name_func[MAX_LEN_BASE_FUNC];

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

//--------------------------------------------------------------------------------------

const int POSITION_NAME_FILE_IN_ARGV = 1;

//---------------------------------------------------------------------------------------

#endif