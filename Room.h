#pragma once
#include<map>
#include<string>
#include<memory>
#include<vector>
using namespace std;

class Item;
class Npc;
class Enemy;
class Room;

class World{
private:
    map<int, shared_ptr<Room>> rooms;
    int currentRoomId;
public:
    World();
    bool movePlayer(const string& dir);
    shared_ptr<Room> getCurrentRoom();
    shared_ptr<Room> getRoomById(int id);
    void setCurrentRoom(int id);
    int getCurrentRoomId();
    map<int, shared_ptr<Room>>& getAllRooms();
    map<int, shared_ptr<Room>>& loadRooms(map<int, shared_ptr<Room>>& roomData);
};

class Room{
private:
    int id;
    string name;
    string description;
    map<string, int> exits;
public:
    Room(int id, const string& name, const string& description);
    virtual ~Room();
    void onEnter();
    map<string, int>& getExits();
    string getName(); 
};

class shopRoom : public Room {
private:
    vector<shared_ptr<Item>> goods;
public:
    shopRoom(int id, const string& name, const string& description);
    const vector<shared_ptr<Item>>& getGoods();
    void addGood(shared_ptr<Item> good);
};

class npcRoom : public Room {
private:
    vector<shared_ptr<Npc>> npcs;
public:
    npcRoom(int id, const string& name, const string& description);
    const vector<shared_ptr<Npc>>& getNpcs();
    void addNPC(shared_ptr<Npc> npc);
};

class combatRoom : public Room {
private:
    vector<shared_ptr<Enemy>> enemy;
public:
    combatRoom(int id, const string& name, const string& description);
    const vector<shared_ptr<Enemy>>& getEnemy();
    const vector<shared_ptr<Enemy>>& getCurrentEnemy();
    void addEnemy(shared_ptr<Enemy> enemy);
};
