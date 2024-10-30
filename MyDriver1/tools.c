#include "tools.h"
#include <ntddk.h> // �����������ں�ģʽ��������Ķ���
#include <ntstrsafe.h> // �����ַ�����ȫ�����Ķ���



ULONG_PTR QueryModule(PUCHAR moduleName, ULONG_PTR * moduleSize)
{
	if (moduleName == NULL) return 0;

	RTL_PROCESS_MODULES rtlMoudles = { 0 };
	PRTL_PROCESS_MODULES SystemMoudles = &rtlMoudles;
	BOOLEAN isAllocate = FALSE;
	//��������
	ULONG * retLen = 0;
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, SystemMoudles, sizeof(RTL_PROCESS_MODULES), &retLen);

	//����ʵ�ʳ����ڴ�
	if (status == STATUS_INFO_LENGTH_MISMATCH)
	{
		SystemMoudles = ExAllocatePool(PagedPool, retLen + sizeof(RTL_PROCESS_MODULES));
		if (!SystemMoudles) return 0;

		memset(SystemMoudles, 0, retLen + sizeof(RTL_PROCESS_MODULES));

		status = ZwQuerySystemInformation(SystemModuleInformation, SystemMoudles, retLen + sizeof(RTL_PROCESS_MODULES), &retLen);
	
		if (!NT_SUCCESS(status))
		{
			ExFreePool(SystemMoudles);
			return 0;
		}

		isAllocate = TRUE;
	}

	PUCHAR kernelModuleName = NULL;
	ULONG_PTR moudleBase = 0;

	do 
	{
	
		
		if (_stricmp(moduleName, "ntoskrnl.exe") == 0 || _stricmp(moduleName, "ntkrnlpa.exe") == 0)
		{
			PRTL_PROCESS_MODULE_INFORMATION moudleInfo = &SystemMoudles->Modules[0];
			moudleBase = moudleInfo->ImageBase;
			if (moduleSize) *moduleSize = moudleInfo->ImageSize;
			
			break;
		}


		kernelModuleName = ExAllocatePool(PagedPool, strlen(moduleName) + 1);
		memset(kernelModuleName, 0, strlen(moduleName) + 1);
		memcpy(kernelModuleName, moduleName, strlen(moduleName));
		_strupr(kernelModuleName);


		for (int i = 0; i < SystemMoudles->NumberOfModules; i++)
		{
			PRTL_PROCESS_MODULE_INFORMATION moudleInfo = &SystemMoudles->Modules[i];
		
			PUCHAR pathName = _strupr(moudleInfo->FullPathName);
			//DbgPrintEx(77, 0, "baseName = %s,fullPath = %s\r\n", 
			//	moudleInfo->FullPathName + moudleInfo->OffsetToFileName, moudleInfo->FullPathName);

			
			if (strstr(pathName, kernelModuleName))
			{
				moudleBase = moudleInfo->ImageBase;
				if (moduleSize) *moduleSize = moudleInfo->ImageSize;
				break;
			}

		}

	} while (0);
	

	if (kernelModuleName)
	{
		ExFreePool(kernelModuleName);
	}

	if (isAllocate)
	{
		ExFreePool(SystemMoudles);
	}

	return moudleBase;
}

