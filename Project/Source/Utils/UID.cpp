#include "UID.h"

#include <random>

#include "Utils/Leaks.h"

static std::random_device random;
static std::mt19937_64 mersenneTwister(random());
static std::uniform_int_distribution<UID> distribution;

UID GenerateUID() {
	return distribution(mersenneTwister);
}
