#ifndef MUD_GAME_H
#define MUD_GAME_H
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include<memory>
#include<map>
using namespace std;

//色彩控制
class ColorCtrl
{
public:
	int textColor;
	int optionColor;
	int infoColor;
	ColorCtrl();
	void setTheme(int themeId);
	void applyText();
	void applyOption();
	void applyInfo();
	void resetColor();
};

//角色基类
class Character {
protected:
	string name;
	int hp;
	int maxHp;
	int atkBase;
	int money;
public:
	Character();
	Character(string name, int hp, int atk, int d);
	virtual ~Character();
	string getName();
	int getHp();
	int getMaxHp();
	int getAtkBase();
	int getMoney();
	void setName(string name);
	void setHp(int hp);
	void setMaxHp(int hp);
	void setAtkBase(int atk);
	void setMoney(int money);
	virtual void showStates(ColorCtrl color);
	virtual void takeDamage(int damage);
	virtual void heal(int heal);
	bool isAlive();
	virtual int getTotalAtk();
};
class Player;
//物品基类
class Item {
public:
	string name;
	int price;
	string description;
	virtual bool use(Player& player) = 0;
	virtual string getType() = 0;
	virtual ~Item() = default;
};

//武器
class Weapon :public Item
{
public:
	int atkBonus;
	Weapon();
	Weapon(string n, int bonus, int p);
	bool use(Player& player)override;
	string getType()override;
};

class Consumable :public Item {
public:
	Consumable();
	Consumable(string n, int restoreHp, int tempAtk, int p);
	int Restore;
	int atkBounsTemp;
	int duration;
	bool use(Player& player)override;
	string getType()override;
};

//Player
class Player :public Character {
private:
	int sp;
	int maxSp;
	int level;
	int exp;
	int expToNextLevel;
	Weapon* equipWeapon;
	vector<Item*>inventory;
	string currentRoomId;
public:
	Player();
	~Player();
	int getSp();
	int getMaxSp();
	int getLevel();
	int getExp();
	int getExpToNextLevel();
	Weapon getEquipWeapon();
	const vector <Item*> getInventory();
	string getCurrentRoomId();
	int getInventorySize();
	void setSp(int sp);
	void setMaxSp(int sp);
	void setLevel(int lv);
	void setExp(int ex);
	void setExpToNextLevel(int ex);
	void setCurrentRoomId(string id);
	int getTotalAtk()override;
	void normalAttack(Character& cha);
	void skillAttack(Character& cha, int skillId);
	bool addItem(Item* item);
	bool removeItem(int idx);
	Item* getItem(int idx);
	void m_equipWeapon(Weapon* wea);
	void unequipWeapon();
	void gainExp(int exp);
	void levelUp();
	bool isInventoryFull();
	void consumeItem(int idx);
	bool savePlayerToFile(const string& path);
	bool loadPlayerFromFile(const string& path);
};

// enemy
class Enemy :public Character {
private:
	int DropGold;
	int expvalue;
	vector<Item>dropItems;
	int level;
public:
	Enemy();
	void enemyAttack(Player& player);
	void generateDrops();
	int getDropGold();
	int getExpValue();
};

class Npc :public Character {
protected:
	vector<string>talkOptions;
	int hiddenHp;
	Item* rewardItem;
	bool isHelped;
public:
	Npc();
	Npc(string npcName);
	~Npc();
	int getHiddenHp();
	Item* getRewardItem();
	bool getIsHelped();
	const vector<string> getTalkOptions();
	void setHiddenHp(int hp);
	void setRewardItem(Item* item);
	void setIsHelped(bool ishelped);
	void doTalk(ColorCtrl& color);
	void addTalkOption(string option);
	virtual void onHelp(Player& player);
	virtual void onAttack(Player& player);
};

//World
class Room;
class World {
private:
	map<int, shared_ptr<Room>> rooms;
	int currentRoomId;
public:
	World();
	bool movePlayer(const string& dir);
	shared_ptr<Room> getCurrentRoom();
	shared_ptr<Room> getRoomById(int id);
	int getCurrentRoomId();
	void setCurrentRoom(int id);
	map<int, shared_ptr<Room>>& getAllRooms();
	map<int, shared_ptr<Room>>& loadRooms(map<int, shared_ptr<Room>>& roomData);
};

//房间
class Room {
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


//具体房间
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

//商人
class MerchantNpc :public Npc {
	vector<Weapon>shopWeapon;
	vector<Consumable>shopConsumable;
public:
	MerchantNpc(string name);
	const vector <Weapon>getShopWeapons();
	const vector<Consumable>getShopConsumable();
	void AddshopWeapon(Weapon& wea);
	void addShopConsumable(Consumable& con);
	void showShop(ColorCtrl& color);
	bool buyWeapon(Player& player, int idx);
	bool buyConsumable(Player& player, int idx);
	int	getWeaponPrice(int idx);
	int getConsumablePrice(int idx);
};

//总控制器
class MudGame
{
public:
	Player player;
	World world;
	ColorCtrl color;
	MudGame();
	~MudGame();
	void drawCharMap(ColorCtrl& col);
	void selectColorTheme();
	void initWorld();
	shared_ptr<Room> findRoomById(int id);
	void showMainUI();
	void parseMainCmd(int opt);
	void battle(Player& me, Enemy& en);
	void run();
	bool saveGame();
	bool loadGame();
};

#endif