#include "scripts/passives/PassiveEffect.h"
#include "gameObjects/creatures/Creature.h"

PassiveEffect::PassiveEffect(Type type, int value, int rounds,
                             std::string_view id, std::string_view name,
                             bool stackable)
    : m_type{type}, m_value{value}, m_roundsLeft{rounds}, m_id{id},
      m_name{name}, m_stackable{stackable} {
  if (rounds <= 0)
    m_expiresOnRest = true;
}
PassiveEffect::Type PassiveEffect::getType() const { return m_type; }
int PassiveEffect::getValue() const { return m_value; }
int PassiveEffect::getRoundsLeft() const { return m_roundsLeft; }
void PassiveEffect::decrementRounds() {
  if (m_roundsLeft > 0)
    --m_roundsLeft;
}
bool PassiveEffect::isStackable() const { return m_stackable; }
bool PassiveEffect::expiresOnRest() const { return m_expiresOnRest; }
const std::string &PassiveEffect::getId() const { return m_id; }
void PassiveEffect::setRoundsLeft(int rounds) { m_roundsLeft = rounds; }
void PassiveEffect::applyEffect(Creature &target) const {
  switch (m_type) {
  case Type::DamageOverTime:
    target.takeDamage(m_value, true);
    break;
  default:
    break;
  }
}
bool PassiveEffect::isExpired() const {
  if (m_expiresOnRest)
    return false;
  return m_roundsLeft <= 0;
}
PassiveEffect::Type PassiveEffect::typeFromString(std::string_view type) {
  if (type == "evasionBonus")
    return Type::EvasionBonus;
  if (type == "strengthBonus")
    return Type::StrengthBonus;
  if (type == "dexterityBonus")
    return Type::DexterityBonus;
  if (type == "intelligenceBonus")
    return Type::IntelligenceBonus;
  if (type == "constitutionBonus")
    return Type::ConstitutionBonus;
  if (type == "damageOverTime")
    return Type::DamageOverTime;
  if (type == "composite")
    return Type::Composite;
  if (type == "meleeHitBonus")
    return Type::MeleeHitBonus;
  if (type == "distanceHitBonus")
    return Type::DistanceHitBonus;
  if (type == "meleeDamageBonus")
    return Type::MeleeDamageBonus;
  if (type == "distanceDamageBonus")
    return Type::DistanceDamageBonus;
  throw std::invalid_argument("Invalid passive effect type: " +
                              std::string(type));
}

int PassiveEffect::getStatModifier(Stat stat) const {
  switch (m_type) {
  case Type::EvasionBonus:
    if (stat == Stat::Evasion)
      return m_value;
    break;
  case Type::StrengthBonus:
    if (stat == Stat::Strength)
      return m_value;
    break;
  case Type::DexterityBonus:
    if (stat == Stat::Dexterity)
      return m_value;
    break;
  case Type::IntelligenceBonus:
    if (stat == Stat::Intelligence)
      return m_value;
    break;
  case Type::ConstitutionBonus:
    if (stat == Stat::Constitution)
      return m_value;
  case Type::ArmorBonus:
    if (stat == Stat::Armor)
      return m_value;
    break;
  case Type::MeleeDamageBonus:
    if (stat == Stat::MeleeDamage)
      return m_value;
    break;
  case Type::DistanceDamageBonus:
    if (stat == Stat::DistanceDamage)
      return m_value;
    break;
  case Type::MeleeHitBonus:
    if (stat == Stat::MeleeHitChance)
      return m_value;
    break;
  case Type::DistanceHitBonus:
    if (stat == Stat::DistanceHitChance)
      return m_value;
    break;
  default:
    break;
  }
  return 0;
}

std::unique_ptr<PassiveEffect> PassiveEffect::clone() const {
  return std::make_unique<PassiveEffect>(*this);
}

const std::string &PassiveEffect::getName() const { return m_name; }

std::string PassiveEffect::typeToString(Type type) {
  switch (type) {
  case Type::EvasionBonus:
    return "evasionBonus";
  case Type::StrengthBonus:
    return "strengthBonus";
  case Type::DexterityBonus:
    return "dexterityBonus";
  case Type::IntelligenceBonus:
    return "intelligenceBonus";
  case Type::ConstitutionBonus:
    return "constitutionBonus";
  case Type::DamageOverTime:
    return "damageOverTime";
  case Type::ArmorBonus:
    return "armorBonus";
  case Type::MeleeHitBonus:
    return "meleeHitBonus";
  case Type::DistanceHitBonus:
    return "distanceHitBonus";
  case Type::MeleeDamageBonus:
    return "meleeDamageBonus";
  case Type::DistanceDamageBonus:
    return "distanceDamageBonus";
  case Type::Composite:
    return "composite";
  default:
    return "unknown";
  }
}

std::string PassiveEffect::display() const {
  std::string result{m_name};
  result += ": " + typeToString(m_type) + " ";
  result += std::to_string(m_value) + ", ";
  if (m_roundsLeft > 0)
    result += std::to_string(m_roundsLeft) + " rounds left";
  else
    result += "Until rest";
  return result;
}

bool PassiveEffect::isBuff() const {
  switch (m_type) {
  case Type::EvasionBonus:
  case Type::StrengthBonus:
  case Type::DexterityBonus:
  case Type::ArmorBonus:
  case Type::IntelligenceBonus:
  case Type::ConstitutionBonus:
  case Type::MeleeDamageBonus:
  case Type::DistanceDamageBonus:
  case Type::MeleeHitBonus:
  case Type::DistanceHitBonus:
    return m_value > 0;
  default:
    return false;
  }
}

bool PassiveEffect::isDebuff() const {
  switch (m_type) {
  case Type::EvasionBonus:
  case Type::StrengthBonus:
  case Type::DexterityBonus:
  case Type::ArmorBonus:
  case Type::IntelligenceBonus:
  case Type::ConstitutionBonus:
  case Type::MeleeDamageBonus:
  case Type::DistanceDamageBonus:
  case Type::MeleeHitBonus:
  case Type::DistanceHitBonus:
    return m_value < 0;
  default:
    return false;
  }
}

using json = nlohmann::json;

json PassiveEffect::toJson() const {
  json j;
  j["id"] = m_id;
  j["name"] = m_name;
  j["type"] = typeToString(m_type);
  j["value"] = m_value;
  j["duration"] = m_roundsLeft;
  j["stackable"] = m_stackable;
  j["expiresOnRest"] = m_expiresOnRest;
  return j;
}