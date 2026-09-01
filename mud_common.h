#ifndef MUD_FINAL_H
#define MUD_FINAL_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
using namespace std;

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

// 武器
class Weapon
{
public:
    string name;
    int atkBonus;
    int price;
    Weapon();
    Weapon(string n, int bonus, int p);
};

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
    virtual ~Character() = default; // 虚析构
    virtual void showStatus(ColorCtrl& col);
    virtual void takeDamage(int dmg);
    bool isAlive();
};
// 玩家主角
class Player : public Character
{
public:
    int sp;
    int maxSp;
    Weapon equipWeapon;
    string currentRoomName; // 当前房间RoomA/RoomB

    Player();
    int getTotalAtk();
    void normalAttack(Character& target);//普攻
    void skillAttack(Character& target);//技能
    bool saveGame(string path);
    bool loadGame(string path);
};

// 敌人
class Enemy : public Character
{
public:
    int dropGold;
    Enemy();
    Enemy(string n, int mh, int atk, int dropG);
    void enemyAttack(Player& p);//敌人普攻
};

// NPC基类
class Npc : public Character
{
public:
    vector<string> talkOptions;
    Npc();
    Npc(string n);
    virtual ~Npc() = default; // 虚析构
    void doTalk(ColorCtrl& col);
};

// 商人NPC，派生自NPC
class MerchantNpc : public Npc
{
public:
    vector<Weapon> shopWeapons;
    MerchantNpc(string n);
    void showShop(ColorCtrl& col);
    bool buyWeapon(Player& p, int idx);
};

// 房间
class Room
{
public:
    string roomName; // RoomA / RoomB
    string desc;
    vector<string> linkedRooms; // 相连房间名字
    vector<Enemy> enemies;
    vector<Npc*> npcs; //存储指针，防止对象切片

    Room();
    Room(string name_, string desc_);//房间名字和基本介绍
    void showRoomInfo(ColorCtrl& col);//展示房间具体信息，包括NPC等交互对象
};

// 游戏总控制器
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