#pragma once

class PassiveEffect {
public:
  enum class Type {
    EvasionBonus,
    StrengthBonus,
    DexterityBonus,
    IntelligenceBonus,
    ConstitutionBonus,
    DamageOverTime
  };

protected:
  Type m_type;
  int m_value;
  int m_roundsLeft;

public:
  PassiveEffect(Type type, int value, int rounds)
      : m_type{type}, m_value{value}, m_roundsLeft{rounds} {}
  Type getType() const { return m_type; }
  int getValue() const { return m_value; }
  int getRoundsLeft() const { return m_roundsLeft; }
  void decrementRounds() {
    if (m_roundsLeft > 0)
      --m_roundsLeft;
  }
  virtual void applyEffect() const {};
  bool isExpired() const { return m_roundsLeft <= 0; }
  virtual ~PassiveEffect() = default;
};