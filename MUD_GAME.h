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
	Character(string name, int hp, int akt, int d);
	virtual ~Character();
	string getName();
	int getHp();
	int getMaxHp();
	int getAtkBase();
	int getMoney();
	void setName(string name);
	void setHp(int hp);
	void setMaxHp(int hp);
	void setAtkBase(int stk);
	void setMoney(int money);
	virtual void showStates(ColorCtrl color);
	virtual void takeDamage(int damage);
	virtual void heal(int heal);
	bool isAlive();
	virtual int getTotalAtk();
};

class Player :public Character{};
// 物品基类
class Item {
public:
	string name;
	int price;
	string description;
	bool use(Player& player);
	string getType();
};
//武器
class Weapon:public Item
{
public:
	int atkBonus;
	Weapon();
	Weapon(string n, int bonus, int p);
};
class Consumable :public Item {
public:
	Consumable();
	Consumable(string n, int bonus, int p);
	int Restore;
	int atkBounsTemp;
	int duration;
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
	~Player();
	int getSp();
	int getMaxSp();
	int getLevel();
	int getExp();
	int getExpToNextLevel();
	Weapon getEquipWeapon();
	const vector <Item*> getInventory;
	string getCurrentRoomId();
	int getInventorySize();
	void setSp(int sp);
	void setMaxSp(int sp);
	void setLevel(int lv);
	void setExp(int ex);
	void setExpToNextLevel(int ex);
	void setCurrentRoomld(string);
	int getTotalAtk();
	void normalAttack(Character& cha);
	void skillAttack(Character& cha, int);
	bool additem(Item* item);
	bool removeltem(int move);
	Item getltem(int get);
	void equipWeapon(Weapon* wea);
	void unequipWeapon();
	void gainExp(int exp);
	void levelUp();
	bool isinventoryFull();
	void consumeltem(int con);
};
// enemy
class Enemy :public Character {
private:
	int DropGold;
	int expvalue;
	vector<Item>dropItems;
	int level;
public:
	void enemyAttack(Player& player);
	void generateDrops();
};
class Npc :public Character {
protected:
	vector<string>talkOptions;
	int hiddenHp;
	Item* rewardItem;
	bool isHelped;
public:
	Npc();
	Npc(string npc);
	~Npc();
	int getHiddenHp();
	Item getRewardItem();
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
	map<int,shared_ptr<Room>> rooms;
	int currentRoomid;
public:
	World();
	bool movePlayer(string dir);
	shared_ptr<Room> getCurrentRoom();
	shared_ptr<Room> getRoomById(int id);
	int getCurrentRoomId();
	void setCurrentRoomId();
	map<int, shared_ptr<Room>> getAllRooms();
	map<int, shared_ptr<Room>>& loadRooms(map<int, shared_ptr<Room>>& roomData);
};
//房间
class Room {
private:
	int Id;
	string name;
	string description;
	map<string, int> exit;
public:
	Room();
	void onEnter();
	map<string, int> getExits();
	string getDescription();
	string getName();
};
//具体房间
class shopRoom : public Room {
private:
	vector<shared_ptr<Item>> goods;
public:
	shopRoom(int id, const string& name, const string& description);
	void onEnter();
	const vector<shared_ptr<Item>>& getGoods();
	void addGood(shared_ptr<Item> good);
	//void addNPC(shared_ptr<Npc> npc);
};

class npcRoom : public Room {
private:
	vector<shared_ptr<Npc>> npcs;
public:
	npcRoom(int id, const string& name, const string& description);
	void onEnter();
	const vector<shared_ptr<Npc>>& getNpcs();
	void addNPC(shared_ptr<Npc> npc);
};

class combatRoom : public Room {
private:
	vector<shared_ptr<Enemy>> enemy;
public:
	combatRoom(int id, const string& name, const string& description);
	void onEnter();
	const vector<shared_ptr<Enemy>>& getEnemy();
	const vector<shared_ptr<Enemy>>& getCurrentEnemy();
	void addEnemy(shared_ptr<Enemy> enemy);
};
//商人
class MerchantNpc :public Npc {
	vector<Weapon>shopWeapon;
	vector<Consumable>shopConsumable;
public:
	MerchantNpc(string a);
	const vector <Weapon>getShopWeapons();
	const vector<Consumable>getShopConsumable();
	void AddshopWeapon(Weapon& wea);
	void addShopConsumable(Consumable& con);
	void showShop(ColorCtrl& color);
	bool buyWeapon(Player& player, int mon);
	bool buyConsumable(Player& player, int mon);
	int	tgetWeaponPrice(int mon);
	int getConsumablePrice(int mon);
};

//总控制器
class MudGame
{
public:
	Player player;
	vector<Room> world;
	ColorCtrl color;

	MudGame();
	~MudGame(); //释放new出来的Npc内存，防止内存泄漏
	void drawCharMap(ColorCtrl& col);//展示游戏地图
	void selectColorTheme();//色彩主题
	void initWorld();//房间信息
	Room* findRoomByName(const string& name);//房间移动
	void showMainUI();//展示主菜单
	void parseMainCmd(int opt);//选项调用
	void battle(Player& me, Enemy& en);//攻击交互
	void run();//游戏运行
};
#endif