#pragma once

#include "xxhash.h"

#define HASH_KEY(s) XXH3_64bits(s, strlen(s))

