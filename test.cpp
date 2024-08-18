#define DBG_MACRO_NO_WARNING
#include "dbg.h"

#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
	spdlog::info("info");
	spdlog::error("error");

	return 0;
}
