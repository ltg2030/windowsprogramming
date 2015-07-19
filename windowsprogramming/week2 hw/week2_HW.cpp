// week2_HW.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "mmio_plus.h"


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

	free(buf); buf = NULL;

	FileIoHelper MMIOFILE;
	FileIoHelper MMIOFILE2;
	LARGE_INTEGER file_size;
	LARGE_INTEGER offset;
	PUCHAR buff;

	offset.QuadPart = 0;

	if (is_file_existsW(file_name))
	{
		MMIOFILE.FIOpenForRead(file_name);
		file_size = *MMIOFILE.FileSize();
	}
	else
	{
		file_size.QuadPart = (unsigned long long)1 * 501 * 1024 * 1024;
		MMIOFILE.FIOCreateFile(file_name, file_size);
	}

	MMIOFILE2.FIOCreateFile(file_name2, file_size);
	// 복사의 개념과는 약간 다른 것 같지만 복사를 하기 위해 먼저 같은 크기의 파일을 생성한 다음 내부 데이터를 덮어씌우는 형태로 구현하였다.


	// copy
	buff = (PUCHAR) malloc((500 * 1024 * 1024 + 1)*sizeof(UCHAR));
	while (1)
	{
		if (offset.QuadPart + 500 * 1024 * 1024 < file_size.QuadPart)
		{
			MMIOFILE.FIOReadFromFile(offset, 500 * 1024 * 1024, buff);
			MMIOFILE2.FIOWriteToFile(offset, 500 * 1024 * 1024, buff);
			offset.QuadPart += 500 * 1024 * 1024;
		}
		else
		{
			MMIOFILE.FIOReadFromFile(offset, file_size.QuadPart - offset.QuadPart, buff);
			MMIOFILE2.FIOWriteToFile(offset, file_size.QuadPart - offset.QuadPart, buff);
			offset.QuadPart = file_size.QuadPart;
			break;
		}
	}

	return 0;
}

