// week2_HW.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "mmio_plus.h"


int _tmain(int argc, _TCHAR* argv[])
{

	// current directory �� ���Ѵ�.
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

	// current dir \\ test.txt ���ϸ� ����
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

	// current dir \\ test2.txt ���ϸ� ����
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
	// ������ ������� �ణ �ٸ� �� ������ ���縦 �ϱ� ���� ���� ���� ũ���� ������ ������ ���� ���� �����͸� ������ ���·� �����Ͽ���.


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

