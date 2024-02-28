#include "../include/sHELL.h"
#include <bcrypt.h>
#include <windows.h>

const char Name[] = "sha256sum";
const char Help[] = "do later";

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)

InternalAPI *core = NULL;

void HashInput(char *input) {

  BCRYPT_ALG_HANDLE hAlg = NULL;
  BCRYPT_HASH_HANDLE hHash = NULL;
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  DWORD cbData = 0, cbHash = 0, cbHashObject = 0;
  PBYTE pbHashObject = NULL;
  PBYTE pbHash = NULL;

  // open an algorithm handle
  if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                      &hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0))) {
    core->wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n",
                  status);
    goto Cleanup;
  }

  // calculate the size of the buffer to hold the hash object
  if (!NT_SUCCESS(status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
                                             (PBYTE)&cbHashObject,
                                             sizeof(DWORD), &cbData, 0))) {
    core->wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
    goto Cleanup;
  }

  // allocate the hash object on the heap
  pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
  if (NULL == pbHashObject) {
    core->wprintf(L"**** memory allocation failed\n");
    goto Cleanup;
  }

  // calculate the length of the hash
  if (!NT_SUCCESS(status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH,
                                             (PBYTE)&cbHash, sizeof(DWORD),
                                             &cbData, 0))) {
    core->wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
    goto Cleanup;
  }

  // allocate the hash buffer on the heap
  pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);
  if (NULL == pbHash) {
    core->wprintf(L"**** memory allocation failed\n");
    goto Cleanup;
  }

  // create a hash
  if (!NT_SUCCESS(status = BCryptCreateHash(hAlg, &hHash, pbHashObject,
                                            cbHashObject, NULL, 0, 0))) {
    core->wprintf(L"**** Error 0x%x returned by BCryptCreateHash\n", status);
    goto Cleanup;
  }

  // hash some data
  if (!NT_SUCCESS(status =
                      BCryptHashData(hHash, (PBYTE)input, sizeof(input), 0))) {
    core->wprintf(L"**** Error 0x%x returned by BCryptHashData\n", status);
    goto Cleanup;
  }

  // close the hash
  if (!NT_SUCCESS(status = BCryptFinishHash(hHash, pbHash, cbHash, 0))) {
    core->wprintf(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
    goto Cleanup;
  }

  core->wprintf(L"Success!\n");

Cleanup:

  if (hAlg) {
    BCryptCloseAlgorithmProvider(hAlg, 0);
  }

  if (hHash) {
    BCryptDestroyHash(hHash);
  }

  if (pbHashObject) {
    HeapFree(GetProcessHeap(), 0, pbHashObject);
  }

  if (pbHash) {
    HeapFree(GetProcessHeap(), 0, pbHash);
  }
}

LPVOID lpOut = NULL;
__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}

// initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) { core = lpCore; }

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
  // Example implementation: print arguments and return count
  if (argc != 2) {
    core->wprintf(L"Usage: %s input_to_hash", argv[0]);
    return NULL;
  }
  HashInput(argv[1]);
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
