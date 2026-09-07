#include<iostream>
#include<map>
#include<string>
#include<memory>
#include<vector>
#include "Room.h"
using namespace std;

//room类的函数定义
Room::Room(int id, const string& name, const string& description) : id(id), name(name), description(description) {}

Room::~Room() {};

void Room::onEnter() {
    cout << "你进入了： " << name << endl;
    cout << description << endl;
}

map<string, int>& Room::getExits() {
    return exits;
}

string Room::getName() {
    return name;
}

//shopRoom类的函数定义
shopRoom::shopRoom(int id, const string& name, const string& description) :
    Room(id, name, description) {}

const vector<shared_ptr<Item>>& shopRoom::getGoods() {
    return goods;
}

void shopRoom::addGood(shared_ptr<Item> good) {
    goods.push_back(good);//添加商品到商店
}


//npcRoom类的函数定义
npcRoom::npcRoom(int id, const string& name, const string& description) :
    Room(id, name, description) {}  

const vector<shared_ptr<Npc>>& npcRoom::getNpcs() {
    return npcs;
}

void npcRoom::addNPC(shared_ptr<Npc> npc) {
    npcs.push_back(npc);//添加NPC到房间
}   

//combatRoom类的函数定义
combatRoom::combatRoom(int id, const string& name, const string& description) :
    Room(id, name, description) {}  

const vector<shared_ptr<Enemy>>& combatRoom::getEnemy() {
    return enemy;
}

const vector<shared_ptr<Enemy>>& combatRoom::getCurrentEnemy() {
    return enemy;
}

void combatRoom::addEnemy(shared_ptr<Enemy> enemy) {
    this->enemy.push_back(enemy);//添加敌人到战斗房间
}

//World类的函数定义
World::World() {
    rooms[0] = make_shared<Room>(0, "回声公寓", "这里是你赛博世界一切的起点。");
    rooms[1] = make_shared<npcRoom>(1, "废弃医院", "这是一个已经荒废了的医院，某日，异体人攻占了这里。这里似乎还有其它人的存在。");
    rooms[2] = make_shared<combatRoom>(2, "幽灵基站", "一听就不是什么好名字，似乎能嗅到危险的气息。");
    rooms[3] = make_shared<combatRoom>(3, "废铁猎场", "这里是新的赛博勇士大显身手的好地方，不过前提是你能打败这里的怪物。");
    rooms[4] = make_shared<combatRoom>(4, "矿场塌陷区", "这里曾经是繁荣的矿区，人们的贪婪导致这里的塌陷，不过好在没有造成无辜者的伤亡。");
    rooms[5] = make_shared<npcRoom>(5, "数据酒吧", "灯红酒绿的氛围让人忘记了门外的危险，人们聚集在这交流信息。");
    rooms[6] = make_shared<combatRoom>(6, "蜘蛛巢穴", "无数战士的向往之地，来到这里，你，真的够格了吗？");
    rooms[7]= make_shared<shopRoom>(7, "霓虹市集", "和酒吧一个风格，这里有很多宝物使你变得更强。");

    for (auto& room : rooms) {
        if (room.first != 7) {
            room.second->getExits()["shop"] = 7;//任何房间都可以去商店，商店通过Id到其他房间，在主程序实现
        }
    }

    rooms[0]->getExits()["right"] = 2;
    rooms[0]->getExits()["up"] = 1;
    rooms[1]->getExits()["left"] = 0;
    rooms[1]->getExits()["up"] = 3;
    rooms[2]->getExits()["down"] = 0;
    rooms[2]->getExits()["right"] = 3;
    rooms[3]->getExits()["up"] = 4;
    rooms[3]->getExits()["down"] = 1;
    rooms[3]->getExits()["right"] = 5;
    rooms[3]->getExits()["left"] = 2;
    rooms[4]->getExits()["down"] = 3;
    rooms[4]->getExits()["right"] = 6;
    rooms[5]->getExits()["left"] = 3;
    rooms[5]->getExits()["up"] = 6;
    rooms[6]->getExits()["down"] = 5;
    rooms[6]->getExits()["left"] = 4;
    currentRoomId = 0;
}

bool World::movePlayer(const string& dir) {
    auto currentRoom = getCurrentRoom();
    if (!currentRoom) {
        cout << "当前房间不存在！" << endl;
        return false;
    }

    auto& exits = currentRoom->getExits();
    auto exit = exits.find(dir);

    if (exit == exits.end()) {
        cout << dir << " 方向没有路了。" << endl;
        return false;
    }

    if (rooms.find(exit->second) == rooms.end()) {
        cout << "目标房间不存在！" << endl;
        return false;
    }

    currentRoomId = exit->second;
    return true;
}

shared_ptr<Room> World::getCurrentRoom() {
    auto it = rooms.find(currentRoomId);
    if (it != rooms.end()) {
        return it->second;
    }   
    return nullptr;
}

shared_ptr<Room> World::getRoomById(int id) {
    if (rooms.find(id) != rooms.end()) {
        return rooms[id];
    }
    return nullptr;
}

int World::getCurrentRoomId() {
    return currentRoomId;
}

void World::setCurrentRoom(int id) {
    if (rooms.find(id) != rooms.end()) {
        currentRoomId = id; // 设置为指定房间的ID
    }
}

map<int, shared_ptr<Room>>& World::getAllRooms() {
    return rooms;
}

map<int, shared_ptr<Room>>& World::loadRooms(map<int, shared_ptr<Room>>& roomData) {
    rooms = roomData;

    for (auto& room : rooms) {
        if (room.first != 7) {
            room.second->getExits()["shop"] = 7;
        }
    }

    return rooms;
}


