// Helloworld.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <Strsafe.h>
#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>

typedef long uint32_t;

bool is_file_existsW(_In_ const wchar_t* file_path)
{
	_ASSERTE(NULL != file_path);
	_ASSERTE(TRUE != IsBadStringPtrW(file_path, MAX_PATH));
	if ((NULL == file_path) || (TRUE == IsBadStringPtrW(file_path, MAX_PATH))) return false;

	WIN32_FILE_ATTRIBUTE_DATA info = { 0 };

	if (GetFileAttributesExW(file_path, GetFileExInfoStandard, &info) == 0)
		return false;
	else
		return true;
}

void print(_In_ const char* fmt, _In_ ...)
{
	char log_buffer[2048];
	va_list args;

	va_start(args, fmt);
	HRESULT hRes = StringCbVPrintfA(log_buffer, sizeof(log_buffer), fmt, args);
	if (S_OK != hRes)
	{
		fprintf(
			stderr,
			"%s, StringCbVPrintfA() failed. res = 0x%08x",
			__FUNCTION__,
			hRes
			);
		return;
	}

	OutputDebugStringA(log_buffer);
	fprintf(stdout, "%s \n", log_buffer);
}

/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
**/
bool create_bob_txt()
{
	// current directory �� ���Ѵ�.
	wchar_t *buf = NULL;
	uint32_t buflen = 0;
	buflen = GetCurrentDirectoryW(buflen, buf);
	if (0 == buflen)
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		return false;
	}

	buf = (PWSTR)malloc(sizeof(WCHAR) * buflen);
	if (0 == GetCurrentDirectoryW(buflen, buf))
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		free(buf);
		return false;
	}

	// current dir \\ bob.txt ���ϸ� ����
	wchar_t file_name[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name,
		sizeof(file_name),
		L"%ws\\bob.txt",
		buf)))
	{
		print("err, can not create file name");
		free(buf);
		return false;
	}
	// current dir \\ bob2.txt ���ϸ� ����
	wchar_t file_name2[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name2,
		sizeof(file_name2),
		L"%ws\\bob2.txt",
		buf)))
	{
		print("err, can not create file name");
		free(buf);
		return false;
	}


	free(buf); buf = NULL;


	if (true == is_file_existsW(file_name))
	{
		::DeleteFileW(file_name);
	}

	if (true == is_file_existsW(file_name2))
	{
		::DeleteFileW(file_name);
	}

	// ���� ����
	HANDLE file_handle = CreateFileW(
		file_name,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}

	// ���Ͽ� ������ ����
	DWORD bytes_written = 0;
	wchar_t string_buf[1024];
	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"bob ����Ʈ ���� �� ����Ʈ bob")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	if (!WriteFile(file_handle, string_buf, wcslen(string_buf)*2, &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	// ���� �ݱ�
	CloseHandle(file_handle);

	// ���� ����
	if (!CopyFile(file_name, file_name2 , false))
		printf("���� ����? \n");


	// ���� �б�
	HANDLE file_handle2 = CreateFileW(
		file_name2,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (file_handle2 == INVALID_HANDLE_VALUE)
	{
		print("%ws �� �о���� ���߽��ϴ�.", file_name2);
		return false;
	}
	DWORD bytes_read;
	wchar_t string_buf2[1024];
	memset(string_buf2, NULL, sizeof(string_buf2));

	if (!ReadFile(file_handle2, string_buf2, sizeof(string_buf2), &bytes_read, NULL))
	{
		printf("������ �о���� ���߽��ϴ�!");
		CloseHandle(file_handle2);
	}

	// wchar -> mbs ��ȯ
	int wcstrsize = wcslen(string_buf2) * 2 + 1;
	int mbstrsize = WideCharToMultiByte(CP_ACP, 0, string_buf2, -1, NULL, 0, NULL, NULL);
	char* mbstr;
	mbstr = new char[mbstrsize+1];
	WideCharToMultiByte(CP_ACP, 0, string_buf2, -1, mbstr, mbstrsize, 0, 0);

	printf("%ws\n-> %s\n\n", file_name2, mbstr);

	// mmio ����

	// ���� ����

	// mmio �� ���� filesize üũ
	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(file_handle2, &fileSize))
	{
		print("err, GetFileSizeEx(%ws) failed, gle = %u", file_name2, GetLastError());
		CloseHandle(file_handle2);
		return false;
	}

	// file size�� 4GB�� �Ѵ��� Ȯ�� 
	// ������ 4GB���� ũ�� Memory�� �÷� ȭ���ϴµ� ������ ���� �� ����.
	_ASSERTE(fileSize.HighPart == 0);
	if (fileSize.HighPart > 0)
	{
		print("file size = %I64d (over 4GB) can not handle. use FileIoHelperClass",
			fileSize.QuadPart);
		CloseHandle(file_handle2);
		return false;
	}

	DWORD file_size = (DWORD)fileSize.QuadPart;
	HANDLE file_map = CreateFileMapping(
		file_handle2,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL
		);

	if (NULL == file_map)
	{
		print("err, CreateFileMapping(%ws) failed, gle = %u", file_name2, GetLastError());
		CloseHandle(file_handle2);
		return false;
	}

	PCHAR file_view = (PCHAR)MapViewOfFile(
		file_map,
		FILE_MAP_READ,
		0,
		0,
		0
		);
	if (file_view == NULL)
	{
		print("err, MapViewOfFile(%ws) failed, gle = %u", file_name2, GetLastError());

		CloseHandle(file_map);
		CloseHandle(file_handle2);
		return false;
	}

	wchar_t mmio_get_string_buf[1024];
	memset(mmio_get_string_buf, NULL, sizeof(mmio_get_string_buf));
	int i;
	unsigned char b1, b2;
	for (i = 0; i < file_size/2; i++)
	{
		b1 = (unsigned char)file_view[2 * i];
		b2 = (unsigned char)file_view[2 * i+1];
		mmio_get_string_buf[i] = b2*256+b1;
	}

	// wchar -> mbs ��ȯ
	int mmio_wcstrsize = file_size + 1;
	int mmio_mbstrsize = WideCharToMultiByte(CP_ACP, 0, mmio_get_string_buf, -1, NULL, 0, NULL, NULL);
	char* mmio_mbstr;
	mmio_mbstr = new char[mmio_mbstrsize + 1];
	WideCharToMultiByte(CP_ACP, 0, mmio_get_string_buf, -1, mmio_mbstr, mmio_mbstrsize, 0, 0);

	printf("mmio : %ws\n-> %s\n\n", file_name2, mmio_mbstr);


	UnmapViewOfFile(file_view);
	CloseHandle(file_map);
	CloseHandle(file_handle2);

	if (!DeleteFile(file_name))
	{
		printf("%ws ������ ����µ� �����߽��ϴ� ", file_name);
	}

	if (!DeleteFile(file_name2))
	{
		printf("%ws ������ ����µ� �����߽��ϴ� ", file_name2);
	}


	return true;

}

int _tmain(int argc, _TCHAR* argv[])
{

	create_bob_txt();

	return 0;
}
