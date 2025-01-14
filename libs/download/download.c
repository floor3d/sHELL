
#include "../include/sHELL.h"
#include "errhandlingapi.h"
#include <urlmon.h>
#include <windows.h>

const char Name[] = "download";
const char Help[] =
    "Download a file from a specified URL to the local filesystem.\n"
    "Usage:\n"
    "    download <URL> <local file path>";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;
__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}
// initialization code
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
  // Example implementation: print arguments and return count
  if (argc != 3) {
    core->wprintf(L"Invalid arguments.\n%S", CommandHelpA());
    return (LPVOID)1; // Error code for invalid arguments
  }
  // // your answer here
  // for some reason this does not work on the azure instance. Do it somewhere
  // else skull emoji x7
  core->wprintf(L"Downloading file %S from %S\n", argv[2], argv[1]);
  HRESULT hr = URLDownloadToFileA(NULL, argv[1], argv[2], 0, NULL);

  if (SUCCEEDED(hr)) {
    // Download successful
    core->wprintf(L"Successfully downloaded file %S!\n", argv[2]);
  } else {
    // Download failed
    core->wprintf(L"Error downloading file. HRESULT: 0x%x\n", hr);
  }

  return 0;
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    // Code to run when the DLL is loaded
    break;
  case DLL_PROCESS_DETACH:
    // Code to run when the DLL is unloaded
    break;
  case DLL_THREAD_ATTACH:
    // Code to run when a thread is created during DLL's existence
    break;
  case DLL_THREAD_DETACH:
    // Code to run when a thread ends normally
    break;
  }
  return TRUE; // Successful
}
