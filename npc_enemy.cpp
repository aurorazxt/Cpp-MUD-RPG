#include "MUD_GAME.h"

#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

// ==================== Enemy实现 ====================

Enemy::Enemy()
	: Character(),
	  DropGold(0),
	  expvalue(0),
	  level(1),
	  aiType(EnemyAIType::Normal),
	  rewardClaimed(false)
{
}

Enemy::Enemy(
	string enemyName,
	int enemyHp,
	int enemyAtk,
	int dropGold,
	int expValue,
	int enemyLevel,
	EnemyAIType type
)
	: Character(
		enemyName,
		max(1, enemyHp),
		max(0, enemyAtk),
		0
	),
	  DropGold(max(0, dropGold)),
	  expvalue(max(0, expValue)),
	  level(max(1, enemyLevel)),
	  aiType(type),
	  rewardClaimed(false)
{
	setName(enemyName);
	setMaxHp(max(1, enemyHp));
	setHp(getMaxHp());
	setAtkBase(max(0, enemyAtk));

	// 敌人不使用Character中的money作为掉落金币
	setMoney(0);
}

void Enemy::enemyAttack(Player& player)
{
	if (!IsAlive() || !player.isAlive())
	{
		return;
	}

	int damage = max(0, getTotalAtk());

	player.takeDamage(damage);
}

void Enemy::generateDrops()
{
	// 具体掉落概率将在模块四实现
	// 目前先清除上一次没有领取的掉落物
	clearGeneratedDrops();
}

int Enemy::getDropGold()
{
	return DropGold;
}

int Enemy::getExpValue()
{
	return expvalue;
}

// ==================== Npc实现 ====================

Npc::Npc()
	: Character(),
	  hiddenHp(0),
	  rewardItem(nullptr),
	  isHelped(false)
{
}

Npc::Npc(string npcName)
	: Character(npcName, 1, 0, 0),
	  hiddenHp(0),
	  rewardItem(nullptr),
	  isHelped(false)
{
	setName(npcName);
	setMaxHp(1);
	setHp(1);
}

Npc::~Npc()
{
	// 如果奖励没有交给玩家，由NPC负责释放
	delete rewardItem;
	rewardItem = nullptr;
}

int Npc::getHiddenHp()
{
	return hiddenHp;
}

Item* Npc::getRewardItem()
{
	return rewardItem;
}

bool Npc::getIsHelped()
{
	return isHelped;
}

const vector<string> Npc::getTalkOptions()
{
	return talkOptions;
}

void Npc::setHiddenHp(int hp)
{
	hiddenHp = max(0, hp);
}

void Npc::setRewardItem(Item* item)
{
	if (rewardItem != item)
	{
		delete rewardItem;
	}

	rewardItem = item;
}

void Npc::setIsHelped(bool ishelped)
{
	isHelped = ishelped;
}

void Npc::doTalk(ColorCtrl& color)
{
	if (talkOptions.empty())
	{
		cout << getName()
			 << "现在没有什么想说的。\n";
		return;
	}

	color.applyInfo();

	cout << "\n【"
		 << getName()
		 << "】对你说：\n";

	for (size_t i = 0; i < talkOptions.size(); ++i)
	{
		cout << i + 1
			 << ". "
			 << talkOptions[i]
			 << '\n';
	}

	color.resetColor();

	cout << "请选择对话选项：";

	int opt = 0;

	if (!(cin >> opt))
	{
		cin.clear();

		cin.ignore(
			(numeric_limits<streamsize>::max)(),
			'\n'
		);

		cout << "输入无效。\n";
		return;
	}

	if (
		opt < 1 ||
		opt > static_cast<int>(talkOptions.size())
	)
	{
		cout << "对话选项不存在。\n";
		return;
	}

	cout << "你选择了："
		 << talkOptions[opt - 1]
		 << '\n';
}

void Npc::doTalk(
	Player& player,
	ColorCtrl& color
)
{
	// 模块五再把对话选项与onHelp等行为连接
	(void)player;

	doTalk(color);
}

void Npc::addTalkOption(string option)
{
	if (!option.empty())
	{
		talkOptions.push_back(option);
	}
}

