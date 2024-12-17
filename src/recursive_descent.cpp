#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "const_in_front_end.h"
#include "operations_with_files.h"
#include "name_table.h"
#include "tokens.h"
#include "tree.h"
#include "recursive_descent.h"

//TODO      дописать функции

static front_end_error_t get_grammar                   (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);  //
static front_end_error_t get_operation                 (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       // 
static front_end_error_t get_assign                    (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
static front_end_error_t get_var_declaration           (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
// static front_end_error_t get_if                        (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
// static front_end_error_t get_while                     (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
// static front_end_error_t get_printf                    (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
// static front_end_error_t get_scanf                     (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
// static front_end_error_t get_interruption              (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
// static front_end_error_t get_empty_operation           (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
// static front_end_error_t get_definition_func           (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
// static front_end_error_t get_return                    (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
static front_end_error_t get_logical                   (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
static front_end_error_t get_and                       (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
static front_end_error_t get_not                       (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
static front_end_error_t get_or                        (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
// static front_end_error_t get_parameters_for_definition (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);
static front_end_error_t get_expression                (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
static front_end_error_t get_term                      (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
static front_end_error_t get_power                     (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       // 
static front_end_error_t get_base_func                 (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
static front_end_error_t get_round                     (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
static front_end_error_t get_element                   (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
static front_end_error_t get_constant                  (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
static front_end_error_t get_variable                  (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);       //
// static front_end_error_t get_call_func                 (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node);

#define error_massage_ printf ("Error in %s:%d\n\n", __FILE__, __LINE__);

#define token_type_(index)      (syntactic_parameters -> tokens -> array_of_tokens)[syntactic_parameters -> index_token + index].type
#define token_value_num_(index) (syntactic_parameters -> tokens -> array_of_tokens)[syntactic_parameters -> index_token + index].value.value_num
#define token_value_op_(index)  (syntactic_parameters -> tokens -> array_of_tokens)[syntactic_parameters -> index_token + index].value.value_op
#define token_value_id_(index)  (syntactic_parameters -> tokens -> array_of_tokens)[syntactic_parameters -> index_token + index].value.value_id

#define check_token_type_(index, type_token)   (token_type_(index)     == type_token)
#define check_op_value_(index, op_value)       (token_value_op_(index) == op_value)

#define constant_node_(number)                                      create_node (CONSTANT,            number,                       NULL, NULL,  NULL)   
#define identifier_node_(index_in_name_table)                       create_node (IDENTIFIER,          (double) index_in_name_table, NULL, NULL,  NULL)
#define keyword_node_(code_keyword, left, right)                    create_node (KEYWORD,             (double) code_keyword,        left, right, NULL)
#define function_definition_node_(index_in_name_table, left, right) create_node (FUNCTION_DEFINITION, (double) index_in_name_table, left, right, NULL)
#define parameters_node_(left, right)                               create_node (PARAMETERS,          0,                            left, right, NULL)
#define var_declaration_node_(index_in_name_table, left, right)     create_node (VAR_DECLARATION,     (double) index_in_name_table, left, right, NULL)
#define call_node_(left, right)                                     create_node (CALL,                0,                            left, right, NULL)

#define try_perform_operation_(name_func, name_skip_command)                                            			\
	syntactic_parameters -> index_token = old_index;                                                    			\
																							            			\
	status = name_func (syntactic_parameters, &operation_node);                                    					\
	if (status != name_skip_command)                                                                    			\
	{                                                                                                   			\
		if (status) {return status;}                                                                   				\
                                                            								            			\
		(*ptr_node)    -> left            = operation_node;                             							\
		operation_node -> parent          = *ptr_node;               												\
		(*ptr_node)    -> right           = keyword_node_(OPERATOR, NULL, NULL);        							\
																													\
		if ((*ptr_node) -> right == NULL) {error_massage_ free (*ptr_node); return NOT_MEMORY_FOR_NEW_NODE;}   		\
																													\
		(*ptr_node) -> right -> parent = *ptr_node;            		  								 				\
		(*ptr_node)                    = (*ptr_node) -> right;  	                      							\
																							  						\
		return status;                                                                        						\
	}    

#define READ_BASE_FUNC_IN_GET_BASE_FUNC_(name_base_func)                                  				\
	if (check_op_value_(0, name_base_func))   											  				\
	{                             													      				\
		(syntactic_parameters -> index_token) += 1;										  				\
																					      				\
		status = get_round (syntactic_parameters, &round_node);                           				\
		if (status) {return status;}                            					      				\
																					      				\
		*ptr_node = keyword_node_(name_base_func, NULL, round_node);                      				\
																						 				\
		if ((*ptr_node) == NULL) {error_massage_ free (round_node); return NOT_MEMORY_FOR_NEW_NODE;}    \
																				 	      				\
		return NOT_ERROR;                                                                 				\
	}

#define ASSERTS_IN_RECURSIVE_DESCENT_ 										    \
	assert (syntactic_parameters);												\
	assert (syntactic_parameters -> tokens);									\
	assert (ptr_node);															\
	assert (syntactic_parameters -> tokens -> array_of_tokens);					\
	assert (syntactic_parameters -> tokens -> name_table);		

//--------------------------------------------------------------------------------

front_end_error_t recursive_descent (array_of_tokens_t* tokens, node_t* root_node)
{
	assert (tokens);
	assert (root_node);
	assert (tokens -> array_of_tokens);
	assert (tokens -> name_table);

	size_t index_token = 0;
	long   scope       = -1;

	syntactic_parameters_t syntactic_parameters = {};

	syntactic_parameters.index_token = index_token;
	syntactic_parameters.scope       = scope;
	syntactic_parameters.tokens      = tokens;

	front_end_error_t status = get_grammar (&syntactic_parameters, &root_node);

	return status;
}

static front_end_error_t get_grammar (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status = NOT_ERROR;

	node_t* grammar_node = *ptr_node;

	while (status == NOT_ERROR && syntactic_parameters -> index_token != syntactic_parameters -> tokens -> index_free) 
		{status = get_operation (syntactic_parameters, &grammar_node);}

	if (status) {return status;}

	if (syntactic_parameters -> index_token != syntactic_parameters -> tokens -> index_free)
	{
		error_massage_
		printf ("Error from 'get_grammar': wait position: %ld, "
		        "but get position: '%ld'\n\n", 
				syntactic_parameters -> tokens -> index_free, syntactic_parameters -> index_token);

		return ERROR_IN_GET_GRAMMAR;
	}

	syntactic_parameters -> index_token += 1;

	return status;
}                

static front_end_error_t get_operation (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status         = NOT_ERROR;
	size_t            old_index      = syntactic_parameters -> index_token;
	node_t*           operation_node = NULL;

	try_perform_operation_(get_assign,          SKIP_GET_ASSIGN)
	// try_perform_operation_(get_if,              SKIP_GET_IF)
	// try_perform_operation_(get_while,           SKIP_GET_WHILE)
	// try_perform_operation_(get_printf,          SKIP_GET_PRINTF)
	// try_perform_operation_(get_scanf,           SKIP_GET_SCANF)
	// try_perform_operation_(get_interruption,    SKIP_GET_INTERRUPTION)
	// try_perform_operation_(get_empty_operation, SKIP_GET_EMPTY_OPERATION)
	// try_perform_operation_(get_definition_func, SKIP_GET_DEFINITION_FUNC)
	// try_perform_operation_(get_return,          SKIP_GET_RETURN)

	error_massage_
	printf ("Error from 'get_operation': in position: %ld operation not definite\n",
			syntactic_parameters -> index_token);

	return ERROR_IN_GET_OPERATION;
}

static front_end_error_t get_assign (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	node_t*           expression_node = NULL;
	node_t*           variable_node   = NULL;
	front_end_error_t status          = NOT_ERROR;

	if (check_token_type_(0, OP) && check_op_value_(0, INT) && check_token_type_(2, OP) &&  ! check_op_value_(2, ROUND_BEGIN))  //double <var> ==  
	{
		status = get_var_declaration (syntactic_parameters, ptr_node);
		if (status) {return status;}

		return NOT_ERROR;
	}

	if (!check_token_type_(0, ID))
	{
		error_massage_
		printf ("Error from 'get_assign': in position in token == %ld wait ASSIGN ('ИЛ'), but not find it\n\n", 
		syntactic_parameters -> index_token + 1);

		return SKIP_GET_ASSIGN;
	}
	
	size_t index_id_in_name_table = token_value_id_(0);

	if ((syntactic_parameters -> tokens -> name_table)[index_id_in_name_table].status == NOT_DEFINITE)
	{
		error_massage_
		printf ("Error from 'get_assign': not have declaration of var (index_id in name_table == %ld) in position in tokens == %ld\n\n", 
				index_id_in_name_table, syntactic_parameters -> index_token);

		return ERROR_IN_GET_ASSIGN;
	}

	if (! (check_token_type_(1, OP) && check_op_value_(1, ASSIGN)))
	{
		error_massage_
		printf ("Error from 'get_assign': in position in token == %ld wait ASSIGN ('ИЛ'), but not find it\n\n", 
		syntactic_parameters -> index_token + 1);

		return ERROR_IN_GET_ASSIGN;
	}

	status = get_variable (syntactic_parameters, &variable_node); 
	if (status) {return status;}

	syntactic_parameters -> index_token += 1;   //пропуск '=' <==> 'ИЛ'

	status = get_expression (syntactic_parameters, &expression_node); 
	if (status) {return status;}

	if (! (check_token_type_(0, OP) && check_op_value_(0, OPERATOR)))
	{
		error_massage_
		printf ("Error from 'get_var_declaration': in position in token == %ld wait OPERATION ('\\n'), but not find it\n\n", 
		syntactic_parameters -> index_token);

		return ERROR_IN_GET_ASSIGN;
	}

	syntactic_parameters -> index_token += 1;  //пропуск ';' <==> '\n'

	*ptr_node = keyword_node_(ASSIGN, expression_node, variable_node);
	if (*ptr_node == NULL) {error_massage_ free (expression_node); free (variable_node); return NOT_MEMORY_FOR_NEW_NODE;}

	return NOT_ERROR;
}

static front_end_error_t get_var_declaration (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status   = NOT_ERROR;
	node_t*           var_node = NULL;

	if (! check_token_type_(1, ID))
	{
		error_massage_
		printf ("Error from 'get_var_declaration': in position in tokens == %ld not find name_variable\n\n", 
		        syntactic_parameters -> index_token + 1);

		return ERROR_IN_GET_VAR_DECLARATION;
	}

	size_t index_id_in_name_table =  token_value_id_(1);

	if ((syntactic_parameters -> tokens -> name_table)[index_id_in_name_table].status)
	{
		error_massage_
		printf ("Error from 'get_var_declaration': repeated declaration of var (index_id in name_table == %ld) in position in tokens == %ld\n\n", 
				index_id_in_name_table, syntactic_parameters -> index_token + 1);

		return ERROR_IN_GET_VAR_DECLARATION;
	}

	(syntactic_parameters -> tokens -> name_table)[index_id_in_name_table].status = DEFINITE;
	(syntactic_parameters -> tokens -> name_table)[index_id_in_name_table].type   = NAME_VAR;

	if (check_token_type_(2, OP) && check_op_value_(2, ASSIGN))
	{
		syntactic_parameters -> index_token += 1;

		status = get_assign (syntactic_parameters, &var_node); 
		if (status) {return status;}

		*ptr_node = var_declaration_node_(index_id_in_name_table, keyword_node_(INT, NULL, NULL), var_node);
		if (*ptr_node == NULL) {error_massage_; free (var_node); return NOT_MEMORY_FOR_NEW_NODE;}

		return NOT_ERROR;
	}

	else if (check_token_type_(2, OP) && check_op_value_(2, OPERATOR))
	{
		syntactic_parameters -> index_token += 1;

		status = get_variable (syntactic_parameters, &var_node);
		if (status) {return status;}

		syntactic_parameters -> index_token += 1;

		*ptr_node = var_declaration_node_(index_id_in_name_table, keyword_node_(INT, NULL, NULL), var_node);
		if (*ptr_node == NULL) {error_massage_; free (var_node); return NOT_MEMORY_FOR_NEW_NODE;}

		return NOT_ERROR;
	}

	else
	{
		error_massage_
		printf ("Error from 'get_var_declaration': in position in token == %ld wait ASSIGN ('ИЛ') or OPERATOR ('\n'), but not find it \n\n", 
		syntactic_parameters -> index_token + 2);

		return ERROR_IN_GET_VAR_DECLARATION;
	}
}

static front_end_error_t get_expression (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status          = NOT_ERROR;
	node_t*           term_node       = NULL;
	size_t            old_index_token = 0;

	status = get_term (syntactic_parameters, ptr_node);
	if (status) {return status;}

	while (check_token_type_(0, OP) && (check_op_value_(0, ADD) || check_op_value_(0, SUB)))
	{
		old_index_token = syntactic_parameters -> index_token;

		syntactic_parameters -> index_token += 1;

		status = get_term (syntactic_parameters, &term_node);
		if (status) {return status;}
		
		*ptr_node = keyword_node_((syntactic_parameters -> tokens -> array_of_tokens)[old_index_token].value.value_op, *ptr_node, term_node);
		if (*ptr_node == NULL) {error_massage_; free (term_node); return NOT_MEMORY_FOR_NEW_NODE;}

	}

	return NOT_ERROR;
}

static front_end_error_t get_term (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status          = NOT_ERROR;
	node_t*           power_node      = NULL;
	size_t            old_index_token = 0;

	status = get_power (syntactic_parameters, ptr_node);
	if (status) {return status;}

	while (check_token_type_(0, OP) && (check_op_value_(0, MUL) || check_op_value_(0, DIV)))
	{
		old_index_token = syntactic_parameters -> index_token;

		syntactic_parameters -> index_token += 1;

		status = get_term (syntactic_parameters, &power_node);
		if (status) {return status;}

		*ptr_node = keyword_node_((syntactic_parameters -> tokens -> array_of_tokens)[old_index_token].value.value_op, *ptr_node, power_node);
		if (*ptr_node == NULL) {error_massage_; free (power_node); return NOT_MEMORY_FOR_NEW_NODE;}
	}

	return NOT_ERROR;
}

static front_end_error_t get_power (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status         = NOT_ERROR;
	node_t*           base_func_node = NULL;

	status = get_base_func (syntactic_parameters, ptr_node);
	if (status) {return status;}

	while (check_token_type_(0, OP) && check_op_value_(0, DEG))
	{
		syntactic_parameters -> index_token += 1;

		status = get_base_func (syntactic_parameters, &base_func_node);
		if (status) {return status;}

		*ptr_node = keyword_node_(DEG, *ptr_node, base_func_node);
		if (*ptr_node == NULL) {error_massage_; free (base_func_node); return NOT_MEMORY_FOR_NEW_NODE;}
	}

	return NOT_ERROR;
}

static front_end_error_t get_base_func (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status     = NOT_ERROR;
	node_t*           round_node = NULL;

	if (check_token_type_(0, OP))
	{
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(SIN)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(COS)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(SH)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(CH)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(SQRT)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(LN)
		//READ_BASE_FUNC_IN_GET_BASE_FUNC_(DIFF)
		//READ_BASE_FUNC_IN_GET_BASE_FUNC_(FLOOR)

		if (check_op_value_(0, LOG))   
		{                             													      
			syntactic_parameters -> index_token += 1;													      
																							
			status = get_round (syntactic_parameters, ptr_node);             
			if (status) {return status;}  

			status = get_round (syntactic_parameters, &round_node);             
			if (status) {return status;}                           					      
																							
			*ptr_node = keyword_node_(LOG, *ptr_node, round_node); 
			if (*ptr_node == NULL) {error_massage_; free (round_node); return NOT_MEMORY_FOR_NEW_NODE;}                     
																							
			return NOT_ERROR;                                                                 
		}
	}

	return get_round (syntactic_parameters, ptr_node);  
}

static front_end_error_t get_round (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status = NOT_ERROR;

	if (check_token_type_(0, OP) && check_op_value_(0, ROUND_BEGIN))
	{
		syntactic_parameters -> index_token += 1;

		status = get_expression (syntactic_parameters, ptr_node);    
		if (status) {return status;}

		if (! (check_token_type_(0, OP) && check_op_value_(0, ROUND_END)))
		{
			error_massage_
			printf ("Error from 'get_round': in position in tokens == %ld wait ROUND_END ('лети'), but not find it\n\n", syntactic_parameters -> index_token);

			return ERROR_IN_GET_ROUND;
		}

		syntactic_parameters -> index_token += 1;

		return NOT_ERROR;
	}

	return get_element (syntactic_parameters, ptr_node);
}

static front_end_error_t get_element (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	if (check_token_type_(0, NUM) && check_token_type_(1, OP)) {return get_constant (syntactic_parameters, ptr_node);}

	if (check_token_type_(0, ID) && check_token_type_(1, OP) && check_op_value_(1, ROUND_BEGIN)) 
		{return NOT_ERROR;} //get_call_func (tokens, ptr_index_token, ptr_node, ptr_scope);

	if (check_token_type_(0, ID) && check_token_type_(1, OP)) {return get_variable (syntactic_parameters, ptr_node);}

	error_massage_
	printf ("Error from 'get_round': in position in tokens == %ld wait OP after NUM or ID, but find ID\n\n", syntactic_parameters -> index_token + 1);

	return ERROR_IN_GET_ELEMENT;
}

static front_end_error_t get_constant (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	*ptr_node = constant_node_(token_value_num_(0));
	if (*ptr_node == NULL) {error_massage_; return NOT_MEMORY_FOR_NEW_NODE;}

	syntactic_parameters -> index_token += 1;

	return NOT_ERROR;
}

static front_end_error_t get_variable (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	*ptr_node = identifier_node_(token_value_id_(0));
	if (*ptr_node == NULL) {error_massage_; return NOT_MEMORY_FOR_NEW_NODE;}

	syntactic_parameters -> index_token += 1;

	return NOT_ERROR;
}

static front_end_error_t get_logical (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status          = NOT_ERROR;
	node_t*           or_node         = NULL;
	size_t            old_index_token = 0;

	if (! (check_token_type_(0, OP) && check_op_value_(0, ROUND_BEGIN)))
	{
		error_massage_
		printf ("Error from 'logical': in position in token == %ld wait ( ('к'), but not find it \n\n", 
		syntactic_parameters -> index_token);

		return ERROR_IN_GET_LOGICAL;
	}

	syntactic_parameters -> index_token += 1;

	status = get_or (syntactic_parameters, ptr_node);
	if (status) {return status;}

	if (! (check_token_type_(0, OP) && (check_op_value_(0, EQUALLY) || check_op_value_(0, LESS)    || 
										check_op_value_(0, MORE)    || check_op_value_(0, NO_MORE) || 
										check_op_value_(0, NO_LESS) || check_op_value_(0, NO_MORE) || check_op_value_(0, NOT_EQUALLY))))
	{
		error_massage_
		printf ("Error from 'logical': in position in token == %ld wait operation comparions: ==, !=, >, <, >-, <= - but not find it \n\n", 
		syntactic_parameters -> index_token);

		return ERROR_IN_GET_LOGICAL;
	}

	old_index_token                      = syntactic_parameters -> index_token;
	syntactic_parameters -> index_token += 1;

	status = get_or (syntactic_parameters, &or_node);
	if (status) {return status;}

	if (! (check_token_type_(0, OP) && check_op_value_(0, ROUND_END)))
	{
		error_massage_
		printf ("Error from 'logical': in position in token == %ld wait ) ('лети'), but not find it \n\n", 
		syntactic_parameters -> index_token);

		return ERROR_IN_GET_LOGICAL;
	}

	syntactic_parameters -> index_token += 1;

	*ptr_node = keyword_node_((syntactic_parameters -> tokens -> array_of_tokens)[old_index_token].value.value_op, *ptr_node, or_node);
	if (*ptr_node == NULL) {error_massage_; free (or_node); return NOT_MEMORY_FOR_NEW_NODE;}

	return NOT_ERROR;
}

static front_end_error_t get_or (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status          = NOT_ERROR;
	node_t*           and_node        = NULL;
	size_t            old_index_token = 0;

	status = get_and (syntactic_parameters, ptr_node);
	if (status) {return status;}

	while (check_token_type_(0, OP) && check_op_value_(0, OR))
	{
		old_index_token = syntactic_parameters -> index_token;

		syntactic_parameters -> index_token += 1;

		status = get_and (syntactic_parameters, &and_node);
		if (status) {return status;}
		
		*ptr_node = keyword_node_((syntactic_parameters -> tokens -> array_of_tokens)[old_index_token].value.value_op, *ptr_node, and_node);
		if (*ptr_node == NULL) {error_massage_; free (and_node); return NOT_MEMORY_FOR_NEW_NODE;}
	}

	return NOT_ERROR;
}

static front_end_error_t get_and (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status          = NOT_ERROR;
	node_t*           not_node        = NULL;
	size_t            old_index_token = 0;

	status = get_not (syntactic_parameters, ptr_node);
	if (status) {return status;}

	while (check_token_type_(0, OP) && check_op_value_(0, AND))
	{
		old_index_token = syntactic_parameters -> index_token;

		syntactic_parameters ->index_token += 1;

		status = get_not (syntactic_parameters, &not_node);
		if (status) {return status;}
		
		*ptr_node = keyword_node_((syntactic_parameters -> tokens -> array_of_tokens)[old_index_token].value.value_op, *ptr_node, not_node);
		if (*ptr_node == NULL) {error_massage_; free (not_node); return NOT_MEMORY_FOR_NEW_NODE;}
	}

	return NOT_ERROR;
}

static front_end_error_t get_not (syntactic_parameters_t* syntactic_parameters, node_t** ptr_node)
{
	ASSERTS_IN_RECURSIVE_DESCENT_ 

	front_end_error_t status = NOT_ERROR;

	if (check_token_type_(0, OP) && check_op_value_(0, NOT))
	{
		status = get_or (syntactic_parameters, ptr_node);
		if (status) {return status;}

		*ptr_node = keyword_node_(NOT, NULL, *ptr_node);
		if (*ptr_node == NULL) {error_massage_; return NOT_MEMORY_FOR_NEW_NODE;}

		return NOT_ERROR;
	}

	return get_expression (syntactic_parameters, ptr_node);
}
