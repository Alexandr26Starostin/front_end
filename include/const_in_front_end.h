#ifndef CONST_IN_FRONT_END_H
#define CONST_IN_FRONT_END_H

enum front_end_error_t
{
	NOT_ERROR 				             = 0,
	NOT_MEMORY_FOR_LIST_OF_FUNC          = 1,
	NOT_MEMORY_FOR_REALLOC__LIST_OF_FUNC = 2
};

//--------------------------------------------------------------------------------

const size_t SIZE_LIST_OF_FUNC = 14;

enum base_func_t
{
	ADD = 0,
	SUB = 1,
	MUL = 2,
	DIV = 3,

	SIN = 4, 
	COS = 5,
	SH  = 6,
	CH  = 7,

	SQRT = 8,
	LOG  = 9,
	LN   = 10,
	DEG  = 11
};

//const char  name_func[MAX_LEN_BASE_FUNC];

struct name_base_func_t
{
	const char* name_func;
	base_func_t code_func;
};

struct list_of_func_t
{
	name_base_func_t* list_of_func;
	size_t            size_of_list;
	size_t            free_index;
};

//-----------------------------------------------------------------------------------

#endif