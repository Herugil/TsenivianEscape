#pragma once

enum class Stat {
  Strength,
  Dexterity,
  Intelligence,
  Constitution,
  Evasion,
  Armor,
  MeleeHitChance,
  MeleeDamage,
  DistanceHitChance,
  DistanceDamage,
  nbStats
};
struct Stats {
  int strength{};
  int dexterity{};
  int intelligence{};
  int constitution{};
};