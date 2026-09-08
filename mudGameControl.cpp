#include "MUD_GAME.h"

#include <iostream>
#include <limits>
using namespace std;

MudGame::MudGame() {}
MudGame::~MudGame() {}

void MudGame::drawCharMap(ColorCtrl& col)
{
	col.applyInfo();
	cout << "\n========地图========\n";
	cout << "       4 ■------■6" << endl;
	cout << "         |       | " << endl;
	cout << "       3 |     5 | " << endl;
	cout << "1■------■------■" << endl;
	cout << " |       | " << endl;
	cout << " |       | " << endl;
	cout << "0■------■2" << endl;
	cout << "当前房间ID:" << world.getCurrentRoomId() << endl;
	cout << "============================\n";
	string opt;
	cout << "你想要往哪个方向走呢?(right/left/up/down/shop)" << endl;
	cin >> opt;
	col.resetColor();
	bool moveSuc = world.movePlayer(opt);
	if (moveSuc)
	{
		shared_ptr<Room> curRoom = world.getCurrentRoom();
		if (curRoom != nullptr)
		{
			curRoom->onEnter();
		}
	}
	col.resetColor();
	system("pause");
}

void MudGame::selectColorTheme()
{
	cout << "\n请选择颜色主题(1默认 2亮色 3冷色):";
	int t;
	cin >> t;
	color.setTheme(t);
	cout << "主题已切换\n";
	system("pause");
}

void MudGame::initWorld()
{
	//获取世界所有房间
	auto& allRooms = world.getAllRooms();

	// 1号 npcRoom：废弃医院 添加NPC
	auto room1 = dynamic_pointer_cast<npcRoom>(allRooms[1]);
	if (room1)
	{
		auto npc1 = make_shared<Npc>("受伤幸存者");
		npc1->addTalkOption("询问发生了什么");
		npc1->addTalkOption("给予帮助");
		room1->addNPC(npc1);
	}

	

	//战斗房间填充敌人
	auto room2 = dynamic_pointer_cast<combatRoom>(allRooms[2]);
	if (room2)
	{
		auto e1 = make_shared<Enemy>();
		e1->setName("机械流浪者");
		e1->setHp(60); e1->setMaxHp(60);
		e1->setAtkBase(8); e1->setMoney(30);
		room2->addEnemy(e1);
	}

	auto room3 = dynamic_pointer_cast<combatRoom>(allRooms[3]);
	if (room3)
	{
		auto e2 = make_shared<Enemy>();
		e2->setName("废铁狂徒");
		e2->setHp(90); e2->setMaxHp(90);
		e2->setAtkBase(11); e2->setMoney(50);
		room3->addEnemy(e2);
	}

	auto room4 = dynamic_pointer_cast<combatRoom>(allRooms[4]);
	if (room4)
	{
		auto e3 = make_shared<Enemy>();
		e3->setName("矿区变异体");
		e3->setHp(120); e3->setMaxHp(120);
		e3->setAtkBase(14); e3->setMoney(70);
		room4->addEnemy(e3);
	}

	auto room6 = dynamic_pointer_cast<combatRoom>(allRooms[6]);
	if (room6)
	{
		auto e4 = make_shared<Enemy>();
		e4->setName("蜘蛛母巢守卫");
		e4->setHp(180); e4->setMaxHp(180);
		e4->setAtkBase(18); e4->setMoney(120);
		room6->addEnemy(e4);
	}
}

shared_ptr<Room> MudGame::findRoomById(int id)
{
	return world.getRoomById(id);
}

void MudGame::showMainUI()
{
	system("cls");
	color.applyText();
	cout << "==== MUD赛博文字冒险游戏 ====\n";
	color.applyOption();
	cout << "1.查看角色状态     2.查看地图\n";
	cout << "3.进入霓虹市集     4.战斗   \n";
	cout << "5.更换颜色主题     6.存档游戏      7.读取存档\n";
	cout << "0.退出游戏\n";
	color.resetColor();

	player.showStates(color);
	auto curRoom = world.getCurrentRoom();
	if (curRoom)
	{
		cout << "当前房间:" << curRoom->getName() << "\n";
	}
	cout << "请输入你的命令数字：";
}

