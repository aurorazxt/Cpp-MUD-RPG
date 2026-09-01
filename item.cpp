#include "mud_common.h"

Item::Item() : name(""), price(0), description("") {}

Item::Item(string n, int p, string desc) : name(std::move(n)), price(p), description(std::move(desc)) {}

string Item::getName() const { return name; }
int Item::getPrice() const { return price; }
string Item::getDescription() const { return description; }

void Item::setName(const string& n) { name = n; }
void Item::setPrice(int p) { price = p; }
void Item::setDescription(const string& desc) { description = desc; }

bool Item::use(Player& p) {
    (void)p;
    return false;
}

string Item::getType() const { return "Item"; }

Weapon::Weapon() : Item("", 0, ""), atkBonus(0) {}
Weapon::Weapon(string n, int bonus, int p) : Item(std::move(n), p, "武器"), atkBonus(bonus) {}

int Weapon::getAtkBonus() const { return atkBonus; }
void Weapon::setAtkBonus(int bonus) { atkBonus = bonus; }

bool Weapon::use(Player& p) {
    p.equipWeapon(this);
    return true;
}

string Weapon::getType() const { return "Weapon"; }

Consumable::Consumable() : Item("", 0, ""), hpRestore(0), atkBonusTemp(0), duration(0) {}
Consumable::Consumable(string n, int hp, int atk, int dur, int p)
    : Item(std::move(n), p, "消耗品"), hpRestore(hp), atkBonusTemp(atk), duration(dur) {}

int Consumable::getHpRestore() const { return hpRestore; }
int Consumable::getAtkBonusTemp() const { return atkBonusTemp; }
int Consumable::getDuration() const { return duration; }

void Consumable::setHpRestore(int hp) { hpRestore = hp; }
void Consumable::setAtkBonusTemp(int atk) { atkBonusTemp = atk; }
void Consumable::setDuration(int dur) { duration = dur; }

bool Consumable::use(Player& p) {
    if (p.getHp() + hpRestore > p.getMaxHp()) {
        p.setHp(p.getMaxHp());
    } else {
        p.setHp(p.getHp() + hpRestore);
    }
    p.setAtkBase(p.getAtkBase() + atkBonusTemp);
    return true;
}

string Consumable::getType() const { return "Consumable"; }
