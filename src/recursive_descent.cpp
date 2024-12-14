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

static front_end_error_t get_grammar                   (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_root_node, long* ptr_scope);  //
static front_end_error_t get_operation                 (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       // 
static front_end_error_t get_assign                    (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
static front_end_error_t get_var_declaration           (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
// static front_end_error_t get_if                        (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_while                     (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_printf                    (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_scanf                     (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_interruption              (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_empty_operation           (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_definition_func           (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_return                    (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_global_variable           (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_logical                   (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
// static front_end_error_t get_parameters_for_definition (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);
static front_end_error_t get_expression                (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
static front_end_error_t get_term                      (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
static front_end_error_t get_power                     (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       // 
static front_end_error_t get_base_func                 (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
static front_end_error_t get_round                     (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
static front_end_error_t get_element                   (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
static front_end_error_t get_constant                  (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
static front_end_error_t get_variable                  (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);       //
// static front_end_error_t get_call_func                 (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope);

#define constant_node_(number)                                      create_node (CONSTANT,            number,                       NULL, NULL,  NULL)   
#define identifier_node_(index_in_name_table)                       create_node (IDENTIFIER,          (double) index_in_name_table, NULL, NULL,  NULL)
#define keyword_node_(code_keyword, left, right)                    create_node (KEYWORD,             (double) code_keyword,        left, right, NULL)
#define function_definition_node_(index_in_name_table, left, right) create_node (FUNCTION_DEFINITION, (double) index_in_name_table, left, right, NULL)
#define parameters_node_(left, right)                               create_node (PARAMETERS,          0,                            left, right, NULL)
#define var_declaration_node_(index_in_name_table, left, right)     create_node (VAR_DECLARATION,     (double) index_in_name_table, left, right, NULL)
#define call_node_(left, right)                                     create_node (CALL,                0,                            left, right, NULL)

#define try_perform_operation_(name_func, name_skip_command)                                  \
	*ptr_index_token = old_index;                                                             \
																							  \
	status = name_func (tokens, ptr_index_token, &operation_node, ptr_scope);                 \
	if (status != name_skip_command)                                                          \
	{                                                                                         \
		if (status) {return status;}                                                          \
                                                            								  \
		(*ptr_node)    -> left             = operation_node;                                  \
		operation_node -> parent           = *ptr_node;                                       \
		(*ptr_node)    -> right            = keyword_node_(OPERATOR, NULL, NULL);             \
		((*ptr_node)   -> right) -> parent = (*ptr_node);                                     \
		*ptr_node                          = (*ptr_node) -> right;                            \
																							  \
		return status;                                                                        \
	}    

#define READ_BASE_FUNC_IN_GET_BASE_FUNC_(name_base_func)                                  \
	if ((tokens -> array_of_tokens)[*ptr_index_token].value.value_op == name_base_func)   \
	{                             													      \
		(*ptr_index_token) += 1;													      \
																					      \
		status = get_round (tokens, ptr_index_token, &round_node, ptr_scope);             \
		if (status) {return status;}                            					      \
																					      \
		*ptr_node = keyword_node_(name_base_func, NULL, round_node);                      \
																				 	      \
		return NOT_ERROR;                                                                 \
	}

//--------------------------------------------------------------------------------

front_end_error_t recursive_descent (array_of_tokens_t* tokens, node_t* root_node)
{
	assert (tokens);
	assert (root_node);

	size_t index_token = 0;
	long   scope       = 0;

	front_end_error_t status = get_grammar (tokens, &index_token, &root_node, &scope);

	return status;
}

static front_end_error_t get_grammar (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_root_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_root_node);
	assert (ptr_scope);

	front_end_error_t status = NOT_ERROR;

	node_t* grammar_node = *ptr_root_node;

	while (status == NOT_ERROR && *ptr_index_token != tokens -> index_free) {status = get_operation (tokens, ptr_index_token, &grammar_node, ptr_scope);}

	if (status) {return status;}

	if (*ptr_index_token != tokens -> index_free)
	{
		printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
		printf ("Error from 'get_grammar': wait position: %ld, "
		        "but get position: '%ld'\n\n", 
				tokens -> index_free, *ptr_index_token);

		return ERROR_IN_GET_GRAMMAR;
	}

	(*ptr_index_token)++;

	return status;
}                

static front_end_error_t get_operation (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	front_end_error_t status         = NOT_ERROR;
	size_t            old_index      = *ptr_index_token;
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
	// try_perform_operation_(get_global_variable, SKIP_GET_GLOBAL_VARIABLE)

	printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
	printf ("Error from 'get_operation': in position: %ld operation not definite\n",
			*ptr_index_token);

	return ERROR_IN_GET_OPERATION;
}

static front_end_error_t get_assign (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	node_t*           expression_node = NULL;
	node_t*           variable_node   = NULL;
	front_end_error_t status          = NOT_ERROR;

	if ((tokens -> array_of_tokens)[*ptr_index_token].type == OP && (tokens -> array_of_tokens)[*ptr_index_token].value.value_id       == INT &&
	    (tokens -> array_of_tokens)[*ptr_index_token].type == OP && (tokens -> array_of_tokens)[(*ptr_index_token) + 2].value.value_id != ROUND_BEGIN)  //double <var> ==  
	{
		status = get_var_declaration (tokens, ptr_index_token, ptr_node, ptr_scope);
		if (status) {return status;}

		return NOT_ERROR;
	}
  
	if ((tokens -> array_of_tokens)[*ptr_index_token].type == ID)
	{
		size_t index_id_in_name_table = (tokens -> array_of_tokens)[*ptr_index_token].value.value_id;

		if ((tokens -> name_table)[index_id_in_name_table].status == NOT_DEFINITE)
		{
			printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
			printf ("Error from 'get_assign': not have declaration of var (index_id in name_table == %ld) in position in tokens == %ld\n\n", 
			        index_id_in_name_table, *ptr_index_token);

			return ERROR_IN_GET_ASSIGN;
		}

		if ((tokens -> array_of_tokens)[(*ptr_index_token) + 1].type == OP && (tokens -> array_of_tokens)[(*ptr_index_token) + 1].value.value_id == ASSIGN)
		{
			status = get_variable (tokens, ptr_index_token, &variable_node, ptr_scope); 
			if (status) {return status;}

			(*ptr_index_token) += 1;   //пропуск '=' <==> 'ИЛ'

			status = get_expression (tokens, ptr_index_token, &expression_node, ptr_scope); 
			if (status) {return status;}

			if ((tokens -> array_of_tokens)[*ptr_index_token].type != OP || (tokens -> array_of_tokens)[*ptr_index_token].value.value_op != OPERATOR)
			{
				printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
				printf ("Error from 'get_var_declaration': in position in token == %ld wait OPERATION ('\n'), but find: %ld", 
				*ptr_index_token, (tokens -> array_of_tokens)[*ptr_index_token].value.value_id);

				return ERROR_IN_GET_ASSIGN;
			}

			(*ptr_index_token) += 1;  //пропуск ';' <==> '\n'

			*ptr_node = keyword_node_(ASSIGN, expression_node, variable_node);

			return NOT_ERROR;
		}

		else
		{
			printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
			printf ("Error from 'get_assign': in position in token == %ld wait ASSIGN ('ИЛ'), but find: %ld", 
			(*ptr_index_token) + 1, (tokens -> array_of_tokens)[(*ptr_index_token) + 1].value.value_id);

			return ERROR_IN_GET_ASSIGN;
		}
	}

	return SKIP_GET_ASSIGN;
}

static front_end_error_t get_var_declaration (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	front_end_error_t status   = NOT_ERROR;
	node_t*           var_node = NULL;

	if ((tokens -> array_of_tokens)[(*ptr_index_token) + 1].type == ID)
	{
		size_t index_id_in_name_table = (tokens -> array_of_tokens)[(*ptr_index_token) + 1].value.value_id;

		if ((tokens -> name_table)[index_id_in_name_table].status)
		{
			printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
			printf ("Error from 'get_var_declaration': repeated declaration of var (index_id in name_table == %ld) in position in tokens == %ld\n\n", 
			        index_id_in_name_table, (*ptr_index_token) + 1);

			return ERROR_IN_GET_VAR_DECLARATION;
		}

		(tokens -> name_table)[index_id_in_name_table].status = DEFINITE;
		(tokens -> name_table)[index_id_in_name_table].type   = NAME_VAR;

		if ((tokens -> array_of_tokens)[(*ptr_index_token) + 2].type == OP && (tokens -> array_of_tokens)[(*ptr_index_token) + 2].value.value_id == ASSIGN)
		{
			(*ptr_index_token) += 1;

			status = get_assign (tokens, ptr_index_token, &var_node, ptr_scope); 
			if (status) {return status;}

			*ptr_node = var_declaration_node_(index_id_in_name_table, keyword_node_(INT, NULL, NULL), var_node);

			return NOT_ERROR;
		}

		else if ((tokens -> array_of_tokens)[(*ptr_index_token) + 2].type == OP && (tokens -> array_of_tokens)[(*ptr_index_token) + 2].value.value_id == OPERATOR)
		{
			(*ptr_index_token) += 1;

			status = get_variable (tokens, ptr_index_token, &var_node, ptr_scope);
			if (status) {return status;}

			(*ptr_index_token) += 1;

			*ptr_node = var_declaration_node_(index_id_in_name_table, keyword_node_(INT, NULL, NULL), var_node);

			return NOT_ERROR;
		}

		else
		{
			printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
			printf ("Error from 'get_var_declaration': in position in token == %ld wait ASSIGN ('ИЛ') or OPERATOR ('\n'), but find: %ld", 
			(*ptr_index_token) + 2, (tokens -> array_of_tokens)[(*ptr_index_token) + 2].value.value_id);

			return ERROR_IN_GET_VAR_DECLARATION;
		}
	}

	else
	{
		printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
		printf ("Error from 'get_var_declaration': in position in tokens == %ld not find name_variable\n\n", 
		        (*ptr_index_token) + 1);

		return ERROR_IN_GET_VAR_DECLARATION;
	}
}

static front_end_error_t get_expression (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	front_end_error_t status          = NOT_ERROR;
	node_t*           term_node       = NULL;
	size_t            old_index_token = 0;

	status = get_term (tokens, ptr_index_token, ptr_node, ptr_scope);
	if (status) {return status;}

	while ((tokens -> array_of_tokens)[*ptr_index_token].type == OP &&
		   (((tokens -> array_of_tokens)[*ptr_index_token].value.value_op == ADD ||
		     (tokens -> array_of_tokens)[*ptr_index_token].value.value_op == SUB)))
	{
		old_index_token = *ptr_index_token;

		(*ptr_index_token) += 1;

		status = get_term (tokens, ptr_index_token, &term_node, ptr_scope);
		if (status) {return status;}

		if ((tokens -> array_of_tokens)[old_index_token].value.value_op == ADD)
		{
			*ptr_node = keyword_node_(ADD, *ptr_node, term_node);
		}

		else  //(tokens -> array_of_tokens)[old_index_token].value.value_op == SUB
		{
			*ptr_node = keyword_node_(SUB, *ptr_node, term_node);
		}
	}

	return NOT_ERROR;
}

static front_end_error_t get_term (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	front_end_error_t status          = NOT_ERROR;
	node_t*           power_node       = NULL;
	size_t            old_index_token = 0;

	status = get_power (tokens, ptr_index_token, ptr_node, ptr_scope);
	if (status) {return status;}

	while ((tokens -> array_of_tokens)[*ptr_index_token].type == OP &&
		   (((tokens -> array_of_tokens)[*ptr_index_token].value.value_op == MUL ||
		     (tokens -> array_of_tokens)[*ptr_index_token].value.value_op == DIV)))
	{
		old_index_token = *ptr_index_token;

		(*ptr_index_token) += 1;

		status = get_term (tokens, ptr_index_token, &power_node, ptr_scope);
		if (status) {return status;}

		if ((tokens -> array_of_tokens)[old_index_token].value.value_op == MUL)
		{
			*ptr_node = keyword_node_(ADD, *ptr_node, power_node);
		}

		else  //(tokens -> array_of_tokens)[old_index_token].value.value_op == DIV
		{
			*ptr_node = keyword_node_(SUB, *ptr_node, power_node);
		}
	}

	return NOT_ERROR;
}

static front_end_error_t get_power (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	front_end_error_t status         = NOT_ERROR;
	node_t*           base_func_node = NULL;

	status = get_base_func (tokens, ptr_index_token, ptr_node, ptr_scope);
	if (status) {return status;}

	while ((tokens -> array_of_tokens)[*ptr_index_token].type           == OP &&
		   (tokens -> array_of_tokens)[*ptr_index_token].value.value_op == DEG)
	{
		(*ptr_index_token) += 1;

		status = get_base_func (tokens, ptr_index_token, &base_func_node, ptr_scope);
		if (status) {return status;}

		*ptr_node = keyword_node_(DEG, *ptr_node, base_func_node);
	}

	return NOT_ERROR;
}

static front_end_error_t get_base_func (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	front_end_error_t status     = NOT_ERROR;
	node_t*           round_node = NULL;

	if ((tokens -> array_of_tokens)[*ptr_index_token].type == OP)
	{
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(SIN)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(COS)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(SH)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(CH)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(SQRT)
		READ_BASE_FUNC_IN_GET_BASE_FUNC_(LN)
		//READ_BASE_FUNC_IN_GET_BASE_FUNC_(DIFF)
		//READ_BASE_FUNC_IN_GET_BASE_FUNC_(FLOOR)

		if ((tokens -> array_of_tokens)[*ptr_index_token].value.value_op == LOG)   
		{                             													      
			(*ptr_index_token) += 1;													      
																							
			status = get_round (tokens, ptr_index_token, ptr_node, ptr_scope);             
			if (status) {return status;}  

			status = get_round (tokens, ptr_index_token, &round_node, ptr_scope);             
			if (status) {return status;}                           					      
																							
			*ptr_node = keyword_node_(LOG, *ptr_node, round_node);                      
																							
			return NOT_ERROR;                                                                 
		}
	}

	status = get_round (tokens, ptr_index_token, ptr_node, ptr_scope);    
	return status;
}

static front_end_error_t get_round (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	front_end_error_t status = NOT_ERROR;

	if ((tokens -> array_of_tokens)[*ptr_index_token].type           == OP &&
		(tokens -> array_of_tokens)[*ptr_index_token].value.value_op == ROUND_BEGIN)
	{
		(*ptr_index_token) += 1;

		status = get_expression (tokens, ptr_index_token, ptr_node, ptr_scope);    
		if (status) {return status;}

		if ((tokens -> array_of_tokens)[*ptr_index_token].type           != OP ||
			(tokens -> array_of_tokens)[*ptr_index_token].value.value_op != ROUND_END)
		{
			printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
			printf ("Error from 'get_round': in position in tokens == %ld wait ROUND_END ('лети'), but not find it\n\n", *ptr_index_token);

			return ERROR_IN_GET_ROUND;
		}

		(*ptr_index_token) += 1;

		return NOT_ERROR;
	}

	status = get_element (tokens, ptr_index_token, ptr_node, ptr_scope);
	return status;
}

static front_end_error_t get_element (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	front_end_error_t status = NOT_ERROR;

	if ((tokens -> array_of_tokens)[*ptr_index_token].type == NUM && (tokens -> array_of_tokens)[(*ptr_index_token) + 1].type == OP)
	{
		status = get_constant (tokens, ptr_index_token, ptr_node, ptr_scope);    
		return status;
	}

	if ((tokens -> array_of_tokens)[*ptr_index_token].type == ID && 
		(tokens -> array_of_tokens)[(*ptr_index_token) + 1].type == OP && 
		(tokens -> array_of_tokens)[(*ptr_index_token) + 1].value.value_op != ROUND_BEGIN)
	{
		//status = get_call_func (tokens, ptr_index_token, ptr_node, ptr_scope);    
		return status;
	}

	if ((tokens -> array_of_tokens)[*ptr_index_token].type == ID &&
		(tokens -> array_of_tokens)[(*ptr_index_token) + 1].type == OP)
	{
		status = get_variable (tokens, ptr_index_token, ptr_node, ptr_scope);    
		return status;
	}

	printf ("Error in %s:%d\n\n", __FILE__, __LINE__);
	printf ("Error from 'get_round': in position in tokens == %ld wait OP after NUM or ID, but find ID\n\n", (*ptr_index_token) + 1);

	return ERROR_IN_GET_ELEMENT;
}

static front_end_error_t get_constant (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	*ptr_node           = constant_node_((tokens -> array_of_tokens)[*ptr_index_token].value.value_num);
	(*ptr_index_token) += 1;

	return NOT_ERROR;
}

static front_end_error_t get_variable (array_of_tokens_t* tokens, size_t* ptr_index_token, node_t** ptr_node, long* ptr_scope)
{
	assert (tokens);
	assert (ptr_index_token);
	assert (ptr_node);
	assert (ptr_scope);

	*ptr_node           = identifier_node_((tokens -> array_of_tokens)[*ptr_index_token].value.value_id);
	(*ptr_index_token) += 1;

	return NOT_ERROR;
}