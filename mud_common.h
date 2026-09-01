#ifndef MUD_GAME_ARCHITECTURE_H
#define MUD_GAME_ARCHITECTURE_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <memory>
#include <windows.h>
#include <cstdlib>
#include <ctime>
using namespace std;

// ==================== 工具类 ====================

// 控制台颜色管理
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

// ==================== 物品模块 (D模块) ====================
// 物品基类
class Item
{
public:
    string name;
    int price;
    string description;
    Item();
    Item(string n, int p, string desc);
    virtual ~Item() = default;
    virtual bool use(Player& p);  // 使用效果
    virtual string getType() const;
};

// 武器类
class Weapon : public Item
{
public:
    int atkBonus;

    Weapon();
    Weapon(string n, int bonus, int p);
    bool use(Player& p) override;
    string getType() const override;
};

// 消耗品类（药水、食物等）
class Consumable : public Item
{
public:
    int hpRestore;      // 生命恢复值
    int atkBonusTemp;   // 临时攻击加成
    int duration;       // 持续回合数

    Consumable();
    Consumable(string n, int hp, int atk, int dur, int p);
    bool use(Player& p) override;
    string getType() const override;
};

// ==================== 角色模块 (C、D模块) ====================
// 角色基类
class Character
{
public:
    string name;
    int hp;
    int maxHp;
    int atkBase;
    int money;

    Character();
    Character(string n, int mh, int atk, int mon);
    virtual ~Character() = default;
    virtual void showStatus(ColorCtrl& col);
    virtual void takeDamage(int dmg);
    bool isAlive();
    virtual int getTotalAtk();  // 获取总攻击力（含武器加成）
};

// 玩家主角
class Player : public Character
{
public:
    int sp;                     // 技能点/魔法值
    int maxSp;
    int level;                  // 等级
    int exp;                    // 经验值
    int expToNextLevel;         // 升级所需经验

    Weapon* equipWeapon;        // 装备的武器（指针）
    vector<Item*> inventory;    // 背包物品
    string currentRoomId;       // 当前房间ID

    Player();
    ~Player();

    int getTotalAtk() override;
    void normalAttack(Character& target);
    void skillAttack(Character& target, int spCost);
    bool addItem(Item* item);
    bool removeItem(int index);
    void equipWeapon(Weapon* w);
    void gainExp(int amount);
    void levelUp();
    bool isInventoryFull() const;
};

// 敌人基类
class Enemy : public Character
{
public:
    int dropGold;
    int expValue;               // 击败获得经验
    vector<Item*> dropItems;    // 掉落物品表
    int level;

    Enemy();
    Enemy(string n, int mh, int atk, int dropG, int exp);
    ~Enemy();
    void enemyAttack(Player& p);
    void generateDrops();       // 生成掉落物
};

// NPC基类
class Npc : public Character
{
public:
    vector<string> talkOptions;
    int hiddenHp;               // 隐藏生命值（攻击NPC时判定用）
    Item* rewardItem;           // 帮助后奖励物品
    bool isHelped;              // 是否已帮助过

    Npc();
    Npc(string n);
    virtual ~Npc();
    void doTalk(ColorCtrl& col);
    virtual void onHelp(Player& p);     // 玩家帮助NPC
    virtual void onAttack(Player& p);   // 玩家攻击NPC
};

// 商人NPC
class MerchantNpc : public Npc
{
public:
    vector<Weapon> shopWeapons;
    vector<Consumable> shopConsumables;

    MerchantNpc(string n);
    void showShop(ColorCtrl& col);
    bool buyWeapon(Player& p, int idx);
    bool buyConsumable(Player& p, int idx);
};

// ==================== 地图与房间模块 (B模块) ====================
// 房间基类
class Room
{
protected:
    int id;                         // 房间唯一ID
    string name;                    // 房间名称
    string description;             // 房间描述
    map<string, int> exits;         // 出口方向->目标房间ID

public:
    Room();
    Room(int id_, string name_, string desc_);
    virtual ~Room() = default;
    virtual void onEnter(Player& p);        // 进入房间时触发
    virtual void showInfo(ColorCtrl& col);  // 显示房间信息
    int getId() const;
    string getName() const;
    string getDescription() const;
    map<string, int> getExits() const;
    bool hasExit(const string& direction) const;
    int getExitTarget(const string& direction) const;
    void addExit(const string& direction, int targetRoomId);
};

