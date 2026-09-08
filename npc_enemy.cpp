#include "MUD_GAME.h"
#include <iostream>
#include <algorithm>
using namespace std;

//================ Enemy实现 =================
Enemy::Enemy() :Character(), DropGold(0), expvalue(0), level(1)
{
}

void Enemy::enemyAttack(Player& player)
{
	if (!isAlive()) return;
	player.takeDamage(getTotalAtk());
}

void Enemy::generateDrops()
{
}

int Enemy::getDropGold()
{
	return DropGold;
}

int Enemy::getExpValue()
{
	return expvalue;
}

//================ Npc实现 =================
Npc::Npc() :Character(), hiddenHp(0), rewardItem(nullptr), isHelped(false)
{
}

Npc::Npc(string npcName) :Character(npcName, 1, 0, 0), hiddenHp(0), rewardItem(nullptr), isHelped(false)
{
}

Npc::~Npc()
{
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
	hiddenHp = hp;
}

void Npc::setRewardItem(Item* item)
{
	rewardItem = item;
}

void Npc::setIsHelped(bool ishelped)
{
	isHelped = ishelped;
}

void Npc::doTalk(ColorCtrl& color)
{
	color.applyInfo();
	cout << "\n【" << getName() << "】对你说：\n";
	for (size_t i = 0; i < talkOptions.size(); i++)
	{
		cout << i + 1 << ". " << talkOptions[i] << endl;
	}
	color.resetColor();
	cout << "请选择对话选项：";
	int opt;
	cin >> opt;
	cout << "你选择了：" << talkOptions[opt - 1] << endl;
}

void Npc::addTalkOption(string option)
{
	talkOptions.push_back(option);
}

void Npc::onHelp(Player& player)
{
	if (!isHelped)
	{
		isHelped = true;
		if (rewardItem != nullptr)
		{
			player.addItem(rewardItem);
			cout << getName() << "给了你奖励物品！\n";
		}
	}
}

void Npc::onAttack(Player& player)
{
	cout << getName() << "躲开了你的攻击！\n";
}

//================ MerchantNpc商人NPC实现 =================
MerchantNpc::MerchantNpc(string name) :Npc(name)
{
}

const vector<Weapon> MerchantNpc::getShopWeapons()
{
	return shopWeapon;
}

const vector<Consumable> MerchantNpc::getShopConsumable()
{
	return shopConsumable;
}

void MerchantNpc::AddshopWeapon(Weapon& wea)
{
	shopWeapon.push_back(wea);
}

void MerchantNpc::addShopConsumable(Consumable& con)
{
	shopConsumable.push_back(con);
}

void MerchantNpc::showShop(ColorCtrl& color)
{
	color.applyOption();
	cout << "\n====商人商店====\n";
	cout << "【武器列表】\n";
	for (size_t i = 0; i < shopWeapon.size(); i++)
	{
		cout << i + 1 << ". " << shopWeapon[i].name << " |攻击+" << shopWeapon[i].atkBonus << " |价格:" << shopWeapon[i].price << endl;
	}
	cout << "\n【消耗品列表】\n";
	for (size_t i = 0; i < shopConsumable.size(); i++)
	{
		cout << i + 1 << ". " << shopConsumable[i].name << " |回血" << shopConsumable[i].Restore << " |价格:" << shopConsumable[i].price << endl;
	}
	color.resetColor();
}

bool MerchantNpc::buyWeapon(Player& player, int idx)
{
	if (idx < 0 || idx >= (int)shopWeapon.size()) return false;
	Weapon& w = shopWeapon[idx];
	if (player.getMoney() < w.price) return false;
	if (player.isInventoryFull()) return false;

	Weapon* newW = new Weapon(w.name, w.atkBonus, w.price);
	newW->description = w.description;
	player.setMoney(player.getMoney() - w.price);
	player.addItem(newW);
	return true;
}

bool MerchantNpc::buyConsumable(Player& player, int idx)
{
	if (idx < 0 || idx >= (int)shopConsumable.size()) return false;
	Consumable& c = shopConsumable[idx];
	if (player.getMoney() < c.price) return false;
	if (player.isInventoryFull()) return false;

	Consumable* newC = new Consumable(c.name, c.Restore, c.atkBounsTemp, c.price);
	newC->description = c.description;
	newC->duration = c.duration;
	player.setMoney(player.getMoney() - c.price);
	player.addItem(newC);
	return true;
}

int MerchantNpc::getWeaponPrice(int idx)
{
	if (idx < 0 || idx >= (int)shopWeapon.size()) return -1;
	return shopWeapon[idx].price;
}

int MerchantNpc::getConsumablePrice(int idx)
{
	if (idx < 0 || idx >= (int)shopConsumable.size()) return -1;
	return shopConsumable[idx].price;
}