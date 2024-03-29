#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <stdint.h>
#include <wchar.h>
#include <string.h>
#include <unistd.h>

void HandleChangeEvent(FILE_NOTIFY_INFORMATION *event);
void LiterallyMakefile(wchar_t *filename, int filenameLength);
void WriteOnMakefile(wchar_t* filename, int filenameLength);

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
	// Manual input here, could be dynamic
	hDir = CreateFile(
        "C:\\Users\\coutl\\Coding\\C\\TPsBaselines",
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
    DWORD name_len;
	WCHAR* name;

    name_len = event->FileNameLength / sizeof(wchar_t);
	name = event->FileName;
	name[name_len] = '\0';

	// When is a Makefile do nothing, when isn't -2 on filename length to remove extension
	if(wcscmp(name, L"Makefile") != 0) {
		name_len -= 2;
		name = event->FileName;
		name[name_len] = '\0';

		switch (event->Action) {
		case FILE_ACTION_ADDED:
			wprintf(L"	    Added: %.*s\n", name_len, event->FileName);
			break;
		case FILE_ACTION_REMOVED:
			wprintf(L"	  Removed: %.*s\n", name_len, event->FileName);
			break;
		case FILE_ACTION_MODIFIED:
			wprintf(L"	 Modified: %.*s\n", name_len, event->FileName);

			// Call the especial function to create our Makefile
			LiterallyMakefile(name, name_len);
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
}

void WriteOnMakefile(wchar_t* filename, int filenameLength) {
	HANDLE nMk;
	DWORD bytesWritten;

	// Manual input here, could be dynamic
	nMk = CreateFile(
		"C:\\Users\\coutl\\Coding\\C\\TPsBaselines\\Makefile",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	// Manual input here, could be dynamic
	char strPart1[] = "# Name of the project\nPROJ_NAME=";
	char strPart3[] = "\nPROJ_EXE=$(PROJ_NAME).exe\n\n# Allegro inclusion\nALLEGRO_VERSION=5.0.10\nMINGW_VERSION=4.7.0\nFOLDER=C:\\Users\\coutl\\Coding\\C\n\nFOLDER_NAME=\\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)\nPATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)\nLIB_ALLEGRO=\\lib\\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a\nINCLUDE_ALLEGRO=\\include\n\n# .c file\nC_SOURCE=$(PROJ_NAME).c\n\n# Object file\nOBJ=$(C_SOURCE:.c=.o)\n\n# Compiler\nCC=gcc\n\n#\n# Compilation and linking\n#\nall: $(PROJ_EXE)\n\n$(PROJ_EXE): $(OBJ)\n	$(CC) -o $@ $^ $(PATH_ALLEGRO)$(LIB_ALLEGRO)\n\n$(OBJ): $(C_SOURCE)\n	$(CC) -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c $<\n\nclean:\n	del *.o *.exe\n";

	// Convert wchar string to char string
	char part2Convert[filenameLength];
	int length;

	length = wcstombs(part2Convert, filename, sizeof(part2Convert));

	if (length) {
		part2Convert[length] = '\0';
		// Sucessfully converted:
		printf("Filename: %s\n", part2Convert);
	}

	WriteFile(
    	nMk,
		strPart1,
    	(sizeof(strPart1) - 1),
    	&bytesWritten,
    	NULL
	);

	WriteFile(
    	nMk,
		part2Convert,
    	sizeof(part2Convert),
    	&bytesWritten,
    	NULL
	);

	WriteFile(
    	nMk,
		strPart3,
    	(sizeof(strPart3) - 1),
    	&bytesWritten,
    	NULL
	);

   	CloseHandle(nMk);
}

void LiterallyMakefile(wchar_t* filename, int filenameLength) {

	// Create Makefile
	HANDLE nMkfl;

	// Manual input here, could be dynamic
	nMkfl = CreateFile(
		"C:\\Users\\coutl\\Coding\\C\\TPsBaselines\\Makefile",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if(nMkfl == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, "Failed to create Makefile.", "Error", MB_OK);
	}

	// Close Makefile handler to not conflit
	CloseHandle(nMkfl);

	// Call the function that writes in makefile depending of the file modified
	WriteOnMakefile(filename, filenameLength);

	// Verify directory and run make on Command Shell
	if(chdir("/C/Users/coutl/Coding/C/TPsBaselines") == -1) {
		ShellExecute(0, "open", "cmd.exe", "/C cd C:/Users/coutl/Coding/C/TPsBaselines && make", 0, SW_HIDE);
	}
}