void MudGame::parseMainCmd(int opt)
{
	switch (opt)
	{
	case 1:
	{
		system("cls");
		color.applyInfo();
		cout << "----------角色详细状态----------\n";
		player.showStates(color);
		cout << "SP:" << player.getSp() << "/" << player.getMaxSp() << endl;
		cout << "等级:" << player.getLevel() << " 经验:" << player.getExp() << "/" << player.getExpToNextLevel() << endl;
		auto eqWeapon = player.getEquipWeapon();
		if (eqWeapon.name.empty())
			cout << "当前装备武器：无\n";
		else
			cout << "当前装备武器：" << eqWeapon.name << " 攻击加成:" << eqWeapon.atkBonus << endl;

		// ========== 新增：展示背包 ==========
		cout << "\n----------背包列表----------\n";
		int invSize = player.getInventorySize();
		if (invSize <= 0)
		{
			cout << "背包为空\n";
		}
		else
		{
			for (int i = 0; i < invSize; i++)
			{
				Item* pItem = player.getItem(i);
				if (!pItem) continue;

				cout << "[" << i << "] " << pItem->name;
				if (pItem->getType() == "Weapon")
				{
					Weapon* w = dynamic_cast<Weapon*>(pItem);
					cout << " 【武器】攻击+" << w->atkBonus;
				}
				else if (pItem->getType() == "Consumable")
				{
					Consumable* c = dynamic_cast<Consumable*>(pItem);
					cout << " 【消耗品】回血:" << c->Restore;
				}
				cout << "\n";
			}
		}

		// ========== 新增交互逻辑：选择下标使用道具/换武器，输入-1退出背包操作 ==========
		cout << "\n请输入物品编号使用/装备，输入-1返回主界面："<<endl;
		int selectIdx;
		cin >> selectIdx;

		if (selectIdx != -1)
		{
			Item* selItem = player.getItem(selectIdx);
			if (selItem == nullptr)
			{
				cout << ">>> 无效物品下标！\n";
			}
			else if (selItem->getType() == "Weapon")
			{
				// 更换武器：调用Weapon的use，内部执行m_equipWeapon
				Weapon* pWea = dynamic_cast<Weapon*>(selItem);
				pWea->use(player);
				cout << ">>> 武器已装备！\n";
			}
			else if (selItem->getType() == "Consumable")
			{
				// 使用消耗品，consumeItem会自动调用use并删除物品
				player.consumeItem(selectIdx);
				cout << ">>> 消耗品已使用！\n";
			}
		}

		color.resetColor();
		system("pause");
		break;
	}
	case 2:
		drawCharMap(color);
		break;
	case 3:
	{
		//移动到商店id=7
		world.setCurrentRoom(7);
		auto shop = world.getCurrentRoom();
		if (shop) shop->onEnter();

		vector<Weapon> shopWeapons = {
			Weapon("破片小刀", 5, 40),
			Weapon("脉冲刃", 12, 120)
		};
		vector<Consumable> shopConsumables = {
			Consumable("急救喷雾", 30, 0, 25),
			Consumable("狂暴药剂", 20, 8, 50)
		};

		while (true)
		{
			cout << "\n====霓虹市集选购面板====" << endl;
			cout << "你的金币：" << player.getMoney() << " |背包剩余容量：" << (20 - player.getInventorySize()) << endl;
			cout << "\n【武器列表】" << endl;
			for (size_t i = 0; i < shopWeapons.size(); i++)
			{
				cout << i + 1 << ". " << shopWeapons[i].name << " |攻击+" << shopWeapons[i].atkBonus << " |价格:" << shopWeapons[i].price << endl;
			}
			cout << "\n【消耗品列表】" << endl;
			for (size_t i = 0; i < shopConsumables.size(); i++)
			{
				cout << shopWeapons.size() + 1 + i << ". " << shopConsumables[i].name
					<< " |回血" << shopConsumables[i].Restore << " |价格:" << shopConsumables[i].price << endl;
			}
			cout << "0.离开商店\n请输入选购编号：";
			int opt;
			cin >> opt;
			if (opt == 0) break;

			bool buyOk = false;
			if (opt >= 1 && opt <= (int)shopWeapons.size())
			{
				int idx = opt - 1;
				Weapon& w = shopWeapons[idx];
				if (player.getMoney() >= w.price && !player.isInventoryFull())
				{
					Weapon* newW = new Weapon(w.name, w.atkBonus, w.price);
					newW->description = w.description;
					player.setMoney(player.getMoney() - w.price);
					player.addItem(newW);
					cout << "购买成功！" << w.name << "已放入背包\n";
					buyOk = true;
				}
				else
				{
					if (player.isInventoryFull()) cout << "背包已满，无法购买！\n";
					else cout << "金币不足！\n";
				}
			}
			else if (opt >= (int)shopWeapons.size() + 1 && opt <= (int)(shopWeapons.size() + shopConsumables.size()))
			{
				int idx = opt - 1 - (int)shopWeapons.size();
				Consumable& c = shopConsumables[idx];
				if (player.getMoney() >= c.price && !player.isInventoryFull())
				{
					Consumable* newC = new Consumable(c.name, c.Restore, c.atkBounsTemp, c.price);
					newC->description = c.description;
					newC->duration = c.duration;
					player.setMoney(player.getMoney() - c.price);
					player.addItem(newC);
					cout << "购买成功！" << c.name << "已放入背包\n";
					buyOk = true;
				}
				else
				{
					if (player.isInventoryFull()) cout << "背包已满，无法购买！\n";
					else cout << "金币不足！\n";
				}
			}
			if (!buyOk && opt != 0)
			{
				cout << "无效的选择！\n";
			}
		}

		cout << "是否回到公寓? 1.yes 2.no" << endl;
		int a; cin >> a;
		if (a == 1) { world.setCurrentRoom(0); }
		system("pause");
		break;
	}
	case 4: {
		auto curRoom = world.getCurrentRoom();
		if (auto combatRoomPtr = dynamic_pointer_cast<combatRoom>(curRoom))
		{
			auto& enemyList = combatRoomPtr->getEnemy();
			if (!enemyList.empty() && enemyList[0] != nullptr)
			{
				auto foe = enemyList[0];
				foe->setHp(foe->getMaxHp());
				battle(player, *foe);
			}
			
		}
		else {
			cout << "这里很安全，也许可以休息一下~"<<endl;
		}
		system("pause");
		break;
	}
	case 5:
		selectColorTheme();
		break;
	case 6:
		if (saveGame())
			cout << "√存档成功保存到save.txt\n";
		else
			cout << "×存档失败\n";
		system("pause");
		break;
	case 7:
		if (loadGame())
			cout << "√读取存档成功\n";
		else
			cout << "×读取存档失败，存档文件不存在或损坏\n";
		system("pause");
		break;
	case 0:
		cout << "游戏即将退出...\n";
		break;
	default:
		cout << "无效选项，请重新输入\n";
		system("pause");
		break;
	}
}

