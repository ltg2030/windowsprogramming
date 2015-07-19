// Helloworld.cpp : Defines the entry point for the console application.
//
#define WIN32_LEAN_AND_MEAN

#include "stdafx.h"
#include "Stopwatch.h"
#include "ffio_mmio.h"



int _tmain(int argc, _TCHAR* argv[])
{
	// current directory 를 구한다.
	wchar_t *buf = NULL;
	uint32_t buflen = 0;
	buflen = GetCurrentDirectoryW(buflen, buf);
	if (0 == buflen)
	{
		print("err ] GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		return false;
	}

	buf = (PWSTR)malloc(sizeof(WCHAR) * buflen);
	if (0 == GetCurrentDirectoryW(buflen, buf))
	{
		print("err ] GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		free(buf);
		return false;
	}

	// current dir \\ test.txt 파일명 생성
	wchar_t file_name[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name,
		sizeof(file_name),
		L"%ws\\test.txt",
		buf)))
	{
		print("err ] can not create file name");
		free(buf);
		return false;
	}

	// current dir \\ test2.txt 파일명 생성
	wchar_t file_name2[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name2,
		sizeof(file_name2),
		L"%ws\\test2.txt",
		buf)))
	{
		print("err ] can not create file name");
		free(buf);
		return false;
	}

	// current dir \\ test3.txt 파일명 생성
	wchar_t file_name3[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name3,
		sizeof(file_name3),
		L"%ws\\test3.txt",
		buf)))
	{
		print("err ] can not create file name");
		free(buf);
		return false;
	}

	free(buf); buf = NULL;


	_ASSERTE(create_very_big_file(file_name, 400));

	StopWatch watch, watch2;
	watch.Start();
	_ASSERTE(file_copy_using_read_write(file_name, file_name2));
	watch.Stop();

	printf("read_write를 통한 파일 카피 %f (ms)\n", watch.GetDurationMilliSecond());


	watch2.Start();
	_ASSERTE(file_copy_using_memory_map(file_name, file_name3));
	watch2.Stop();

	printf("mmio를 통한 파일 카피 %f (ms)\n", watch2.GetDurationMilliSecond());

	/*
	-> 디버그 모드에서는 파일 입출력함수가 MMIO보다 빠르다.
	-> 릴리즈 모드에서는 MMIO가 파일 입출력함수보다 빠르다.
	--> 왜 이런 결과가 만들어질까?
	*/

	return 0;
}