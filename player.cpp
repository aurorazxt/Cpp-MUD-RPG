#include "MUD_GAME.h"

#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

Character::Character() : name(""), hp(0), maxHp(0), atkBase(0), money(0) {}

Character::Character(string characterName, int characterHp, int attack, int characterMoney)
    : name(characterName), hp(max(0, characterHp)), maxHp(max(0, characterHp)),
      atkBase(max(0, attack)), money(max(0, characterMoney)) {}

Character::~Character() = default;

string Character::getName() { return name; }
int Character::getHp() { return hp; }
int Character::getMaxHp() { return maxHp; }
int Character::getAtkBase() { return atkBase; }
int Character::getMoney() { return money; }

void Character::setName(string characterName) { name = characterName; }
void Character::setHp(int value) { hp = max(0, min(value, maxHp)); }
void Character::setMaxHp(int value)
{
    maxHp = max(0, value);
    hp = min(hp, maxHp);
}
void Character::setAtkBase(int value) { atkBase = max(0, value); }
void Character::setMoney(int value) { money = max(0, value); }

void Character::showStates(ColorCtrl color)
{
    color.applyInfo();
    cout << "名称: " << name << '\n'
         << "生命: " << hp << '/' << maxHp << '\n'
         << "攻击: " << getTotalAtk() << '\n'
         << "金钱: " << money << '\n';
    color.resetColor();
}

void Character::takeDamage(int damage)
{
    if (damage > 0)
        hp = max(0, hp - damage);
}

void Character::heal(int amount)
{
    if (amount > 0)
        hp = min(maxHp, hp + amount);
}

bool Character::isAlive() { return hp > 0; }
int Character::getTotalAtk() { return atkBase; }

Weapon::Weapon() : atkBonus(0)
{
    name = "";
    price = 0;
    description = "";
}

Weapon::Weapon(string weaponName, int bonus, int weaponPrice)
    : atkBonus(max(0, bonus))
{
    name = weaponName;
    price = max(0, weaponPrice);
    description = "";
}

bool Weapon::use(Player& player)
{
    player.m_equipWeapon(this);
    const vector<Item*> inventory = player.getInventory();
    return find(inventory.begin(), inventory.end(), this) != inventory.end();
}

string Weapon::getType() { return "Weapon"; }

Consumable::Consumable()
    : Restore(0), atkBounsTemp(0), duration(0)
{
    name = "";
    price = 0;
    description = "";
}

Consumable::Consumable(string itemName, int restoreHp, int tempAtk, int itemPrice)
    : Restore(max(0, restoreHp)), atkBounsTemp(max(0, tempAtk)), duration(1)
{
    name = itemName;
    price = max(0, itemPrice);
    description = "";
}

bool Consumable::use(Player& player)
{
    player.heal(Restore);
    if (atkBounsTemp > 0)
        player.setAtkBase(player.getAtkBase() + atkBounsTemp);
    return true;
}

string Consumable::getType() { return "Consumable"; }

Player::Player()
    : Character("玩家", 100, 10, 100), sp(50), maxSp(50), level(1), exp(0),
    expToNextLevel(100), equipWeapon(nullptr), currentRoomId("0") {}

Player::~Player()
{
    for (Item* item : inventory)
        delete item;
    inventory.clear();
    equipWeapon = nullptr;
}

int Player::getSp() { return sp; }
int Player::getMaxSp() { return maxSp; }
int Player::getLevel() { return level; }
int Player::getExp() { return exp; }
int Player::getExpToNextLevel() { return expToNextLevel; }

Weapon Player::getEquipWeapon()
{
    return equipWeapon == nullptr ? Weapon() : *equipWeapon;
}

const vector<Item*> Player::getInventory() { return inventory; }
string Player::getCurrentRoomId() { return currentRoomId; }
int Player::getInventorySize() { return static_cast<int>(inventory.size()); }

