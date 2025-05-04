#pragma once

#include "../Includes/Structs.h"

NTSTATUS GetBaseAddress(HANDLE ProcessPid, uintptr_t* BaseAddress) {

    PEPROCESS TargetProcess = nullptr;

    NTSTATUS  Status = PsLookupProcessByProcessId(ProcessPid, &TargetProcess);
    if (!NT_SUCCESS(Status))
        return 0;

    *BaseAddress = uintptr_t(PsGetProcessSectionBaseAddress(TargetProcess));
    ObDereferenceObject(TargetProcess);

    return STATUS_SUCCESS;
}