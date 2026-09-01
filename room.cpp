#include "mud_common.h"

#include <algorithm>

Room::Room() : id(0), name(""), description("") {}
Room::Room(int id_, string name_, string desc_) : id(id_), name(std::move(name_)), description(std::move(desc_)) {}

int Room::getId() const { return id; }
string Room::getName() const { return name; }
string Room::getDescription() const { return description; }
map<string, int> Room::getExits() const { return exits; }

void Room::setId(int id_) { id = id_; }
void Room::setName(const string& n) { name = n; }
void Room::setDescription(const string& desc) { description = desc; }

void Room::onEnter(Player& p) { p.setCurrentRoomId(std::to_string(id)); }

void Room::showInfo(ColorCtrl& col) {
    col.applyInfo();
    std::cout << "[房间] " << name << "\n" << description << "\n";
    if (!exits.empty()) {
        std::cout << "通道：";
        for (const auto& [direction, target] : exits) {
            std::cout << direction << "(" << target << ") ";
        }
        std::cout << std::endl;
    }
    col.resetColor();
}

bool Room::hasExit(const string& direction) const { return exits.find(direction) != exits.end(); }
int Room::getExitTarget(const string& direction) const {
    auto it = exits.find(direction);
    return it == exits.end() ? -1 : it->second;
}
void Room::addExit(const string& direction, int targetRoomId) { exits[direction] = targetRoomId; }
void Room::removeExit(const string& direction) { exits.erase(direction); }

CombatRoom::CombatRoom() : Room() {}
CombatRoom::CombatRoom(int id_, string name_, string desc_) : Room(id_, std::move(name_), std::move(desc_)) {}

vector<shared_ptr<Enemy>>& CombatRoom::getEnemies() { return enemies; }
size_t CombatRoom::getEnemyCount() const { return enemies.size(); }

void CombatRoom::onEnter(Player& p) {
    Room::onEnter(p);
    if (!enemies.empty()) {
        std::cout << "这里有 " << enemies.size() << " 个敌人盘踞着。" << std::endl;
    }
}

void CombatRoom::showInfo(ColorCtrl& col) {
    Room::showInfo(col);
    if (!enemies.empty()) {
        std::cout << "敌人：";
        for (const auto& enemy : enemies) {
            if (enemy && enemy->isAlive()) {
                std::cout << enemy->getName() << "(" << enemy->getHp() << ") ";
            }
        }
        std::cout << std::endl;
    }
}

void CombatRoom::addEnemy(shared_ptr<Enemy> e) { if (e) enemies.push_back(e); }
bool CombatRoom::hasEnemies() const {
    for (const auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            return true;
        }
    }
    return false;
}
void CombatRoom::clearDeadEnemies() {
    auto it = std::remove_if(enemies.begin(), enemies.end(), [](const shared_ptr<Enemy>& e) {
        return e == nullptr || !e->isAlive();
    });
    enemies.erase(it, enemies.end());
}
Enemy* CombatRoom::getFirstAliveEnemy() {
    for (auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            return enemy.get();
        }
    }
    return nullptr;
}

NpcRoom::NpcRoom() : Room() {}
NpcRoom::NpcRoom(int id_, string name_, string desc_) : Room(id_, std::move(name_), std::move(desc_)) {}

vector<shared_ptr<Npc>>& NpcRoom::getNpcs() { return npcs; }
size_t NpcRoom::getNpcCount() const { return npcs.size(); }

void NpcRoom::onEnter(Player& p) {
    Room::onEnter(p);
    if (!npcs.empty()) {
        std::cout << "这里的居民在向你打招呼：" << std::endl;
        for (const auto& npc : npcs) {
            if (npc) {
                std::cout << "- " << npc->getName() << std::endl;
            }
        }
    }
}

void NpcRoom::showInfo(ColorCtrl& col) {
    Room::showInfo(col);
    if (!npcs.empty()) {
        std::cout << "NPC：";
        for (const auto& npc : npcs) {
            if (npc) {
                std::cout << npc->getName() << " ";
            }
        }
        std::cout << std::endl;
    }
}

void NpcRoom::addNpc(shared_ptr<Npc> n) { if (n) npcs.push_back(n); }
Npc* NpcRoom::getNpcByName(const string& name) {
    for (const auto& npc : npcs) {
        if (npc && npc->getName() == name) {
            return npc.get();
        }
    }
    return nullptr;
}
Npc* NpcRoom::getNpcByIndex(int index) {
    if (index < 0 || index >= static_cast<int>(npcs.size())) {
        return nullptr;
    }
    return npcs[index].get();
}

ShopRoom::ShopRoom() : Room(), shopkeeper(nullptr) {}
ShopRoom::ShopRoom(int id_, string name_, string desc_) : Room(id_, std::move(name_), std::move(desc_)), shopkeeper(nullptr) {}
ShopRoom::~ShopRoom() { shopkeeper = nullptr; }

const vector<Weapon>& ShopRoom::getShopWeapons() const { return shopWeapons; }
const vector<Consumable>& ShopRoom::getShopConsumables() const { return shopConsumables; }
MerchantNpc* ShopRoom::getShopkeeper() const { return shopkeeper; }
void ShopRoom::setShopkeeper(MerchantNpc* npc) { shopkeeper = npc; }

void ShopRoom::onEnter(Player& p) {
    Room::onEnter(p);
    if (shopkeeper) {
        std::cout << "店主 " << shopkeeper->getName() << " 正在招呼你。" << std::endl;
    }
}

void ShopRoom::showInfo(ColorCtrl& col) {
    Room::showInfo(col);
    if (shopkeeper) {
        shopkeeper->showShop(col);
    }
}

void ShopRoom::addWeapon(const Weapon& w) { shopWeapons.push_back(w); }
void ShopRoom::addConsumable(const Consumable& c) { shopConsumables.push_back(c); }
bool ShopRoom::buyItem(Player& p, int type, int idx) {
    if (type == 1) {
        return shopkeeper != nullptr && shopkeeper->buyWeapon(p, idx);
    }
    if (type == 2) {
        return shopkeeper != nullptr && shopkeeper->buyConsumable(p, idx);
    }
    return false;
}
int ShopRoom::getWeaponCount() const { return static_cast<int>(shopWeapons.size()); }
int ShopRoom::getConsumableCount() const { return static_cast<int>(shopConsumables.size()); }

NormalRoom::NormalRoom() : Room() {}
NormalRoom::NormalRoom(int id_, string name_, string desc_) : Room(id_, std::move(name_), std::move(desc_)) {}
void NormalRoom::onEnter(Player& p) { Room::onEnter(p); }
