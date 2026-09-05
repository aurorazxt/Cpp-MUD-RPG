#include "mud_common.h"

#include <algorithm>

Character::Character() : name(""), hp(0), maxHp(0), atkBase(0), money(0) {}

Character::Character(string n, int mh, int atk, int mon)
    : name(std::move(n)), hp(mh), maxHp(mh), atkBase(atk), money(mon) {}

string Character::getName() const { return name; }
int Character::getHp() const { return hp; }
int Character::getMaxHp() const { return maxHp; }
int Character::getAtkBase() const { return atkBase; }
int Character::getMoney() const { return money; }

void Character::setName(const string& n) { name = n; }
void Character::setHp(int h) { hp = std::max(0, h); }
void Character::setMaxHp(int mh) { maxHp = std::max(0, mh); }
void Character::setAtkBase(int atk) { atkBase = std::max(0, atk); }
void Character::setMoney(int mon) { money = std::max(0, mon); }

void Character::showStatus(ColorCtrl& col) {
    col.applyInfo();
    std::cout << "[角色] " << name << " HP: " << hp << "/" << maxHp
              << " ATK: " << atkBase << " 金币: " << money << std::endl;
    col.resetColor();
}

void Character::takeDamage(int dmg) {
    if (dmg <= 0) {
        return;
    }
    hp = std::max(0, hp - dmg);
}

void Character::heal(int amount) {
    if (amount <= 0) {
        return;
    }
    hp = std::min(maxHp, hp + amount);
}

bool Character::isAlive() const { return hp > 0; }

int Character::getTotalAtk() { return atkBase; }
