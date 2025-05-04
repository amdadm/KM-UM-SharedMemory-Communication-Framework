#pragma once

#include "../../Includes/Structs.h"

NTSTATUS CreateSharedMemory() {

    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    constexpr WCHAR SharedSectionName[] = L"\\BaseNamedObjects\\SharedMemory";
   
    RtlInitUnicodeString(&SectionName, SharedSectionName);

    SECURITY_DESCRIPTOR SecurityDescriptro;

    Status = RtlCreateSecurityDescriptor(&SecurityDescriptro, SECURITY_DESCRIPTOR_REVISION);
    if (!NT_SUCCESS(Status))
    {
        DbgPrintEx(0, 0, "RtlCreateSecurityDescriptor Failed! Status: %08X\n", Status);
        return Status;
    }

    Status = RtlSetDaclSecurityDescriptor(&SecurityDescriptro, TRUE, nullptr, FALSE);
    if (!NT_SUCCESS(Status))
    {
        DbgPrintEx(0, 0, "RtlSetDaclSecurityDescriptor Failed! Status: %08X\n", Status);
        return Status;
    }

    OBJECT_ATTRIBUTES objAttributes = {};
    InitializeObjectAttributes(&objAttributes, &SectionName, OBJ_CASE_INSENSITIVE, nullptr, &SecurityDescriptro);

    LARGE_INTEGER lMaxSize = {};
    lMaxSize.LowPart = 1024 * 10;

    SIZE_T viewSize = 1024 * 10;

    Status = ZwCreateSection(&SectionHandle, SECTION_ALL_ACCESS, &objAttributes, &lMaxSize, PAGE_READWRITE, SEC_COMMIT, nullptr);
    if (!NT_SUCCESS(Status))
    {
        DbgPrintEx(0, 0, "ZwCreateSection Failed! Status: %08X\n", Status);
        return Status;
    }

    Status = ZwMapViewOfSection(SectionHandle, ZwCurrentProcess(), &SharedMemory, 0, 0, nullptr, &viewSize, ViewShare, 0, PAGE_READWRITE);
    if (!NT_SUCCESS(Status))
    {
        DbgPrintEx(0, 0, "ZwMapViewOfSection Failed! Status: %08X\n", Status);
        ZwClose(SectionHandle);
        return Status;
    }
    return STATUS_SUCCESS;
}