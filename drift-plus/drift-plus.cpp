#include "pch.h"

#include "blackcipher.h"
#include "util.h"

int main()
{
	plog::init<plog::MessageOnlyFormatter>(plog::debug, plog::streamStdOut);

	blackcipher bc;
	system("pause");
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
}