#ifndef MUD_GAME_ARCHITECTURE_H
#define MUD_GAME_ARCHITECTURE_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <memory>
#include <windows.h>
#undef max
#undef min
#include <cstdlib>
#include <ctime>
using namespace std;

// ==================== 工具类 ====================

// 控制台颜色管理
class ColorCtrl
{
private:
    int textColor;
    int optionColor;
    int infoColor;

public:
    ColorCtrl();
    
    // Getter
    int getTextColor() const;
    int getOptionColor() const;
    int getInfoColor() const;
    
    // Setter
    void setTheme(int themeId);
    void setTextColor(int color);
    void setOptionColor(int color);
    void setInfoColor(int color);
    
    // 功能方法
    void applyText();
    void applyOption();
    void applyInfo();
    void resetColor();
};

// ==================== 物品模块 (D模块) ====================

// 物品基类
class Item
{
protected:
    string name;
    int price;
    string description;

public:
    Item();
    Item(string n, int p, string desc);
    virtual ~Item() = default;
    
    // Getter
    string getName() const;
    int getPrice() const;
    string getDescription() const;
    
    // Setter
    void setName(const string& n);
    void setPrice(int p);
    void setDescription(const string& desc);
    
    // 功能方法
    virtual bool use(class Player& p);
    virtual string getType() const;
};

// 武器类
class Weapon : public Item
{
private:
    int atkBonus;

public:
    Weapon();
    Weapon(string n, int bonus, int p);
    
    // Getter
    int getAtkBonus() const;
    
    // Setter
    void setAtkBonus(int bonus);
    
    // 功能方法
    bool use(Player& p) override;
    string getType() const override;
};

// 消耗品类（药水、食物等）
class Consumable : public Item
{
private:
    int hpRestore;
    int atkBonusTemp;
    int duration;

public:
    Consumable();
    Consumable(string n, int hp, int atk, int dur, int p);
    
    // Getter
    int getHpRestore() const;
    int getAtkBonusTemp() const;
    int getDuration() const;
    
    // Setter
    void setHpRestore(int hp);
    void setAtkBonusTemp(int atk);
    void setDuration(int dur);
    
    // 功能方法
    bool use(Player& p) override;
    string getType() const override;
};

// ==================== 角色模块 (C、D模块) ====================

// 角色基类
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
    Character(string n, int mh, int atk, int mon);
    virtual ~Character() = default;
    
    // Getter
    string getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getAtkBase() const;
    int getMoney() const;
    
    // Setter
    void setName(const string& n);
    void setHp(int h);
    void setMaxHp(int mh);
    void setAtkBase(int atk);
    void setMoney(int mon);
    
    // 功能方法
    virtual void showStatus(ColorCtrl& col);
    virtual void takeDamage(int dmg);
    virtual void heal(int amount);
    bool isAlive() const;
    virtual int getTotalAtk();
};

// 玩家主角
class Player : public Character
{
private:
    int sp;
    int maxSp;
    int level;
    int exp;
    int expToNextLevel;

    Weapon* equippedWeapon;
    vector<Item*> inventory;
    string currentRoomId;

public:
    Player();
    ~Player();
    
    // Getter
    int getSp() const;
    int getMaxSp() const;
    int getLevel() const;
    int getExp() const;
    int getExpToNextLevel() const;
    Weapon* getEquipWeapon() const;
    const vector<Item*>& getInventory() const;
    string getCurrentRoomId() const;
    int getInventorySize() const;
    
    // Setter
    void setSp(int s);
    void setMaxSp(int ms);
    void setLevel(int l);
    void setExp(int e);
    void setExpToNextLevel(int exp);
    void setCurrentRoomId(const string& id);
    
    // 功能方法
    int getTotalAtk() override;
    void normalAttack(Character& target);
    void skillAttack(Character& target, int spCost);
    bool addItem(Item* item);
    bool removeItem(int index);
    Item* getItem(int index) const;
    void equipWeapon(Weapon* w);
    void unequipWeapon();
    void gainExp(int amount);
    void levelUp();
    bool isInventoryFull() const;
    void consumeItem(int index);
};

// 敌人基类
class Enemy : public Character
{
private:
    int dropGold;
    int expValue;
    vector<Item*> dropItems;
    int level;

public:
    Enemy();
    Enemy(string n, int mh, int atk, int dropG, int exp);
    ~Enemy();
    
    // Getter
    int getDropGold() const;
    int getExpValue() const;
    const vector<Item*>& getDropItems() const;
    int getLevel() const;
    
    // Setter
    void setDropGold(int gold);
    void setExpValue(int exp);
    void setLevel(int l);
    
    // 功能方法
    void enemyAttack(Player& p);
    void generateDrops();
    void addDropItem(Item* item);
};

// NPC基类
class Npc : public Character
{
protected:
    vector<string> talkOptions;
    int hiddenHp;
    Item* rewardItem;
    bool isHelped;

public:
    Npc();
    Npc(string n);
    virtual ~Npc();
    
    // Getter
    int getHiddenHp() const;
    Item* getRewardItem() const;
    bool getIsHelped() const;
    const vector<string>& getTalkOptions() const;
    
