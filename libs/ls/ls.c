
#include "../include/sHELL.h"
#include "fileapi.h"
#include "handleapi.h"
#include "minwinbase.h"
#include "minwindef.h"
#include <windows.h>

// `ls` - List Directory Contents

// **Command Name and Help:**

const char Name[] = "ls";
const char Help[] =
    "List information about the files (the current directory by default).\n"
    "Flags:\n"
    "    -R  List subdirectories recursively.\n"
    "    -a  Show all files, including hidden ones.\n"
    "    -l  Use a long listing format showing file attributes.\n"
    "Usage:\n"
    "    ls [-Ral] [path]";

// ### Requirements:
// - List files and directories in the current or specified directory.
// - Support flags: -R (recursive), -a (all files), -l (long format).

void FileAttributesToString(const WIN32_FIND_DATAA *findData, char *result) {
  // File attributes
  core->wprintf(
      L"%c%c%c\t%lu bytes\t%s\n",
      (findData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 'd' : '-',
      (findData->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? 'a' : '-',
      (findData->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? 'h' : '-',
      (unsigned long)findData->nFileSizeLow, findData->cFileName);
  // return result;
}

void PrintFileData(WIN32_FIND_DATAA *lpFileData, BOOL find_hidden,
                   BOOL show_attrs) {
  if (!((lpFileData->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
        !find_hidden)) {
    if (show_attrs) {
      char attrs[2048];
      FileAttributesToString(lpFileData, attrs);
      // core->wprintf(L"%s", attrs);
    } else {
      core->wprintf(L"%s", lpFileData->cFileName);
    }
  }
}

InternalAPI *core = NULL;

// initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) { core = lpCore; }

// Exported function - Name
__declspec(dllexport) const char *CommandName() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelp() { return Help; }

// Exported function - Run
__declspec(dllexport) int CommandRun(int argc, char **argv) {
  BOOL find_hidden = FALSE;
  BOOL show_attrs = FALSE;
  BOOL recurse = FALSE;
  WIN32_FIND_DATAA *lpFileData = NULL;
  char *lpFileName = ".\\*";
  HANDLE hFind = INVALID_HANDLE_VALUE;
  // we must go through the arguments and see what needs to be added
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      for (int j = 1; j < lstrlenA(argv[i]); j++) {
        if (argv[i][j] == 'R') {
          recurse = TRUE;
        } else if (argv[i][j] == 'a') {
          find_hidden = TRUE;
        } else if (argv[i][j] == 'l') {
          show_attrs = TRUE;
        }
      }
    } else if (i == argc - 1) {
      // treat as path
      lpFileName = argv[i];
    }
  }
  hFind = FindFirstFileA(lpFileName, lpFileData);
  if (hFind == INVALID_HANDLE_VALUE) {
    core->wprintf(L"\n");
    return 0;
  }

  PrintFileData(lpFileData, find_hidden, show_attrs);

  while (FindNextFileA(hFind, lpFileData) != 0) {
    PrintFileData(lpFileData, find_hidden, show_attrs);
  }
  DWORD error = GetLastError();
  if (error != ERROR_NO_MORE_FILES) {
    return error;
  }

  FindClose(hFind);
  core->wprintf(L"I am so modular!\n");
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
