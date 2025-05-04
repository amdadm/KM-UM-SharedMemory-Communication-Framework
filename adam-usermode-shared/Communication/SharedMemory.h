#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

HANDLE hMapFileW;
HANDLE hMapFileR;

uint64_t Pid;
uintptr_t BaseAddress;

typedef struct _SHARED_MEMORY
{
    BOOLEAN Close;
    BOOLEAN BaseAddressRequest;
    ULONG BufferAddress;
    void* Output;
    ULONG64 BaseAddress;
} SHARED_MEMORY, * PSHARED_MEMORY;


bool OpenSharedMemory()
{
    hMapFileW = OpenFileMappingA(FILE_MAP_WRITE, FALSE, "Global\\SharedMemory");
    if (!hMapFileW || hMapFileW == INVALID_HANDLE_VALUE) {
        return false;
    }
    hMapFileR = OpenFileMappingA(FILE_MAP_READ, FALSE, "Global\\SharedMemory");
    if (!hMapFileR || hMapFileR == INVALID_HANDLE_VALUE) {
        CloseHandle(hMapFileW);
        return false;
    }

    std::cout << " [+] Complete Shared Memory is available to use!" << std::endl;
   
    return true;
}

bool WriteToFile(_SHARED_MEMORY* instructions)
{
    auto pBuf = (_SHARED_MEMORY*)MapViewOfFile(hMapFileW, FILE_MAP_WRITE, 0, 0, sizeof(_SHARED_MEMORY));
    if (!pBuf)
    {
        printf(" [-] MapViewOfFile(write) fail! Error: %u\n", GetLastError());
        return false;
    }
    RtlCopyMemory(pBuf, instructions, sizeof(_SHARED_MEMORY));
    UnmapViewOfFile(pBuf);
    return true;
}

DWORD GetProcessID(const wchar_t* processName)
{
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(processEntry);
        if (Process32First(snapshot, &processEntry)) {
            do {
                if (_wcsicmp(processEntry.szExeFile, processName) == 0) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return processId;
}

bool CloseDriver()
{
    _SHARED_MEMORY Instructions;
    Instructions.Close = TRUE;
    Instructions.BaseAddressRequest = FALSE;

    if (WriteToFile(&Instructions))
    {
        std::cout << " [+] Sent Close Request!" << std::endl;
        return true;
    }
    else { return false; }
}

bool GetBaseAddress()
{
    SHARED_MEMORY Instructions = { 0 };
    Instructions.BaseAddressRequest = TRUE;
    Instructions.Close = FALSE;

    Instructions.BufferAddress = Pid;

    if (WriteToFile(&Instructions))
    {
        std::cout << " [+] Sent BaseAddress Request!" << std::endl;

        Sleep(50);

        auto pBuf = (SHARED_MEMORY*)MapViewOfFile(hMapFileR, FILE_MAP_READ, 0, 0, sizeof(SHARED_MEMORY));
        if (pBuf)
        {
            BaseAddress = reinterpret_cast<uintptr_t>(pBuf->Output);
            std::cout << " [+] Received BaseAddress from driver: 0x" << std::hex << BaseAddress << std::endl;
            UnmapViewOfFile(pBuf);
            return true;
        }
        else
        {
            std::cerr << " [-] Failed to map view of file for reading." << std::endl;
        }
    }
    else
    {
        std::cerr << " [-] Failed to write to shared memory." << std::endl;
    }
    return false;
}

bool CloseHandles()
{
    if (hMapFileW)
        CloseHandle(hMapFileW);
    if (hMapFileR)
        CloseHandle(hMapFileR);
    return true;
}