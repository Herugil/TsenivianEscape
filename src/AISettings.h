#pragma once

namespace AISettings {
// fleeing behaviors
constexpr int g_waryMeleeFleeChance{25};
constexpr int g_waryMeleeFleeHealthPercent{40};

// buffing behavior
constexpr int g_buffChanceBaseMult{15};
constexpr int g_buffChanceMax{85};
constexpr int g_buffChanceBase{50};
constexpr int g_selfBuffLikelihoodPenalty{50};
constexpr int g_nonVisibleTargetPenalty{50};
constexpr int g_alreadyBuffedPenalty{25};

// boss behavior
constexpr int g_bossHealHealthPercent{50};
constexpr int g_bossHealChance{60};
constexpr int g_bossActionChance{75};
} // namespace AISettings