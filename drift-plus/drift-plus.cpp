#include "pch.h"

#include "blackcipher.h"
#include "util.h"
#include "mainform.h"
#include "engine.h"

int main()
{
	plog::init<plog::MessageOnlyFormatter>(plog::info, plog::streamStdOut);

	{
		blackcipher bc;
		if (!bc)
		{
			PLOGI << xorstr_(L"NGS 초기화에 실패하였습니다");
			system("pause");
			return -1;
		}
	}

	engine eng;

	mainform fm{ &eng };
	fm.show();
	nana::exec();

	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
}