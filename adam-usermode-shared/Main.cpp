#include "Communication/SharedMemory.h"

/*===============================================================================================
  Project         : KM-UM-Shared-Memory-Communication-Framework

  Original Author : amdadm
  GitHub          : https://github.com/amdadm
  Created         : 2025-04-29

  Description     : Framework for establishing communication between Kernel Mode (KM) and User Mode (UM) components through Shared Memory.

  License         : MIT License
===============================================================================================*/

int main()
{
    while (true)
    {
        if (OpenSharedMemory())
        {
            system("cls");
            break;
        }
        if (!OpenSharedMemory())
        {
            std::cerr << " [-] Failed to open shared memory." << std::endl;
        }
        system("cls");
        std::cout << "\n [+] Driver not loaded, waiting..." << std::endl;
        Sleep(1000);
    } 

    Pid = GetProcessID(L"Notepad.exe");

    std::cout << "\n [+] Process Id: " << Pid << std::endl;

    GetBaseAddress();

    std::cout << " [+] Base Address: 0x" << BaseAddress << std::endl;

    CloseDriver();
    CloseHandles();
    std::cin.get();
    return 0;
}