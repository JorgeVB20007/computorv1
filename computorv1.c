#include <unistd.h>
#include <stdio.h>


int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(2, "Please give only one argument.\n", 31);
		return (1);
	}


}

