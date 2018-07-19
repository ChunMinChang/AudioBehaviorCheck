#ifndef UTILS_H
#define UTILS_H

#include <ctime>		// for clock(), CLOCKS_PER_SEC
#include <iostream>	// for fprintf

#define ENABLE_LOG true
#define LOG(...) ENABLE_LOG && fprintf(stderr, __VA_ARGS__)

void delay(unsigned int ms)
{
	clock_t end;
	end = clock() + ms * (CLOCKS_PER_SEC/1000);
	while (clock() < end) {}
}

#endif // #ifndef UTILS_H
