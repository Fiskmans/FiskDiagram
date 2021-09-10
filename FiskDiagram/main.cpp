
#include "stdafx.h"
#include "Run.h"


int main(int argc, char** argv)
{
	if (argc < 2)
	{
		return Error();
	}
	else
	{
		return Run(argv[1]);
	}

	return EXIT_FAILURE;
}