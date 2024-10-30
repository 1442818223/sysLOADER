#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{

	DbgPrintEx(77, 0, "我被成功加载上了\n");


	pDriver->DriverUnload = DriverUnload;
	return STATUS_UNSUCCESSFUL;
}