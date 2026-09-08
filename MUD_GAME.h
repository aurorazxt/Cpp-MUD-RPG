#ifndef MUD_GAME_H
#define MUD_GAME_H

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

// ==================== 色彩控制 ====================
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

// ==================== 角色基类 ====================
class Character
{
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

// ==================== 物品基类 ====================
class Item
{
public:
	string id;          // 唯一编号，用于掉落表和存档
	string name;
	int price = 0;
	string description;

	virtual bool use(Player& player) = 0;
	virtual string getType() = 0;
	virtual ~Item() = default;
};

// ==================== 武器 ====================
class Weapon : public Item
{
public:
	int atkBonus = 0;

	Weapon();
	Weapon(string n, int bonus, int p);

	Weapon(
		string itemId,
		string n,
		int bonus,
		int p,
		string desc
	);

	bool use(Player& player) override;
	string getType() override;
};

// ==================== 消耗品 ====================
class Consumable : public Item
{
public:
	int Restore = 0;

	// 暂时保留原来的变量名，避免已有代码失效
	int atkBounsTemp = 0;

	int duration = 0;

	Consumable();

	Consumable(
		string n,
		int restoreHp,
		int tempAtk,
		int p
	);

	Consumable(
		string itemId,
		string n,
		int restoreHp,
		int tempAtk,
		int effectDuration,
		int p,
		string desc
	);

	bool use(Player& player) override;
	string getType() override;

	int getTempAtkBonus() const
	{
		return atkBounsTemp;
	}
};

// 根据物品ID创建新物品
// 返回的指针加入玩家背包后，由Player负责释放
class ItemFactory
{
public:
	static Item* createItem(const string& itemId);
};

// 敌人掉落表中的一项
// dropRate取值范围为0～100
struct DropEntry
{
	string itemId;
	int dropRate;

	DropEntry(const string& id = "", int rate = 0)
		: itemId(id),
		  dropRate(rate < 0 ? 0 : (rate > 100 ? 100 : rate))
	{
	}
};

// ==================== 玩家 ====================
class Player : public Character
{
private:
	int sp;
	int maxSp;
	int level;
	int exp;
	int expToNextLevel;

	Weapon* equipWeapon;
	vector<Item*> inventory;
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
	const vector<Item*> getInventory();
	string getCurrentRoomId();
	int getInventorySize();

	void setSp(int sp);
	void setMaxSp(int sp);
	void setLevel(int lv);
	void setExp(int ex);
	void setExpToNextLevel(int ex);
	void setCurrentRoomId(string id);

	int getTotalAtk() override;

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

// ==================== 敌人AI类型 ====================
enum class EnemyAIType
{
	Normal,     // 普通攻击
	Heavy,      // 概率重击
	Berserk,    // 低血量狂暴
	Healer,     // 低血量恢复
	Vampire,    // 吸血攻击
	Boss        // Boss阶段行为
};

// ==================== 敌人 ====================
class Enemy : public Character
{
private:
	int DropGold;
	int expvalue;
	int level;

	EnemyAIType aiType = EnemyAIType::Normal;

	// 敌人的掉落配置
	vector<DropEntry> dropTable;

	// 本次实际生成的掉落物品
	vector<Item*> generatedDrops;

	// 防止重复领取奖励
	bool rewardClaimed = false;

public:
	Enemy();

	Enemy(
		string enemyName,
		int enemyHp,
		int enemyAtk,
		int dropGold,
		int expValue,
		int enemyLevel,
		EnemyAIType type = EnemyAIType::Normal
	);

	~Enemy() override
	{
		clearGeneratedDrops();
	}

	// 敌人攻击
	void enemyAttack(Player& player);

	// 根据dropTable生成物品
	void generateDrops();

	// 将生成的物品转交给玩家
	// 调用后Enemy不再持有这些指针
	vector<Item*> takeGeneratedDrops()
	{
		vector<Item*> result = generatedDrops;
		generatedDrops.clear();
		return result;
	}

	// 删除尚未交给玩家的掉落物品
	void clearGeneratedDrops()
	{
		for (Item* item : generatedDrops)
		{
			delete item;
		}

		generatedDrops.clear();
	}

	// 添加一项掉落配置
	void addDropEntry(const string& itemId, int dropRate)
	{
		dropTable.push_back(DropEntry(itemId, dropRate));
	}

	void clearDropTable()
	{
		dropTable.clear();
	}

	const vector<DropEntry>& getDropTable() const
	{
		return dropTable;
	}

	int getDropGold();
	int getExpValue();

