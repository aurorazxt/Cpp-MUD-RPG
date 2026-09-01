#pragma once
#include<iostream>
#include<map>
#include<string>
#include<memory>
#include<vector>
using namespace std;

class item;
class Npc;
class Enemy;

class World{
private:
    map<int, shared_ptr<Room>> rooms;
    int currentRoomId;
public:
    World();
    bool movePlayer(const string& dir);
    shared_ptr<Room> getCurrentRoom();
    shared_ptr<Room> getRoomById(int id);
    int getCurrentRoomId();
    void setCurrentRoom();
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
    void onEnter();
    map<string, int>& getExits();
    string getName(); 
};

class shopRoom : public Room {
private:
    vector<shared_ptr<item>> goods;
public:
    shopRoom(int id, const string& name, const string& description);
    void onEnter();
    const vector<shared_ptr<item>>& getGoods();
    void addGood(shared_ptr<item> good);
    //void addNPC(shared_ptr<Npc> npc);
};

class npcRoom : public Room {
private:
    vector<shared_ptr<Npc>> npcs;
public:
    npcRoom(int id, const string& name, const string& description);
    void onEnter() ;
    const vector<shared_ptr<Npc>>& getNpcs();
    void addNPC(shared_ptr<Npc> npc);
};

class combatRoom : public Room {
private:
    vector<shared_ptr<Enemy>> enemy;
public:
    combatRoom(int id, const string& name, const string& description);
    void onEnter() ;
    const vector<shared_ptr<Enemy>>& getEnemy();
    const vector<shared_ptr<Enemy>>& getCurrentEnemy();
    void addEnemy(shared_ptr<Enemy> enemy);
};