void MudGame::battle(Player& me, Enemy& en)
{
	color.applyInfo();
	cout << "\n=====战斗开始！对手：" << en.getName() << " =====\n";
	color.resetColor();
	while (me.isAlive() && en.isAlive())
	{
		color.applyOption();
		cout << "\n【你的回合】\n";
		cout << "1.普通攻击  2.技能攻击(消耗SP) 3.使用道具 0.逃跑\n";
		int sel;
		cin >> sel;
		if (sel == 1)
		{
			me.normalAttack(en);
			cout << "你发动普通攻击！敌人剩余HP:" << en.getHp() << endl;
		}
		else if (sel == 2)
		{
			if (me.getSp() >= 20)
			{
				me.skillAttack(en, 2);
				cout << "你释放强力技能！敌人剩余HP:" << en.getHp() << endl;
			}
			else
			{
				cout << "SP不足，无法释放技能！\n";
				continue;
			}
		}
		else if (sel == 3)
		{
			
			system("cls");
			color.applyInfo();
			cout << "======== 使用消耗品 ========\n";

			vector<int> realIndexList; //保存真实背包下标：只存消耗品的下标
			int invSize = player.getInventorySize();

			for (int i = 0; i < invSize; i++)
			{
				Item* item = player.getItem(i);
				if (!item) continue;
				if (item->getType() == "Consumable")
				{
					realIndexList.push_back(i);
					Consumable* c = dynamic_cast<Consumable*>(item);
					//显示给玩家的序号从1开始
					cout << "[" << realIndexList.size() << "] " << item->name
						<< "  回血:" << c->Restore << "\n";
				}
			}

			if (realIndexList.empty())
			{
				cout << "背包没有可用的消耗品！\n";
			}
			else
			{
				cout << "\n请输入序号使用，0返回主菜单：";
				int sel;
				if (!(cin >> sel))
				{
					cin.clear();
					cin.ignore((numeric_limits<streamsize>::max)(), '\n');
					cout << "输入无效\n";
				}
				else if (sel != 0)
				{
					//玩家输入的是显示序号，转成真实背包下标
					int displayIdx = sel - 1;
					if (displayIdx >= 0 && displayIdx < (int)realIndexList.size())
					{
						int trueBagIndex = realIndexList[displayIdx];
						//核心：调用consumeItem 使用消耗品
						player.consumeItem(trueBagIndex);
						cout << "消耗品使用成功！\n";
					}
					else
					{
						cout << "选择的序号无效\n";
					}
				}
		

			color.resetColor();
			system("pause");
			}
			cout << "你使用了补给品进行恢复，敌人剩余HP:" << en.getHp() << endl;
		}
		else if (sel == 0)
		{
			cout << "你选择逃跑，脱离战斗！\n";
			return;
		}

		if (!en.isAlive()) break;

		//敌人回合
		cout << "\n【敌人回合】\n";
		en.enemyAttack(me);
		cout << en.getName() << "攻击了你，你的剩余HP:" << me.getHp() << endl;
	}

	if (!en.isAlive())
	{
		color.applyInfo();
		cout << "\n敌人被击败！\n";
		int gold = en.getDropGold();
		int exp = en.getExpValue();
		me.setMoney(me.getMoney() + gold);
		me.gainExp(exp);
		cout << "获得金币:" << gold << " 获得经验:" << exp << endl;
		color.resetColor();
	}
	else if (!me.isAlive())
	{
		cout << "\n你被打倒了，游戏结束\n";
	}

}

