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
	auto& allRooms = world.getAllRooms();

	// ==================== 普通NPC ====================

	auto room1 =
		dynamic_pointer_cast<npcRoom>(
			allRooms[1]
		);

	if (room1)
	{
		auto npc1 =
			make_shared<Npc>(
				"受伤幸存者"
			);

		npc1->addTalkOption(
			"询问发生了什么"
		);

		npc1->addTalkOption(
			"给予帮助"
		);

		// 配置帮助NPC后的奖励
		npc1->setRewardItem(
			ItemFactory::createItem(
				"first_aid_spray"
			)
		);

		room1->addNPC(npc1);
	}

	// ==================== 商人NPC ====================

	auto room5 =
		dynamic_pointer_cast<npcRoom>(
			allRooms[5]
		);

	if (room5)
	{
		auto merchant =
			make_shared<MerchantNpc>(
				"黑市商人"
			);

		Weapon w1(
			"scrap_knife",
			"破片小刀",
			5,
			40,
			"用金属碎片打磨而成的小刀。"
		);

		Weapon w2(
			"pulse_blade",
			"脉冲刃",
			12,
			120,
			"刀刃周围环绕着高频能量脉冲。"
		);

		Consumable c1(
			"first_aid_spray",
			"急救喷雾",
			30,
			0,
			0,
			25,
			"恢复30点生命值。"
		);

		Consumable c2(
			"berserk_potion",
			"狂暴药剂",
			20,
			8,
			3,
			50,
			"恢复20点生命值，并临时提高攻击力。"
		);

		merchant->addShopWeapon(w1);
		merchant->addShopWeapon(w2);

		merchant->addShopConsumable(c1);
		merchant->addShopConsumable(c2);

		room5->addNPC(merchant);
	}

	// ==================== 敌人一 ====================

	auto room2 =
		dynamic_pointer_cast<combatRoom>(
			allRooms[2]
		);

	if (room2)
	{
		auto e1 = make_shared<Enemy>(
			"机械流浪者",
			60,                      // HP
			8,                       // 攻击力
			30,                      // 掉落金币
			35,                      // 经验值
			1,                       // 等级
			EnemyAIType::Normal
		);

		room2->addEnemy(e1);
	}

	// ==================== 敌人二 ====================

	auto room3 =
		dynamic_pointer_cast<combatRoom>(
			allRooms[3]
		);

	if (room3)
	{
		auto e2 = make_shared<Enemy>(
			"废铁狂徒",
			90,
			11,
			50,
			60,
			2,
			EnemyAIType::Heavy
		);

		room3->addEnemy(e2);
	}

	// ==================== 敌人三 ====================

	auto room4 =
		dynamic_pointer_cast<combatRoom>(
			allRooms[4]
		);

	if (room4)
	{
		auto e3 = make_shared<Enemy>(
			"矿区变异体",
			120,
			14,
			70,
			90,
			3,
			EnemyAIType::Berserk
		);

		room4->addEnemy(e3);
	}

	// ==================== 敌人四 ====================

	auto room6 =
		dynamic_pointer_cast<combatRoom>(
			allRooms[6]
		);

	if (room6)
	{
		auto e4 = make_shared<Enemy>(
			"蜘蛛母巢守卫",
			180,
			18,
			120,
			160,
			5,
			EnemyAIType::Boss
		);

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
		cout << "是否回到公寓? 1.yes 2.no"<<endl;
		int a; cin >> a;
		if(a==1){ world.setCurrentRoom(0); }
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
		cout << "1普通攻击 2技能攻击(消耗SP) 3逃跑\n";
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
	system("pause");
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