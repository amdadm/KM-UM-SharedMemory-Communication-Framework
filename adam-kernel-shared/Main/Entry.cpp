#pragma once

#include "SharedMemory/CreateSharedMemory.h"
#include "SharedMemory/ThreadLoop.h"

/*===============================================================================================
  Project         : KM-UM-Shared-Memory-Communication-Framework

  Original Author : amdadm
  GitHub          : https://github.com/amdadm
  Created         : 2025-04-29

  Description     : Framework for establishing communication between Kernel Mode (KM) and User Mode (UM) components through Shared Memory.

  License         : MIT License
===============================================================================================*/

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    const auto Status = PsCreateSystemThread(
        &ThreadHandle,
        GENERIC_ALL,
        nullptr,
        nullptr,
        nullptr,
        ThreadLoop,
        nullptr
    );

    if (!NT_SUCCESS(Status))
        return Status;

    ZwClose(ThreadHandle);

    return STATUS_SUCCESS;
}

