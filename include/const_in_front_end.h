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
	NOT_MEMORY_FOR_REALLOC_ARRAY_OF_TOKENS = 8,
	NOT_MEMORY_FOR_NEW_NODE                = 9,
	NOT_FIND_TREE_DOT                      = 10,
	NOT_FIND_TREE_HTML                     = 11
};

//--------------------------------------------------------------------------------
/*list_of_func*/

const size_t SIZE_LIST_OF_FUNC = 5;
const size_t MAX_VALUE_OF_ENUM = 40;
const long   WORD_IS_NOT_FUNC  = -1;

enum base_func_t
{
	//------------------------------------------------------------------------------------------
	/*standard*/

	IF       = 11,      // if 
	WHILE    = 12,      // while
	ASSIGN   = 13,      // =

	SIN = 21,   // sin
	COS = 22,   // cos

	//FLOOR = 23,

	ADD = 24,   // +
	SUB = 25,   // -
	MUL = 26,   // *
	DIV = 27,   // /

	//DIFF = 28,

	SQRT = 29,    // sqrt

	//== = 31,
	//<  = 32,
	//>  = 33,
	//<= = 34,
	//>= = 35,
	//!= = 36,
	//&& = 37,
	//|| = 38,
	//!  = 39,

	OPERATOR = 41,      // ;

	//, = 42,

	INT = 51,      // int/long/doubles

	SCANF  = 61,      // scanf
	PRINTF = 62,      // printf

	RETURN = 71,      // return

	//BREAK    = 72,
	//CONTINUE = 73,
	//ABORT    = 74,

	//-------------------------------------------------------------------------------
	/*other*/
	
	SH = 81,   // sinh
	CH = 82,   // cosh

	LOG = 83,    // log
	LN  = 84,   // ln
	DEG = 85,   // ^

	MAIN     = 86,      // main
	
	ROUND_BEGIN = 91,   // (
	ROUND_END   = 92,   // )
	CURLY_BEGIN = 93,   // {
	CURLY_END   = 94,   // }

	COMMENT = 95    // //
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

enum token_type_t 
{
	NUM = 1,
	OP  = 2,
	ID  = 3
};

union token_value_t
{
	double      value_num;
	base_func_t value_op;
	size_t      value_id; 
};

struct token_t 
{
	token_type_t        type;
	union token_value_t value;
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
/*tree*/

const size_t INDEX_NUMBER_OF_PICTURE = 36;

enum node_type_t 
{
	CONSTANT            = 1,
	IDENTIFIER          = 2,
	KEYWORD             = 3,
	FUNCTION_DEFINITION = 4,
	PARAMETERS          = 5,
	VAR_DECLARATION     = 6,
	CALL                = 7
};

union node_value_t
{
	double      value_constant;
	size_t      value_identifier;
	base_func_t value_keyword;
	size_t      value_function_definition;
	double      value_parameters;            //not value_parameters
	size_t      value_var_declaration;
	double      value_call;                 //not value_call
};

struct node_t
{
	node_type_t  type;
	node_value_t value;
	node_t*      left;
	node_t*      right;
	node_t*      parent;
};

//---------------------------------------------------------------------------------------

#endif

//Добавить типы и изменить value