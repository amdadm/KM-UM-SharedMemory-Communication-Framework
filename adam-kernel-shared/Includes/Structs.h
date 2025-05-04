#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#include <cstdint>

HANDLE SectionHandle = nullptr;
PVOID SharedMemory = nullptr;
HANDLE ThreadHandle = nullptr;
UNICODE_STRING SectionName = {};

typedef struct _SHARED_MEMORY
{
	BOOLEAN Close;
	BOOLEAN BaseAddressRequest;
	ULONG BufferAddress;
	void* Output;
	ULONG64 BaseAddress;
} SHARED_MEMORY, * PSHARED_MEMORY;

extern "C" {
    NTKERNELAPI PVOID PsGetProcessSectionBaseAddress( IN PEPROCESS  Process);
}
