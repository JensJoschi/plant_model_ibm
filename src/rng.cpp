//This file is not meant for sharing. If you have received the file in error, please email us immediately at info@ecolopes.org

// --------------------------------------------------------------------------
// TEMPORARY. PLANT MODEL SHOULD NOT INCLUDE JOINT MODEL CODE IN FOLDER SRC. 
// --------------------------------------------------------------------------


#include <random>
#include "rng.h"

namespace RNGs{
	std::mt19937 mersenne{std::random_device{}()};
}