void Player::setSp(int value) { sp = max(0, min(value, maxSp)); }
void Player::setMaxSp(int value)
{
    maxSp = max(0, value);
    sp = min(sp, maxSp);
}
void Player::setLevel(int value) { level = max(1, value); }
void Player::setExp(int value) { exp = max(0, value); }
void Player::setExpToNextLevel(int value) { expToNextLevel = max(1, value); }
void Player::setCurrentRoomId(string id) { currentRoomId = id; }

int Player::getTotalAtk()
{
    return atkBase + (equipWeapon == nullptr ? 0 : equipWeapon->atkBonus);
}

void Player::normalAttack(Character& character)
{
    if (isAlive())
        character.takeDamage(getTotalAtk());
}

void Player::skillAttack(Character& character, int skillId)
{
    if (skillId <= 0)
        return;

    int spCost = max(1, skillId * 10);
    if (!isAlive() || sp < spCost)
        return;

    sp -= spCost;
    character.takeDamage(getTotalAtk() * 2);
}

bool Player::addItem(Item* item)
{
    if (item == nullptr || isInventoryFull())
        return false;
	if (find(inventory.begin(), inventory.end(), item) != inventory.end())
		return false;
    inventory.push_back(item);
    return true;
}

bool Player::removeItem(int index)
{
    if (index < 0 || index >= static_cast<int>(inventory.size()))
        return false;

    Item* item = inventory[index];
    if (item == equipWeapon)
        equipWeapon = nullptr;
    delete item;
    inventory.erase(inventory.begin() + index);
    return true;
}

Item* Player::getItem(int index)
{
    if (index < 0 || index >= static_cast<int>(inventory.size()))
        return nullptr;
    return inventory[index];
}

void Player::m_equipWeapon(Weapon* weapon)
{
    if (weapon == nullptr)
        return;

    auto found = find(inventory.begin(), inventory.end(), weapon);
    if (found != inventory.end() && dynamic_cast<Weapon*>(*found) != nullptr)
        equipWeapon = weapon;
}

void Player::unequipWeapon() { equipWeapon = nullptr; }

void Player::gainExp(int amount)
{
    if (amount <= 0)
        return;

    exp += amount;
    while (exp >= expToNextLevel)
        levelUp();
}

void Player::levelUp()
{
    if (exp < expToNextLevel)
        return;

    exp -= expToNextLevel;
    ++level;
    expToNextLevel = level * 100;
    maxHp += 20;
    maxSp += 10;
    atkBase += 5;
    hp = maxHp;
    sp = maxSp;
}

bool Player::isInventoryFull() { return inventory.size() >= 20; }

void Player::consumeItem(int index)
{
    Item* item = getItem(index);
    if (item == nullptr || item->getType() != "Consumable")
        return;

    if (item->use(*this))
        removeItem(index);
}

bool Player::savePlayerToFile(const string& path)
{
    ofstream output(path);
    if (!output)
        return false;

    int equippedIndex = -1;
    for (int index = 0; index < static_cast<int>(inventory.size()); ++index) {
        if (inventory[index] == equipWeapon) {
            equippedIndex = index;
            break;
        }
    }

    output << "MUD_PLAYER_V1\n"
           << quoted(name) << ' ' << hp << ' ' << maxHp << ' ' << atkBase << ' '
           << money << ' ' << sp << ' ' << maxSp << ' ' << level << ' ' << exp << ' '
           << expToNextLevel << ' ' << quoted(currentRoomId) << ' '
           << equippedIndex << '\n'
           << inventory.size() << '\n';

    for (Item* item : inventory) {
        if (item == nullptr)
            return false;

        output << quoted(item->getType()) << ' ' << quoted(item->name) << ' '
               << item->price << ' ' << quoted(item->description);

        if (Weapon* weapon = dynamic_cast<Weapon*>(item)) {
            output << ' ' << weapon->atkBonus;
        } else if (Consumable* consumable = dynamic_cast<Consumable*>(item)) {
            output << ' ' << consumable->Restore << ' '
                   << consumable->atkBounsTemp << ' ' << consumable->duration;
        } else {
            return false;
        }
        output << '\n';
    }

    return static_cast<bool>(output);
}