void Npc::onHelp(Player& player)
{
	if (isHelped)
	{
		cout << getName()
			 << "已经得到过你的帮助。\n";
		return;
	}

	if (rewardItem != nullptr)
	{
		if (!player.addItem(rewardItem))
		{
			cout << "背包已满，暂时无法领取奖励。\n";
			return;
		}

		// 所有权已经交给Player
		rewardItem = nullptr;

		cout << getName()
			 << "给了你奖励物品！\n";
	}

	isHelped = true;
}

void Npc::onAttack(Player& player)
{
	(void)player;

	cout << getName()
		 << "躲开了你的攻击！\n";
}

// ==================== MerchantNpc实现 ====================

MerchantNpc::MerchantNpc(string name)
	: Npc(name)
{
}

const vector<Weapon>
MerchantNpc::getShopWeapons()
{
	return shopWeapon;
}

const vector<Consumable>
MerchantNpc::getShopConsumable()
{
	return shopConsumable;
}

void MerchantNpc::AddshopWeapon(Weapon& wea)
{
	shopWeapon.push_back(wea);
}

void MerchantNpc::addShopConsumable(
	Consumable& con
)
{
	shopConsumable.push_back(con);
}

void MerchantNpc::showShop(ColorCtrl& color)
{
	color.applyOption();

	cout << "\n====商人商店====\n";
	cout << "【武器列表】\n";

	for (size_t i = 0; i < shopWeapon.size(); ++i)
	{
		cout << i + 1
			 << ". "
			 << shopWeapon[i].name
			 << " |攻击+"
			 << shopWeapon[i].atkBonus
			 << " |价格:"
			 << shopWeapon[i].price
			 << '\n';
	}

	cout << "\n【消耗品列表】\n";

	for (
		size_t i = 0;
		i < shopConsumable.size();
		++i
	)
	{
		cout << i + 1
			 << ". "
			 << shopConsumable[i].name
			 << " |回血"
			 << shopConsumable[i].Restore
			 << " |价格:"
			 << shopConsumable[i].price
			 << '\n';
	}

	color.resetColor();
}

bool MerchantNpc::buyWeapon(
	Player& player,
	int idx
)
{
	if (
		idx < 0 ||
		idx >= static_cast<int>(shopWeapon.size())
	)
	{
		return false;
	}

	Weapon& weapon = shopWeapon[idx];

	if (player.getMoney() < weapon.price)
	{
		return false;
	}

	if (player.isInventoryFull())
	{
		return false;
	}

	Weapon* newWeapon = new Weapon(
		weapon.id,
		weapon.name,
		weapon.atkBonus,
		weapon.price,
		weapon.description
	);

	if (!player.addItem(newWeapon))
	{
		delete newWeapon;
		return false;
	}

	player.setMoney(
		player.getMoney() - weapon.price
	);

	return true;
}

bool MerchantNpc::buyConsumable(
	Player& player,
	int idx
)
{
	if (
		idx < 0 ||
		idx >= static_cast<int>(
			shopConsumable.size()
		)
	)
	{
		return false;
	}

	Consumable& consumable =
		shopConsumable[idx];

	if (player.getMoney() < consumable.price)
	{
		return false;
	}

	if (player.isInventoryFull())
	{
		return false;
	}

	Consumable* newConsumable =
		new Consumable(
			consumable.id,
			consumable.name,
			consumable.Restore,
			consumable.atkBounsTemp,
			consumable.duration,
			consumable.price,
			consumable.description
		);

	if (!player.addItem(newConsumable))
	{
		delete newConsumable;
		return false;
	}

	player.setMoney(
		player.getMoney() -
		consumable.price
	);

	return true;
}

int MerchantNpc::getWeaponPrice(int idx)
{
	if (
		idx < 0 ||
		idx >= static_cast<int>(shopWeapon.size())
	)
	{
		return -1;
	}

	return shopWeapon[idx].price;
}

int MerchantNpc::getConsumablePrice(int idx)
{
	if (
		idx < 0 ||
		idx >= static_cast<int>(
			shopConsumable.size()
		)
	)
	{
		return -1;
	}

	return shopConsumable[idx].price;
}