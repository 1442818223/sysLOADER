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
	WCHAR fullPath[512]; // ����·�����ᳬ��512���ַ�

	PKLDR_DATA_TABLE_ENTRY ldr = (PKLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
	// �������һ����б��
	WCHAR* lastBackslash = wcsrchr(ldr->FullDllName.Buffer, L'\\');
	if (lastBackslash)
	{
		// �����һ����б���滻Ϊ�ַ���������
		*lastBackslash = L'\0'; // �ض��ַ���

		// ��ӡ·������
		//DbgPrintEx(77, 0, "Driver Path: %ws\n", ldr->FullDllName.Buffer);
		// ƴ��·�����ļ���
		
		NTSTATUS status = RtlStringCchPrintfW(fullPath, sizeof(fullPath) / sizeof(WCHAR), L"%ws\\Other.sys", ldr->FullDllName.Buffer);

		if (NT_SUCCESS(status))
		{
			// ��ӡƴ�Ӻ��·��
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

	
	// �����ڴ��Դ洢�ļ�����
	unsigned char* pMemory = (unsigned char*)ExAllocatePool(NonPagedPool, x);
	if (!pMemory) {
		//DbgPrintEx(77, 0, "Memory allocation failed for size: %lu\n", x);
		return STATUS_INSUFFICIENT_RESOURCES; // �����ڴ�������
	}

	// ��ȡ�ļ�����
	NTSTATUS status = ReadAllFile(fullPath, x, pMemory);
	if (!NT_SUCCESS(status)) {
		//DbgPrintEx(77, 0, "Failed to read file, status: 0x%X\n", status);
		ExFreePool(pMemory); // �ͷ��ڴ�
		return status; // ���ش���״̬
	}

	// ʹ�� pMemory �е����ݽ�����������
	LoadDriver(pMemory, pDriver, pReg); // ���д����豸
	ExFreePool(pMemory); // ʹ������ͷ��ڴ�



	pDriver->DriverUnload = DriverUnload;
	return 0;
}