void MudGame::run()
{
	initWorld();
	int option = -1;
	while (true)
	{
		showMainUI();
		if (!(cin >> option))
		{
			cin.clear();
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			option = -1;
		}
		if (option == 0) break;

		parseMainCmd(option);
		
		
	}
	cout << "再见！\n";
}

bool MudGame::saveGame()
{
	return player.savePlayerToFile("save.txt");
}

bool MudGame::loadGame()
{
	bool ok = player.loadPlayerFromFile("save.txt");
	if (ok)
	{
		//读档同步玩家房间id到world
		int rid = stoi(player.getCurrentRoomId());
		world.setCurrentRoom(rid);
	}
	return ok;
}

// 新增：打印玩家背包，不需要修改头文件，写在mudGameControl.cpp文件顶部
void MudGame::ShowPlayerInventory(Player& player, ColorCtrl& color)
{
	color.applyInfo();
	cout << "\n==========背包==========\n";
	int size = player.getInventorySize();
	if (size <= 0)
	{
		cout << "背包空空如也。\n";
		color.resetColor();
		return;
	}
	for (int i = 0; i < size; i++)
	{
		Item* item = player.getItem(i);
		if (!item) continue;

		cout << "[" << i << "] ";
		cout << item->name;
		if (item->getType() == "Weapon")
		{
			Weapon* w = dynamic_cast<Weapon*>(item);
			cout << " 【武器】攻击+" << w->atkBonus;
		}
		else if (item->getType() == "Consumable")
		{
			Consumable* c = dynamic_cast<Consumable*>(item);
			cout << " 【消耗品】回血:" << c->Restore;
		}
		cout << " |价格:" << item->price << "\n";
	}
	cout << "========================\n";
	color.resetColor();
}