#include "enhancements.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include <cstdint>
#include <ctime>

constexpr const char* kSinglePlayerShufflerCVar = "gEnhancements.SinglePlayerShuffler";
constexpr unsigned char kFighterCount = 12;
constexpr unsigned char kStageCount = 9;

static int sPortIsRandomOpponentsActive = 0;
static unsigned char sPortRandomOpponents[2] = { 0, 0 };
static int sPortRandomEnemyCount = 1;
static unsigned char sPortRandomStage = 0;

// Custom self-contained PRNG state
static uint32_t sRandomSeed = 1;

extern "C" {
    void port_enhancement_set_is_team(int is_team);

    // Xorshift PRNG: Mathematically guaranteed to advance its sequence
    static uint32_t GetNextRandom(uint32_t max) {
        sRandomSeed ^= sRandomSeed << 13;
        sRandomSeed ^= sRandomSeed >> 17;
        sRandomSeed ^= sRandomSeed << 5;
        return sRandomSeed % max;
    }

    void port_enhancement_init_randomizer(void) {
        sRandomSeed = static_cast<uint32_t>(std::time(nullptr));
        if (sRandomSeed == 0) sRandomSeed = 1; // Seed cannot be 0
    }

    int port_enhancement_is_shuffler_enabled(void) {
        return CVarGetInteger(ssb64::enhancements::SinglePlayerShufflerCVarName(), 0) != 0;
    }

    void port_enhancement_randomize_round_stage(void) {
        if (!port_enhancement_is_shuffler_enabled()) {
            sPortIsRandomOpponentsActive = 0;
            return;
        }

        // 1. Roll the Physical Map
        sPortRandomStage = (unsigned char)GetNextRandom(kStageCount);

        // 2. Roll a completely random Match Archetype
        int matchMode = GetNextRandom(4);
        sPortIsRandomOpponentsActive = 1;

        if (matchMode == 1) { // 2v2
            sPortRandomOpponents[0] = (unsigned char)GetNextRandom(kFighterCount);
            sPortRandomOpponents[1] = (unsigned char)GetNextRandom(kFighterCount);
            port_enhancement_set_is_team(1);
            sPortRandomEnemyCount = 2;
        }
        else if (matchMode == 2) { // Horde
            sPortRandomOpponents[0] = (unsigned char)GetNextRandom(kFighterCount);
            sPortRandomOpponents[1] = sPortRandomOpponents[0];
            port_enhancement_set_is_team(0);
            sPortRandomEnemyCount = 3;
        }
        else if (matchMode == 3) { // Special
            sPortRandomOpponents[0] = (GetNextRandom(2) == 0) ? 6 : 10;
            sPortRandomOpponents[1] = 0;
            port_enhancement_set_is_team(0);
            sPortRandomEnemyCount = 1;
        }
        else { // Standard 1v1
            sPortRandomOpponents[0] = (unsigned char)GetNextRandom(kFighterCount);
            sPortRandomOpponents[1] = 0;
            port_enhancement_set_is_team(0);
            sPortRandomEnemyCount = 1;
        }
    }

    int port_enhancement_is_random_opponents_active(void) { return sPortIsRandomOpponentsActive; }
    int port_enhancement_get_random_enemy_count(void) { return sPortRandomEnemyCount; }
    unsigned char port_enhancement_get_random_stage(void) { return sPortRandomStage; }

    unsigned char port_enhancement_get_random_opponent(int index) {
        if (sPortRandomEnemyCount == 4) return sPortRandomOpponents[0]; // Horde clones
        if (index >= 0 && index < 2) return sPortRandomOpponents[index];
        return 0;
    }
}