NTSTATUS DeleteRegisterPath(PUNICODE_STRING pReg)
{ //���ڵ���һ��һ���ɾ��
	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pReg->Buffer, L"DisplayName"); //RTL_REGISTRY_ABSOLUTE�Ǿ��Ե�ע���·����
	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pReg->Buffer, L"ErrorControl");
	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pReg->Buffer, L"ImagePath");
	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pReg->Buffer, L"Start");
	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pReg->Buffer, L"Type");
	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, pReg->Buffer, L"WOW64");

	//ƴװ�ַ���
	PWCH enumPath = (PWCH)ExAllocatePool(PagedPool, pReg->MaximumLength + 0X100);
	memset(enumPath, 0, pReg->MaximumLength + 0X100);
	memcpy(enumPath, pReg->Buffer, pReg->Length);
	wcscat(enumPath, L"\\Enum");

	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, enumPath, L"enumPath");
	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, enumPath, L"INITSTARTFAILED");
	RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, enumPath, L"NextInstance");

	HANDLE hKeyEnum = NULL;
	OBJECT_ATTRIBUTES enumObj = { 0 };
	UNICODE_STRING unEnumName;
	RtlInitUnicodeString(&unEnumName, enumPath);
	InitializeObjectAttributes(&enumObj, &unEnumName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	NTSTATUS status = ZwOpenKey(&hKeyEnum, KEY_ALL_ACCESS, &enumObj);

	if (NT_SUCCESS(status))
	{
		ZwDeleteKey(hKeyEnum);
		ZwClose(hKeyEnum);
	}

	ExFreePool(enumPath);

	//ɾ������

	HANDLE hKeyRoot = NULL;
	OBJECT_ATTRIBUTES rootObj = { 0 };

	InitializeObjectAttributes(&rootObj, pReg, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwOpenKey(&hKeyRoot, KEY_ALL_ACCESS, &rootObj);

	if (NT_SUCCESS(status))
	{
		ZwDeleteKey(hKeyRoot);
		ZwClose(hKeyRoot);
		return STATUS_SUCCESS;
	}

	return STATUS_UNSUCCESSFUL;
}


NTSTATUS GetFileSize(PWCH path, ULONG* fileSize)
{
	OBJECT_ATTRIBUTES objFile = { 0 };
	HANDLE hFile = NULL;
	IO_STATUS_BLOCK ioBlock = { 0 };
	UNICODE_STRING unFileName = { 0 };
	NTSTATUS status;

	RtlInitUnicodeString(&unFileName, path);
	InitializeObjectAttributes(&objFile, &unFileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	// �����ļ�
	status = ZwCreateFile(
		&hFile,
		GENERIC_READ,
		&objFile,
		&ioBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FILE_NON_DIRECTORY_FILE,
		NULL,
		0
	);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	// ʹ��ZwQueryInformationFile��ȡ�ļ���Ϣ
	FILE_STANDARD_INFORMATION fileInfo;
	status = ZwQueryInformationFile(
		hFile,
		&ioBlock,
		&fileInfo,
		sizeof(fileInfo),
		FileStandardInformation
	);

	if (NT_SUCCESS(status))
	{
		*fileSize = fileInfo.EndOfFile.LowPart; // ��ȡ�ļ���С
	}
	else
	{
		*fileSize = 0; // δ�ܻ�ȡ�ļ���С
	}

	ZwClose(hFile); // �ر��ļ����

	return status;
}



NTSTATUS ReadAllFile(PCWSTR filePath, ULONG actualSize, PVOID returnAddress) {
	HANDLE fileHandle;
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatusBlock;
	UNICODE_STRING unFileName;
	OBJECT_ATTRIBUTES objFile;
	PVOID buffer = NULL;
	ULONG bytesRead;

	// ��ӡ������ļ�·��
	//DbgPrintEx(77, 0, "ReadAllFile called with path: %ws\n", filePath);

	// ��� returnAddress �Ƿ���Ч
	if (!returnAddress || actualSize == 0) {
		//DbgPrintEx(77, 0, "Invalid parameters: returnAddress or actualSize is not valid.\n");
		return STATUS_INVALID_PARAMETER;
	}

	// ��ʼ���ļ���
	RtlInitUnicodeString(&unFileName, filePath);

	// ��ʼ����������
	InitializeObjectAttributes(&objFile, &unFileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	// ���ļ�
	status = ZwOpenFile(
		&fileHandle,
		GENERIC_READ,
		&objFile,
		&ioStatusBlock,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_NON_DIRECTORY_FILE
	);

	if (!NT_SUCCESS(status)) {
		//DbgPrintEx(77, 0, "Failed to open file: %ws, status: 0x%X\n", filePath, status);
		return status; // �������
	}

	// �����ڴ��Դ洢�ļ�����
	buffer = ExAllocatePool(NonPagedPool, actualSize);
	if (!buffer) {
		ZwClose(fileHandle);
		//DbgPrintEx(77, 0, "Memory allocation failed for size: %lu\n", actualSize);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// ��ȡ�ļ�����
	//DbgPrintEx(77, 0, "Attempting to read file: %ws, size: %lu\n", filePath, actualSize);

	LARGE_INTEGER byteOffset;
	byteOffset.QuadPart = 0;

	// ��ȡ�ļ�����
	status = ZwReadFile(
		fileHandle,
		NULL,
		NULL,
		NULL,
		&ioStatusBlock,
		buffer,
		actualSize,
		&byteOffset,//��ѡ������ָ�����ļ��е��ĸ�λ�ÿ�ʼ��ȡ���ݡ�
		NULL
	);

	// ��ȡ�ɹ�
	if (NT_SUCCESS(status)) {
		bytesRead = (ULONG)ioStatusBlock.Information;
		// ���ƶ�ȡ������
		RtlCopyMemory(returnAddress, buffer, bytesRead);
		//DbgPrintEx(77, 0, "Successfully read %lu bytes from file: %ws\n", bytesRead, filePath);
	}
	else {
		//DbgPrintEx(77, 0, "Failed to read file: %ws, status: 0x%X\n", filePath, status);
		ExFreePool(buffer);
		ZwClose(fileHandle); // ȷ���ļ�������ر�
		return status; // ���ض�ȡ�����״̬
	}

	// �ر��ļ�
	ZwClose(fileHandle);
	ExFreePool(buffer); // �ͷ��ڴ�
	return STATUS_SUCCESS; // ���سɹ�״̬
}









