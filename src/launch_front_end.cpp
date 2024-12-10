#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "const_in_front_end.h"
#include "list_of_func.h"
#include "operations_with_files.h"
#include "launch_front_end.h"

front_end_error_t launch_front_end (int argc, char** argv)
{
	assert (argv);

	list_of_func_t list_of_func = {};

	front_end_error_t status = create_list_of_func (&list_of_func);
	if (status) {return status;}
	
	dump_list_of_func (&list_of_func);

	char* str_with_program = NULL;

	status = read_program_file (argc, argv, &str_with_program);
	if (status) {return status;}

	printf ("%s\n", str_with_program);

	delete_list_of_func (&list_of_func);

	free (str_with_program);

	return status;
}
