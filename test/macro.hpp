#ifndef __TEST_MACRO_HPP__
#define __TEST_MACRO_HPP__

#include <debug.hpp>

#define CHECK(condition) if (!(condition)) warn("assertation ", #condition, " failed")
#define FATAL_CHECK(condition) if (!(condition)) { warn("fatal assertation ", #condition, " failed"); return; }

#endif /*__TEST_MACRO_HPP__*/