// 战斗房间（有怪物）
class CombatRoom : public Room
{
protected:
    vector<shared_ptr<Enemy>> enemies;

public:
    CombatRoom();
    CombatRoom(int id_, string name_, string desc_);

    void onEnter(Player& p) override;
    void showInfo(ColorCtrl& col) override;

    void addEnemy(shared_ptr<Enemy> e);
    vector<shared_ptr<Enemy>>& getEnemies();
    bool hasEnemies() const;
    void clearDeadEnemies();
};

// NPC房间
class NpcRoom : public Room
{
protected:
    vector<shared_ptr<Npc>> npcs;

public:
    NpcRoom();
    NpcRoom(int id_, string name_, string desc_);

    void onEnter(Player& p) override;
    void showInfo(ColorCtrl& col) override;

    void addNpc(shared_ptr<Npc> n);
    vector<shared_ptr<Npc>>& getNpcs();
    Npc* getNpcByName(const string& name);
};

// 商店房间
class ShopRoom : public Room
{
protected:
    vector<Weapon> shopWeapons;
    vector<Consumable> shopConsumables;
    MerchantNpc* shopkeeper;

public:
    ShopRoom();
    ShopRoom(int id_, string name_, string desc_);
    ~ShopRoom();

    void onEnter(Player& p) override;
    void showInfo(ColorCtrl& col) override;

    void addWeapon(const Weapon& w);
    void addConsumable(const Consumable& c);
    bool buyItem(Player& p, int type, int idx);  // type: 0=武器，1=消耗品
};

// 普通房间（无特殊功能）
class NormalRoom : public Room
{
public:
    NormalRoom();
    NormalRoom(int id_, string name_, string desc_);
    void onEnter(Player& p) override;
};

// ==================== 世界管理模块 (B模块扩展) ====================

class World
{
private:
    map<int, shared_ptr<Room>> rooms;
    int currentRoomId;
    int startRoomId;

public:
    World();

    void addRoom(shared_ptr<Room> room);
    Room* getRoom(int id);
    Room* getCurrentRoom();
    bool movePlayer(const string& direction);
    void setCurrentRoom(int id);
    int getCurrentRoomId() const;

    map<int, shared_ptr<Room>>& getAllRooms();
    bool loadRooms(const string& filename);  // 从文件加载地图
    void initDefaultWorld();                 // 初始化默认地图（20+房间）
};

// ==================== 存档管理模块 (E模块) ====================
class SaveManager
{
public:
    static bool saveGame(const string& path, Player& player, World& world);
    static bool loadGame(const string& path, Player& player, World& world);
    static bool autoSave(Player& player, World& world);
    static vector<string> getSaveFiles();
    static bool isValidSave(const string& path);
};

// ==================== 游戏主控制器 (A模块) ====================

class MudGame
{
private:
    Player player;
    World world;
    ColorCtrl color;
    bool isRunning;
    bool isCombatMode;
    Enemy* currentEnemy;  // 当前战斗的敌人

public:
    MudGame();
    ~MudGame();

    // 初始化
    void init();
    void initWorld();
    void selectColorTheme();

    // 显示
    void drawCharMap();
    void showMainUI();
    void showPlayerStatus();
    void showRoomInfo();

    // 命令解析
    void parseMainCmd(int opt);
    void parseCombatCmd(int opt);
    void parseCommand(const string& input);

    // 战斗系统
    void enterCombat(Enemy& enemy);
    void battle(Player& me, Enemy& en);
    void playerAttack();
    void playerSkill();
    void playerRun();
    void enemyTurn();
    void endCombat(bool won);

    // 交互系统
    void interactWithNpc(Npc* npc);
    void helpNpc(Npc* npc);
    void attackNpc(Npc* npc);
    void visitShop(MerchantNpc* merchant);

    // 存档
    void saveGame();
    void loadGame();

    // 游戏循环
    void run();
};

#endif