bool Player::loadPlayerFromFile(const string& path)
{
    ifstream input(path);
    if (!input)
        return false;

    string header;
    if (!(input >> header) || header != "MUD_PLAYER_V1")
        return false;

    string loadedName;
    string loadedRoom;
    int loadedHp;
    int loadedMaxHp;
    int loadedAtk;
    int loadedMoney;
    int loadedSp;
    int loadedMaxSp;
    int loadedLevel;
    int loadedExp;
    int loadedExpToNextLevel;
    int loadedEquippedIndex;
    size_t itemCount;

    if (!(input >> quoted(loadedName) >> loadedHp >> loadedMaxHp >> loadedAtk
          >> loadedMoney >> loadedSp >> loadedMaxSp >> loadedLevel >> loadedExp
          >> loadedExpToNextLevel >> quoted(loadedRoom)
          >> loadedEquippedIndex >> itemCount)) {
        return false;
    }

    if (loadedMaxHp < 0 || loadedHp < 0 || loadedMaxHp < loadedHp ||
        loadedAtk < 0 || loadedMoney < 0 || loadedMaxSp < 0 || loadedSp < 0 ||
        loadedSp > loadedMaxSp || loadedLevel < 1 || loadedExp < 0 ||
        loadedExpToNextLevel < 1 || loadedEquippedIndex < -1 ||
        itemCount > 20 ||
        (loadedEquippedIndex >= 0 &&
         static_cast<size_t>(loadedEquippedIndex) >= itemCount)) {
        return false;
    }

    vector<Item*> loadedInventory;
    loadedInventory.reserve(itemCount);

    for (size_t index = 0; index < itemCount; ++index) {
        string type;
        string itemName;
        string itemDescription;
        int itemPrice;

        if (!(input >> quoted(type) >> quoted(itemName) >> itemPrice
              >> quoted(itemDescription)) || itemPrice < 0) {
            for (Item* item : loadedInventory)
                delete item;
            return false;
        }

        Item* item = nullptr;
        if (type == "Weapon") {
            int attackBonus;
            if (!(input >> attackBonus) || attackBonus < 0) {
                for (Item* loadedItem : loadedInventory)
                    delete loadedItem;
                return false;
            }
            Weapon* weapon = new Weapon(itemName, attackBonus, itemPrice);
            weapon->description = itemDescription;
            item = weapon;
        } else if (type == "Consumable") {
            int restore;
            int temporaryAttack;
            int duration;
            if (!(input >> restore >> temporaryAttack >> duration) ||
                restore < 0 || temporaryAttack < 0 || duration < 0) {
                for (Item* loadedItem : loadedInventory)
                    delete loadedItem;
                return false;
            }
            Consumable* consumable = new Consumable(
                itemName, restore, temporaryAttack, itemPrice);
            consumable->description = itemDescription;
            consumable->duration = duration;
            item = consumable;
        } else {
            for (Item* loadedItem : loadedInventory)
                delete loadedItem;
            return false;
        }
        loadedInventory.push_back(item);
    }

    for (Item* item : inventory)
        delete item;
    inventory.swap(loadedInventory);
    for (Item* item : loadedInventory)
        delete item;

    name = loadedName;
    hp = loadedHp;
    maxHp = loadedMaxHp;
    atkBase = loadedAtk;
    money = loadedMoney;
    sp = loadedSp;
    maxSp = loadedMaxSp;
    level = loadedLevel;
    exp = loadedExp;
    expToNextLevel = loadedExpToNextLevel;
    currentRoomId = loadedRoom;
    equipWeapon = loadedEquippedIndex < 0
        ? nullptr
        : dynamic_cast<Weapon*>(inventory[loadedEquippedIndex]);

    return true;
}