    // Setter
    void setHiddenHp(int hp);
    void setRewardItem(Item* item);
    void setIsHelped(bool helped);
    
    // 功能方法
    void doTalk(ColorCtrl& col);
    void addTalkOption(const string& option);
    virtual void onHelp(Player& p);
    virtual void onAttack(Player& p);
};

// 商人NPC
class MerchantNpc : public Npc
{
private:
    vector<Weapon> shopWeapons;
    vector<Consumable> shopConsumables;

public:
    MerchantNpc(string n);
    
    // Getter
    const vector<Weapon>& getShopWeapons() const;
    const vector<Consumable>& getShopConsumables() const;
    
    // Setter
    void addShopWeapon(const Weapon& w);
    void addShopConsumable(const Consumable& c);
    
    // 功能方法
    void showShop(ColorCtrl& col);
    bool buyWeapon(Player& p, int idx);
    bool buyConsumable(Player& p, int idx);
    int getWeaponPrice(int idx) const;
    int getConsumablePrice(int idx) const;
};

// ==================== 地图与房间模块 (B模块) ====================

// 房间基类
class Room
{
protected:
    int id;
    string name;
    string description;
    map<string, int> exits;

public:
    Room();
    Room(int id_, string name_, string desc_);
    virtual ~Room() = default;
    
    // Getter
    int getId() const;
    string getName() const;
    string getDescription() const;
    map<string, int> getExits() const;
    
    // Setter
    void setId(int id_);
    void setName(const string& n);
    void setDescription(const string& desc);
    
    // 功能方法
    virtual void onEnter(Player& p);
    virtual void showInfo(ColorCtrl& col);
    bool hasExit(const string& direction) const;
    int getExitTarget(const string& direction) const;
    void addExit(const string& direction, int targetRoomId);
    void removeExit(const string& direction);
};

// 战斗房间（有怪物）
class CombatRoom : public Room
{
protected:
    vector<shared_ptr<Enemy>> enemies;

public:
    CombatRoom();
    CombatRoom(int id_, string name_, string desc_);
    
    // Getter
    vector<shared_ptr<Enemy>>& getEnemies();
    size_t getEnemyCount() const;
    
    // 功能方法
    void onEnter(Player& p) override;
    void showInfo(ColorCtrl& col) override;
    void addEnemy(shared_ptr<Enemy> e);
    bool hasEnemies() const;
    void clearDeadEnemies();
    Enemy* getFirstAliveEnemy();
};

// NPC房间
class NpcRoom : public Room
{
protected:
    vector<shared_ptr<Npc>> npcs;

public:
    NpcRoom();
    NpcRoom(int id_, string name_, string desc_);
    
    // Getter
    vector<shared_ptr<Npc>>& getNpcs();
    size_t getNpcCount() const;
    
    // 功能方法
    void onEnter(Player& p) override;
    void showInfo(ColorCtrl& col) override;
    void addNpc(shared_ptr<Npc> n);
    Npc* getNpcByName(const string& name);
    Npc* getNpcByIndex(int index);
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
    
    // Getter
    const vector<Weapon>& getShopWeapons() const;
    const vector<Consumable>& getShopConsumables() const;
    MerchantNpc* getShopkeeper() const;
    
    // Setter
    void setShopkeeper(MerchantNpc* npc);
    
    // 功能方法
    void onEnter(Player& p) override;
    void showInfo(ColorCtrl& col) override;
    void addWeapon(const Weapon& w);
    void addConsumable(const Consumable& c);
    bool buyItem(Player& p, int type, int idx);
    int getWeaponCount() const;
    int getConsumableCount() const;
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
    
    // Getter
    int getCurrentRoomId() const;
    int getStartRoomId() const;
    size_t getRoomCount() const;
    
    // Setter
    void setCurrentRoom(int id);
    void setStartRoomId(int id);
    
    // 功能方法
    void addRoom(shared_ptr<Room> room);
    Room* getRoom(int id);
    Room* getCurrentRoom();
    bool movePlayer(const string& direction);
    map<int, shared_ptr<Room>>& getAllRooms();
    bool loadRooms(const string& filename);
    void initDefaultWorld();
    bool hasRoom(int id) const;
    void removeRoom(int id);
};

// ==================== 存档管理模块 (E模块) ====================

class SaveManager
{
public:
    // 不需要构造函数和析构函数（全静态方法）
    
    static bool saveGame(const string& path, Player& player, World& world);
    static bool loadGame(const string& path, Player& player, World& world);
    static bool autoSave(Player& player, World& world);
    static vector<string> getSaveFiles();
    static bool isValidSave(const string& path);
    static string getSaveDirectory();
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
    Enemy* currentEnemy;

public:
    MudGame();
    ~MudGame();
    
    // Getter（可选，一般不需要外部访问）
    Player& getPlayer();
    World& getWorld();
    bool getIsRunning() const;
    bool getIsCombatMode() const;
    
    // Setter
    void setIsRunning(bool running);
    void setIsCombatMode(bool combat);
    
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
