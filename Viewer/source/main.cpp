#include <iostream>
#include <stdexcept>

#include "include/viewer.h"

int main(int argc, char** argv) {
    Eagle::Viewer viewer;

    try {
        viewer.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

	return 0;
}