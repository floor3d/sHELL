#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "cd";
const char Help[] = "Change the shell's current directory.\n"
                    "Usage:\n"
                    "    cd <path>";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;
__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}

// Initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) {
  core = lpCore;
  return TRUE;
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
  if (argc != 2) {
    core->wprintf(L"Invalid arguments.\n%s", CommandHelpA());
    return NULL; // Error code for invalid arguments
  }
  // your answer here
  // stolen from microsoft api docs
  if (!SetCurrentDirectory(argv[1])) {
    core->wprintf(L"SetCurrentDirectory failed (%d)\n", GetLastError());
    return NULL;
  }
  core->wprintf(L"Set current directory to %s\n", argv[1]);
  return (LPVOID)1; // Success indicates directory change
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  }
  return TRUE; // Successful
}
