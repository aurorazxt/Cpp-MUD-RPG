#include "mud_common.h"

#include <memory>

World::World() : currentRoomId(1), startRoomId(1) {}

int World::getCurrentRoomId() const { return currentRoomId; }
int World::getStartRoomId() const { return startRoomId; }
size_t World::getRoomCount() const { return rooms.size(); }
void World::setCurrentRoom(int id) { currentRoomId = id; }
void World::setStartRoomId(int id) { startRoomId = id; }

void World::addRoom(shared_ptr<Room> room) {
    if (room != nullptr) {
        rooms[room->getId()] = room;
    }
}
Room* World::getRoom(int id) {
    auto it = rooms.find(id);
    if (it == rooms.end()) {
        return nullptr;
    }
    return it->second.get();
}
Room* World::getCurrentRoom() { return getRoom(currentRoomId); }

bool World::movePlayer(const string& direction) {
    Room* room = getCurrentRoom();
    if (room == nullptr) {
        return false;
    }
    int target = room->getExitTarget(direction);
    if (target == -1 || !hasRoom(target)) {
        return false;
    }
    currentRoomId = target;
    return true;
}

map<int, shared_ptr<Room>>& World::getAllRooms() { return rooms; }

bool World::loadRooms(const string& filename) {
    (void)filename;
    return false;
}

void World::initDefaultWorld() {
    rooms.clear();
    auto town = make_shared<NormalRoom>(1, "村落广场", "晨雾中，你看到一座热闹的村落，路边的木屋和杂货摊子散发出生活气息。");
    auto forest = make_shared<CombatRoom>(2, "幽暗森林", "树林中有低沉的脚步声，危险正潜伏在树影之间。");
    auto market = make_shared<ShopRoom>(3, "小镇集市", "摊贩和材料商汇聚在这里，能买到各种武器和药品。");
    auto ruins = make_shared<NpcRoom>(4, "古遗址", "被风吹动的石墙之间，隐约能看到归来的神秘居民。");
    auto cave = make_shared<CombatRoom>(5, "洞穴深处", "潮湿且寒冷，四周是石壁与不安的声响。");

    town->addExit("north", 2);
    town->addExit("east", 3);
    forest->addExit("south", 1);
    forest->addExit("east", 4);
    market->addExit("west", 1);
    market->addExit("north", 5);
    ruins->addExit("west", 2);
    cave->addExit("south", 3);

    auto goblin = make_shared<Enemy>("洞穴哥布林", 35, 8, 12, 25);
    goblin->generateDrops();
    forest->addEnemy(goblin);

    auto wolf = make_shared<Enemy>("野狼", 45, 10, 18, 35);
    wolf->generateDrops();
    cave->addEnemy(wolf);

    auto elder = make_shared<Npc>("长老");
    elder->setHiddenHp(25);
    elder->setRewardItem(new Item("秘银护符", 0, "提升防御与运气"));
    ruins->addNpc(elder);

    auto merchant = make_shared<MerchantNpc>("商人阿林");
    merchant->setRewardItem(new Item("修理卷轴", 0, "能让你精修装备"));
    market->setShopkeeper(merchant.get());

    addRoom(town);
    addRoom(forest);
    addRoom(market);
    addRoom(ruins);
    addRoom(cave);

    currentRoomId = 1;
    startRoomId = 1;
}

bool World::hasRoom(int id) const { return rooms.find(id) != rooms.end(); }
void World::removeRoom(int id) { rooms.erase(id); }
