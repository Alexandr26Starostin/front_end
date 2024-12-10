#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "const_in_front_end.h"
#include "list_of_func.h"

#define FILL_LIST_OF_FUNC_(code_func, name_func)                                                 \
	case code_func:                                                                              \
	{                                                                                            \
		(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = name_func;        \
		break;                                                                                   \
	} 

static front_end_error_t fill_list_of_func    (list_of_func_t* list_of_func);
static front_end_error_t realloc_list_of_func (list_of_func_t* list_of_func);

front_end_error_t create_list_of_func (list_of_func_t* list_of_func)
{
	assert (list_of_func);

	list_of_func -> free_index   = 0;
	list_of_func -> size_of_list = SIZE_LIST_OF_FUNC;

	list_of_func -> list_of_func = (name_base_func_t*) calloc (SIZE_LIST_OF_FUNC, sizeof (name_base_func_t));

	if (list_of_func -> list_of_func == NULL)
	{
		printf ("\nError in %s:%d\nNot memory for list_of_func\n\n", __FILE__, __LINE__);
		return NOT_MEMORY_FOR_LIST_OF_FUNC;
	}

	front_end_error_t status = fill_list_of_func (list_of_func);

	return status;
}

front_end_error_t delete_list_of_func (list_of_func_t* list_of_func)
{
	assert (list_of_func);

	list_of_func -> free_index   = 0;
	list_of_func -> size_of_list = 0;

	free (list_of_func -> list_of_func);

	return NOT_ERROR;
}

static front_end_error_t fill_list_of_func (list_of_func_t* list_of_func)
{
	assert (list_of_func);

	front_end_error_t status = NOT_ERROR;

	while (true)
	{
		switch (list_of_func -> free_index)
		{
			case ADD:                                                                              
			{                      
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = ADD;                                                                      
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "дозаправить";        
				break;                                                                                   
			} 
			
			case SUB:                                                                              
			{                                  
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = SUB;                                                           
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "ударить";        
				break;                                                                                   
			} 

			case MUL:                                                                              
			{                    
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = MUL;                                                                         
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "штопор";        
				break;                                                                                   
			} 

			case DIV:                                                                              
			{           
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = DIV;                                                                                  
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "кобра";        
				break;                                                                                   
			} 

			case SIN:                                                                              
			{              
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = SIN;                                                                               
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "ангар";        
				break;                                                                                   
			} 

			case COS:                                                                              
			{ 
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = COS;                                                                                            
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "перрон";        
				break;                                                                                   
			} 

			case SH:                                                                              
			{  
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = SH;                                                                                           
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "гидрашка";        
				break;                                                                                   
			} 

			case CH:                                                                              
			{           
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = CH;                                                                                  
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "винт";        
				break;                                                                                   
			}

			case SQRT:                                                                              
			{      
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = SQRT;                                                                                       
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "петля";        
				break;                                                                                   
			} 

			case DEG:                                                                              
			{      
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = DEG;                                                                                       
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "тяга";        
				break;                                                                                   
			} 

			case LOG:                                                                              
			{        
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = LOG;                                                                                     
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "касалетка";        
				break;                                                                                   
			}

			case LN:                                                                              
			{                   
				(list_of_func -> list_of_func)[list_of_func -> free_index].code_func = LN;                                                                          
				(list_of_func -> list_of_func)[list_of_func -> free_index].name_func = "иллюминатор";        
				break;                                                                                   
			}

			default:    //В этом случае значение list_of_func -> free_index больше значения enum, =>, все базовые функции записаны.
			{
				return NOT_ERROR;
			}
		}

		list_of_func -> free_index += 1;

		if (list_of_func -> free_index >= SIZE_LIST_OF_FUNC)
		{
			status = realloc_list_of_func (list_of_func);
			if (status) {return status;}
		}
	}

	return NOT_ERROR;
}

static front_end_error_t realloc_list_of_func (list_of_func_t* list_of_func)
{
	assert (list_of_func);

	list_of_func -> size_of_list *= 2;

	list_of_func -> list_of_func = (name_base_func_t*) realloc (list_of_func -> list_of_func, list_of_func -> size_of_list);

	if (list_of_func -> list_of_func == NULL)
	{
		printf ("\nError in %s:%d\nNot memory for realloc of list_of_func\n\n", __FILE__, __LINE__);
		return NOT_MEMORY_FOR_REALLOC__LIST_OF_FUNC;
	}

	return NOT_ERROR;
}

front_end_error_t dump_list_of_func (list_of_func_t* list_of_func)
{
	assert (list_of_func);

	printf ("\n\n------------------------------------------------------------------\n\n");
	printf ("list_of_func:\nsize_of_list == %ld\nfree_index == %ld\n\n", list_of_func -> size_of_list, list_of_func -> free_index);

	printf ("index    code_func    name_func\n");

	for (size_t index = 0; index < list_of_func -> free_index; index++)
	{
		printf ("%5ld    %9d    %s\n", index, (list_of_func -> list_of_func)[index].code_func, 
		                                (list_of_func -> list_of_func)[index].name_func);
	}

	printf ("\n\n------------------------------------------------------------------\n\n");

	return NOT_ERROR;
}


