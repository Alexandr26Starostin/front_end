#include <stdio.h>

#include "const_in_front_end.h"
#include "launch_front_end.h"

int main (int argc, char** argv)
{
	printf ("Привет!\n");
	
	return launch_front_end (argc, argv);
}