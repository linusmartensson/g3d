#include"core.h"

int main(int argc, char* argv[]) {
	syscore::core().init();
	setup(argc, argv);
	return syscore::core().mainloop();
}

