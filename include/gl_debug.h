#pragma once

#include <iostream>

#ifdef DEBUG
#define DEBUG_OUT(x) std::cout << x << std::endl;
#else
#define DEBUG_OUT(x)
#endif
