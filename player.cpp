#include "mud_common.h"

#include <algorithm>

Player::Player() : Character("勇者", 100, 12, 30), sp(20), maxSp(20), level(1), exp(0), expToNextLevel(50), equippedWeapon(nullptr), currentRoomId("1") {
    inventory.clear();
}

Player::~Player() {
    equippedWeapon = nullptr;
    for (Item* item : inventory) {
        delete item;
    }
    inventory.clear();
}

int Player::getSp() const { return sp; }
int Player::getMaxSp() const { return maxSp; }
int Player::getLevel() const { return level; }
int Player::getExp() const { return exp; }
int Player::getExpToNextLevel() const { return expToNextLevel; }
Weapon* Player::getEquipWeapon() const { return equippedWeapon; }
const vector<Item*>& Player::getInventory() const { return inventory; }
string Player::getCurrentRoomId() const { return currentRoomId; }
int Player::getInventorySize() const { return static_cast<int>(inventory.size()); }

void Player::setSp(int s) { sp = std::max(0, s); }
void Player::setMaxSp(int ms) { maxSp = std::max(0, ms); }
void Player::setLevel(int l) { level = std::max(1, l); }
void Player::setExp(int e) { exp = std::max(0, e); }
void Player::setExpToNextLevel(int exp) { expToNextLevel = std::max(1, exp); }
void Player::setCurrentRoomId(const string& id) { currentRoomId = id; }

int Player::getTotalAtk() {
    int bonus = equippedWeapon ? equippedWeapon->getAtkBonus() : 0;
    return atkBase + bonus;
}

void Player::normalAttack(Character& target) {
    target.takeDamage(getTotalAtk());
}

void Player::skillAttack(Character& target, int spCost) {
    if (sp < spCost) {
        return;
    }
    sp -= spCost;
    target.takeDamage(getTotalAtk() + level * 3);
}

bool Player::addItem(Item* item) {
    if (item == nullptr) {
        return false;
    }
    inventory.push_back(item);
    return true;
}

bool Player::removeItem(int index) {
    if (index < 0 || index >= static_cast<int>(inventory.size())) {
        return false;
    }
    delete inventory[index];
    inventory.erase(inventory.begin() + index);
    return true;
}

Item* Player::getItem(int index) const {
    if (index < 0 || index >= static_cast<int>(inventory.size())) {
        return nullptr;
    }
    return inventory[index];
}

void Player::equipWeapon(Weapon* w) {
    equippedWeapon = w;
    if (w != nullptr && std::find(inventory.begin(), inventory.end(), w) == inventory.end()) {
        inventory.push_back(w);
    }
}

void Player::unequipWeapon() {
    equippedWeapon = nullptr;
}

void Player::gainExp(int amount) {
    if (amount <= 0) {
        return;
    }
    exp += amount;
    while (exp >= expToNextLevel) {
        exp -= expToNextLevel;
        levelUp();
    }
}

void Player::levelUp() {
    level += 1;
    expToNextLevel = std::max(20, expToNextLevel + 20);
    maxHp += 20;
    hp = maxHp;
    atkBase += 4;
    maxSp += 4;
    sp = maxSp;
}

bool Player::isInventoryFull() const {
    return inventory.size() >= 10;
}

void Player::consumeItem(int index) {
    Item* item = getItem(index);
    if (item == nullptr) {
        return;
    }
    item->use(*this);
    removeItem(index);
}
