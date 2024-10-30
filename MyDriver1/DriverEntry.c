#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include "tools.h"
#include "Loader.h"



typedef struct _KLDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	PVOID ExceptionTable;
	ULONG ExceptionTableSize;
	// ULONG padding on IA64
	PVOID GpValue;
	PVOID NonPagedDebugInfo;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT __Unused5;
	PVOID SectionPointer;
	ULONG CheckSum;
	// ULONG padding on IA64
	PVOID LoadedImports;
	PVOID PatchInformation;
} KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	WCHAR fullPath[512]; // 假设路径不会超过512个字符

	PKLDR_DATA_TABLE_ENTRY ldr = (PKLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
	// 查找最后一个反斜杠
	WCHAR* lastBackslash = wcsrchr(ldr->FullDllName.Buffer, L'\\');
	if (lastBackslash)
	{
		// 将最后一个反斜杠替换为字符串结束符
		*lastBackslash = L'\0'; // 截断字符串

		// 打印路径部分
		//DbgPrintEx(77, 0, "Driver Path: %ws\n", ldr->FullDllName.Buffer);
		// 拼接路径和文件名
		
		NTSTATUS status = RtlStringCchPrintfW(fullPath, sizeof(fullPath) / sizeof(WCHAR), L"%ws\\Other.sys", ldr->FullDllName.Buffer);

		if (NT_SUCCESS(status))
		{
			// 打印拼接后的路径
			//DbgPrintEx(77, 0, "Full Path to Other.sys: %ws\n", fullPath);
		}
		else
		{
			//DbgPrintEx(77, 0, "Failed to format string, status: 0x%X\n", status);
		}

	}

	ULONG x = 0;
	NTSTATUS result = GetFileSize(fullPath,&x);
	if (NT_SUCCESS(result))
	{
		//DbgPrintEx(77, 0, "File Size: %lu bytes\n", x);
	}
	else
	{
		//DbgPrintEx(77, 0, "Failed to get file size, status: 0x%X\n", x);
	}

	
	// 分配内存以存储文件内容
	unsigned char* pMemory = (unsigned char*)ExAllocatePool(NonPagedPool, x);
	if (!pMemory) {
		//DbgPrintEx(77, 0, "Memory allocation failed for size: %lu\n", x);
		return STATUS_INSUFFICIENT_RESOURCES; // 处理内存分配错误
	}

	// 读取文件内容
	NTSTATUS status = ReadAllFile(fullPath, x, pMemory);
	if (!NT_SUCCESS(status)) {
		//DbgPrintEx(77, 0, "Failed to read file, status: 0x%X\n", status);
		ExFreePool(pMemory); // 释放内存
		return status; // 返回错误状态
	}

	// 使用 pMemory 中的内容进行其他操作
	LoadDriver(pMemory, pDriver, pReg); // 自行传递设备
	ExFreePool(pMemory); // 使用完后释放内存



	pDriver->DriverUnload = DriverUnload;
	return 0;
}