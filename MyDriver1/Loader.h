#pragma once
#include <ntifs.h>

BOOLEAN UpdataIAT(char * imageBuffer);

BOOLEAN LoadDriver(PUCHAR fileBuffer, PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg);