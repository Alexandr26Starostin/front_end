#include <stdio.h>

#include "const_in_front_end.h"
#include "list_of_func.h"
#include "launch_front_end.h"

front_end_error_t launch_front_end ()
{
	list_of_func_t list_of_func = {};

	front_end_error_t status = create_list_of_func (&list_of_func);
	if (status) {return status;}
	
	dump_list_of_func (&list_of_func);

	delete_list_of_func (&list_of_func);

	return status;
}


