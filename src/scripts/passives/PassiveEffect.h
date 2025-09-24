#pragma once
#include "gameObjects/creatures/Stats.h"
#include <memory>
#include <string>
#include <string_view>

class Creature;

class PassiveEffect {
public:
  enum class Type {
    EvasionBonus,
    StrengthBonus,
    DexterityBonus,
    IntelligenceBonus,
    ConstitutionBonus,
    DamageOverTime,
    Composite,
  };

protected:
  Type m_type;
  int m_value;
  int m_roundsLeft;
  std::string m_id{};
  std::string m_name{};
  bool m_stackable{false};
  bool m_expiresOnRest{false};

public:
  PassiveEffect(Type type, int value, int rounds, std::string_view id,
                std::string_view name, bool stackable = false);
  Type getType() const;
  int getValue() const;
  int getRoundsLeft() const;
  void decrementRounds();
  bool isStackable() const;
  const std::string &getId() const;
  void setRoundsLeft(int rounds);
  virtual void applyEffect(Creature &target) const;
  bool isExpired() const;
  static Type typeFromString(std::string_view type);
  virtual int getStatModifier(Stat stat) const;
  const std::string &getName() const;
  virtual std::unique_ptr<PassiveEffect> clone() const;
  virtual std::string display() const;
  static std::string typeToString(Type type);
  virtual ~PassiveEffect() = default;
};