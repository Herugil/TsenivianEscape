#pragma once

enum class Stat {
  Strength,
  Dexterity,
  Intelligence,
  Constitution,
  Evasion,
  Armor,
  nbStats
};
struct Stats {
  int strength{};
  int dexterity{};
  int intelligence{};
  int constitution{};
};