#pragma once

#include "../../Includes/Structs.h"

#include "../../Features/GetBaseAddress.h"

VOID ThreadLoop(PVOID Context) {

    UNREFERENCED_PARAMETER(Context);

    DbgPrintEx(0, 0, "ThreadLoop Started\n");

    if (!NT_SUCCESS(CreateSharedMemory()))
    {
        DbgPrintEx(0, 0, "Failed to create shared memory.\n");
        PsTerminateSystemThread(STATUS_UNSUCCESSFUL);
    }

    auto* Instructions = static_cast<SHARED_MEMORY*>(SharedMemory);
    if (!Instructions)
    {
        DbgPrintEx(0, 0, "Instructions pointer is NULL!\n");
        PsTerminateSystemThread(STATUS_UNSUCCESSFUL);
    }

    LARGE_INTEGER delayInterval;
    delayInterval.QuadPart = -10 * 1000 * 5;

    while (true)
    {
        if (!SharedMemory)
        {
            DbgPrintEx(0, 0, "Shared memory is NULL. Exiting thread.\n");
            break;
        }

        // Close Shared Memory
        if (Instructions->Close == TRUE)
        {
            DbgPrintEx(0, 0, "Received Close Request! Shutting down...\n");

            if (SharedMemory)
                ZwUnmapViewOfSection(ZwCurrentProcess(), SharedMemory);

            if (SectionHandle)
                ZwClose(SectionHandle);

            break;
        }

        // Base Address
        if (Instructions->BaseAddressRequest == TRUE)
        {
           DbgPrintEx(0, 0, "Received BaseAddress Request!\n");

            uintptr_t BaseAddress = 0;
            NTSTATUS status = GetBaseAddress(reinterpret_cast<HANDLE>(Instructions->BufferAddress), &BaseAddress);

            if (NT_SUCCESS(status))
            {
                Instructions->BaseAddress = BaseAddress;
                Instructions->Output = reinterpret_cast<void*>(BaseAddress);
            }
            else
            {
                Instructions->BaseAddress = 0;
                Instructions->Output = nullptr;
            }

            Instructions->BaseAddressRequest = FALSE;
        }

        // ...

        KeDelayExecutionThread(KernelMode, FALSE, &delayInterval);
    }

    PsTerminateSystemThread(STATUS_SUCCESS);
}