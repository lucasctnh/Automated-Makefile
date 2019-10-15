#include <windows.h>
#include <unistd.h>

int main() {
	if(chdir("/C/Users/coutl/Coding/C/TPsBaselines") == -1) {
		ShellExecute(0, "open", "cmd.exe", "/C cd C:/Users/coutl/Coding/C/TPsBaselines && make clean", 0, SW_HIDE);
	}

	return 0;
}