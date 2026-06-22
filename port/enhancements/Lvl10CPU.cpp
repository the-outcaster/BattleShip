#include "enhancements.h"
#include <libultraship/bridge/consolevariablebridge.h>

constexpr const char* kLevel10CPUCVar = "gCheats.Lvl10CPU";

// Expose the CVar check to the native C codebase
extern "C" int port_cheat_level10_active(void) {
    return CVarGetInteger(kLevel10CPUCVar, 0) != 0 ? 1 : 0;
}

namespace ssb64 {
    namespace enhancements {

        const char* Level10CPUCVarName() {
            return kLevel10CPUCVar;
        }

    } // namespace enhancements
} // namespace ssb64