	void setDropGold(int gold)
	{
		DropGold = gold < 0 ? 0 : gold;
	}

	void setExpValue(int value)
	{
		expvalue = value < 0 ? 0 : value;
	}

	int getLevel()
	{
		return level;
	}

	void setLevel(int value)
	{
		level = value < 1 ? 1 : value;
	}

	EnemyAIType getAIType()
	{
		return aiType;
	}

	void setAIType(EnemyAIType type)
	{
		aiType = type;
	}

	// 与公共接口约定保持一致
	bool IsAlive()
	{
		return isAlive();
	}

	void SetAlive(bool alive)
	{
		if (alive)
		{
			int restoredHp = getMaxHp() > 0
				? getMaxHp()
				: 1;

			setHp(restoredHp);
		}
		else
		{
			setHp(0);
		}
	}

	bool IsRewardClaimed()
	{
		return rewardClaimed;
	}

	void SetRewardClaimed(bool claimed)
	{
		rewardClaimed = claimed;
	}
};

// ==================== NPC ====================
class Npc : public Character
{
protected:
	vector<string> talkOptions;
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

	// 保留原接口，兼容目前已有代码
	void doTalk(ColorCtrl& color);

	// 新接口可以根据对话选择执行帮助行为
	void doTalk(Player& player, ColorCtrl& color);

	void addTalkOption(string option);

	virtual void onHelp(Player& player);
	virtual void onAttack(Player& player);

	// 与公共接口约定保持一致
	bool IsAlive()
	{
		return isAlive();
	}

	void SetAlive(bool alive)
	{
		if (alive)
		{
			int restoredHp = getMaxHp() > 0
				? getMaxHp()
				: 1;

			setHp(restoredHp);
		}
		else
		{
			setHp(0);
		}
	}

	bool IsHelped()
	{
		return isHelped;
	}

	void SetHelped(bool helped)
	{
		isHelped = helped;
	}

	// 将奖励物品的所有权交给玩家
	Item* takeRewardItem()
	{
		Item* item = rewardItem;
		rewardItem = nullptr;
		return item;
	}
};

// ==================== 世界 ====================
class Room;

class World
{
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

	map<int, shared_ptr<Room>>& loadRooms(
		map<int, shared_ptr<Room>>& roomData
	);
};

// ==================== 房间基类 ====================
class Room
{
private:
	int id;
	string name;
	string description;
	map<string, int> exits;

public:
	Room(
		int id,
		const string& name,
		const string& description
	);

	virtual ~Room();

	void onEnter();

	map<string, int>& getExits();

	string getName();
};

// ==================== 商店房间 ====================
class shopRoom : public Room
{
private:
	vector<shared_ptr<Item>> goods;

public:
	shopRoom(
		int id,
		const string& name,
		const string& description
	);

	const vector<shared_ptr<Item>>& getGoods();

	void addGood(shared_ptr<Item> good);
};

// ==================== NPC房间 ====================
class npcRoom : public Room
{
private:
	vector<shared_ptr<Npc>> npcs;

public:
	npcRoom(
		int id,
		const string& name,
		const string& description
	);

	const vector<shared_ptr<Npc>>& getNpcs();

	void addNPC(shared_ptr<Npc> npc);
};

// ==================== 战斗房间 ====================
class combatRoom : public Room
{
private:
	vector<shared_ptr<Enemy>> enemy;

public:
	combatRoom(
		int id,
		const string& name,
		const string& description
	);

	const vector<shared_ptr<Enemy>>& getEnemy();

	const vector<shared_ptr<Enemy>>& getCurrentEnemy();

	void addEnemy(shared_ptr<Enemy> enemy);
};

// ==================== 商人NPC ====================
class MerchantNpc : public Npc
{
private:
	vector<Weapon> shopWeapon;
	vector<Consumable> shopConsumable;

public:
	MerchantNpc(string name);

	const vector<Weapon> getShopWeapons();

	const vector<Consumable> getShopConsumable();

	// 保留旧接口，避免原代码报错
	void AddshopWeapon(Weapon& wea);

	// 新增命名规范的接口
	void addShopWeapon(Weapon& wea)
	{
		AddshopWeapon(wea);
	}

	void addShopConsumable(Consumable& con);

	void showShop(ColorCtrl& color);

	bool buyWeapon(Player& player, int idx);

	bool buyConsumable(Player& player, int idx);

	int getWeaponPrice(int idx);

	int getConsumablePrice(int idx);
};

// ==================== 游戏总控制器 ====================
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