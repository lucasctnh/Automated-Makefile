#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <stdint.h>
#include <wchar.h>
#include <string.h>

void HandleChangeEvent(FILE_NOTIFY_INFORMATION *event);

int main() {

	// Variables declaration
	bool bWatchSubtree = false;
	uint8_t buffer[1024];
    DWORD dwBytes;
    HANDLE hDir;
    FILE_NOTIFY_INFORMATION *event;
    BOOL success;

	unsigned int flags = FILE_NOTIFY_CHANGE_LAST_WRITE |
                    	FILE_NOTIFY_CHANGE_SIZE |
                    	FILE_NOTIFY_CHANGE_DIR_NAME |
                    	FILE_NOTIFY_CHANGE_FILE_NAME;

	// Assign directory handler
	hDir = CreateFile(
        "C:\\Users\\coutl\\Coding\\C\\Script",
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	if(hDir == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, "Failed to open directory.", "Error", MB_OK);
	}

	//
	printf("Looking for changes...\n");

	// Start watcher logic
	while(1) {
		success = ReadDirectoryChangesW(hDir, buffer, sizeof(buffer), bWatchSubtree, flags, &dwBytes, NULL, NULL);

		if(!success) {
			MessageBox(NULL, "Failed to read directory changes.", "Error", MB_OK);
		}

		event = (FILE_NOTIFY_INFORMATION*)buffer;

		while(1) {
			HandleChangeEvent(event);

			// To handle every next events
			if (event->NextEntryOffset)
                *((uint8_t**)&event) += event->NextEntryOffset;
            else
                break;
		}
	}

	return 0;
}

void HandleChangeEvent(FILE_NOTIFY_INFORMATION* event)
{
    DWORD name_len;//, name;

    name_len = (event->FileNameLength / sizeof(wchar_t)) - 2; // -2 for filename without extension
	// name = (int)event->FileName;

    switch (event->Action)
    {
    case FILE_ACTION_ADDED:
        wprintf(L"	    Added: %.*s\n", name_len, event->FileName);
        break;
    case FILE_ACTION_REMOVED:
        wprintf(L"	  Removed: %.*s\n", name_len, event->FileName);
        break;
    case FILE_ACTION_MODIFIED:
        // wprintf(L"	 Modified: %.*s\n", name_len, event->FileName);
		wprintf(L"%s\n", event->FileName);

		// Call the especial function to create our Makefile
		//LiterallyMakefile()

        break;
    case FILE_ACTION_RENAMED_OLD_NAME:
        wprintf(L"Renamed from: %.*s\n", name_len, event->FileName);
        break;
    case FILE_ACTION_RENAMED_NEW_NAME:
        wprintf(L"	       	to: %.*s\n", name_len, event->FileName);
        break;
    default:
        printf("Unknown action!\n");
        break;
    }
}