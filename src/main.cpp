#include "Sandbox.h"

int main(int argc, char** argv)
{
	Sandbox* sb = new Sandbox();

	while (sb->loop());

	delete sb;	

	return 0;
};