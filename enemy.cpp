#include "mud_common.h"

#include <algorithm>

Enemy::Enemy() : Character("野怪", 30, 6, 0), dropGold(5), expValue(10), level(1) {}

Enemy::Enemy(string n, int mh, int atk, int dropG, int exp)
    : Character(std::move(n), mh, atk, 0), dropGold(dropG), expValue(exp), level(1) {}

Enemy::~Enemy() {
    for (Item* item : dropItems) {
        delete item;
    }
    dropItems.clear();
}

int Enemy::getDropGold() const { return dropGold; }
int Enemy::getExpValue() const { return expValue; }
const vector<Item*>& Enemy::getDropItems() const { return dropItems; }
int Enemy::getLevel() const { return level; }

void Enemy::setDropGold(int gold) { dropGold = std::max(0, gold); }
void Enemy::setExpValue(int exp) { expValue = std::max(0, exp); }
void Enemy::setLevel(int l) { level = std::max(1, l); }

void Enemy::enemyAttack(Player& p) {
    p.takeDamage(atkBase);
}

void Enemy::generateDrops() {
    dropItems.push_back(new Consumable("治疗药水", 20, 0, 0, 10));
    if (rand() % 2 == 0) {
        dropItems.push_back(new Weapon("铁匕首", 3, 15));
    }
}

void Enemy::addDropItem(Item* item) {
    if (item != nullptr) {
        dropItems.push_back(item);
    }
}
