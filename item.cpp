#include "MUD_GAME.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <unordered_map>

using namespace std;

namespace
{
	using ItemCreator = function<Item*()>;

	const unordered_map<string, ItemCreator>& getItemCreators()
	{
		static const unordered_map<string, ItemCreator> creators =
		{
			// ==================== 7种武器 ====================

			{
				"rust_pipe",
				[]()
				{
					return new Weapon(
						"rust_pipe",
						"锈蚀钢管",
						3,
						20,
						"随处可见的旧钢管，虽然简陋，但比赤手空拳可靠。"
					);
				}
			},

			{
				"scrap_knife",
				[]()
				{
					return new Weapon(
						"scrap_knife",
						"破片小刀",
						5,
						40,
						"用金属碎片打磨而成的小刀，轻便而锋利。"
					);
				}
			},

			{
				"electric_baton",
				[]()
				{
					return new Weapon(
						"electric_baton",
						"电击警棍",
						8,
						75,
						"能够释放短促电流的警棍，适合近距离战斗。"
					);
				}
			},

			{
				"pulse_blade",
				[]()
				{
					return new Weapon(
						"pulse_blade",
						"脉冲刃",
						12,
						120,
						"刀刃周围环绕着高频能量脉冲。"
					);
				}
			},

			{
				"rail_pistol",
				[]()
				{
					return new Weapon(
						"rail_pistol",
						"磁轨手枪",
						15,
						180,
						"利用电磁力发射弹丸的高威力手枪。"
					);
				}
			},

			{
				"plasma_sword",
				[]()
				{
					return new Weapon(
						"plasma_sword",
						"等离子长剑",
						18,
						220,
						"高温等离子体形成的剑刃，可以切开坚固护甲。"
					);
				}
			},

			{
				"core_breaker",
				[]()
				{
					return new Weapon(
						"core_breaker",
						"核心破坏者",
						28,
						400,
						"专门针对大型机械核心制造的重型武器。"
					);
				}
			},

			// ==================== 8种消耗品 ====================

			{
				"first_aid_spray",
				[]()
				{
					return new Consumable(
						"first_aid_spray",
						"急救喷雾",
						30,
						0,
						0,
						25,
						"快速止血并恢复30点生命值。"
					);
				}
			},

			{
				"berserk_potion",
				[]()
				{
					return new Consumable(
						"berserk_potion",
						"狂暴药剂",
						20,
						8,
						3,
						50,
						"恢复20点生命值，并在3回合内提高8点攻击力。"
					);
				}
			},

			{
				"small_medkit",
				[]()
				{
					return new Consumable(
						"small_medkit",
						"小型医疗包",
						50,
						0,
						0,
						50,
						"恢复50点生命值。"
					);
				}
			},

			{
				"large_medkit",
				[]()
				{
					return new Consumable(
						"large_medkit",
						"高级医疗箱",
						100,
						0,
						0,
						110,
						"恢复100点生命值。"
					);
				}
			},

			{
				"combat_stimulant",
				[]()
				{
					return new Consumable(
						"combat_stimulant",
						"战斗兴奋剂",
						0,
						12,
						2,
						70,
						"在2回合内提高12点攻击力。"
					);
				}
			},

			{
				"nano_repair",
				[]()
				{
					return new Consumable(
						"nano_repair",
						"纳米修复剂",
						80,
						0,
						0,
						90,
						"释放修复纳米机器人，恢复80点生命值。"
					);
				}
			},

			{
				"adrenaline",
				[]()
				{
					return new Consumable(
						"adrenaline",
						"肾上腺素",
						15,
						6,
						5,
						65,
						"恢复15点生命值，并在5回合内提高6点攻击力。"
					);
				}
			},

			{
				"core_serum",
				[]()
				{
					return new Consumable(
						"core_serum",
						"核心强化血清",
						150,
						15,
						5,
						200,
						"恢复150点生命值，并在5回合内提高15点攻击力。"
					);
				}
			}
		};

		return creators;
	}
}

// ==================== Weapon实现 ====================

Weapon::Weapon()
{
	id = "";
	name = "";
	price = 0;
	description = "";
	atkBonus = 0;
}

Weapon::Weapon(string n, int bonus, int p)
{
	id = n;
	name = n;
	price = max(0, p);
	description = "";
	atkBonus = max(0, bonus);
}

Weapon::Weapon(
	string itemId,
	string n,
	int bonus,
	int p,
	string desc
)
{
	id = itemId;
	name = n;
	price = max(0, p);
	description = desc;
	atkBonus = max(0, bonus);
}

bool Weapon::use(Player& player)
{
	player.m_equipWeapon(this);

	cout << "你装备了【" << name << "】，攻击力增加 "
		 << atkBonus << " 点。\n";

	return true;
}

string Weapon::getType()
{
	return "Weapon";
}

// ==================== Consumable实现 ====================

Consumable::Consumable()
{
	id = "";
	name = "";
	price = 0;
	description = "";
	Restore = 0;
	atkBounsTemp = 0;
	duration = 0;
}

Consumable::Consumable(
	string n,
	int restoreHp,
	int tempAtk,
	int p
)
{
	id = n;
	name = n;
	price = max(0, p);
	description = "";
	Restore = max(0, restoreHp);
	atkBounsTemp = max(0, tempAtk);

	// 旧构造函数没有持续时间参数
	// 有攻击加成时默认持续3回合
	duration = atkBounsTemp > 0 ? 3 : 0;
}

Consumable::Consumable(
	string itemId,
	string n,
	int restoreHp,
	int tempAtk,
	int effectDuration,
	int p,
	string desc
)
{
	id = itemId;
	name = n;
	price = max(0, p);
	description = desc;
	Restore = max(0, restoreHp);
	atkBounsTemp = max(0, tempAtk);
	duration = max(0, effectDuration);
}

bool Consumable::use(Player& player)
{
	bool used = false;

	// 回血效果
	if (Restore > 0 && player.getHp() < player.getMaxHp())
	{
		int missingHp = player.getMaxHp() - player.getHp();
		int actualRestore = min(Restore, missingHp);

		player.heal(actualRestore);

		cout << "你使用了【" << name << "】，恢复 "
			 << actualRestore << " 点生命值。\n";

		used = true;
	}

	// 临时攻击力效果
	if (atkBounsTemp > 0 && duration > 0)
	{
		player.applyTempAtkBonus(atkBounsTemp, duration);

		cout << "攻击力提高 " << atkBounsTemp
			 << " 点，持续 " << duration << " 回合。\n";

		used = true;
	}

	if (!used)
	{
		cout << "当前无法使用【" << name << "】。\n";
	}

	return used;
}

string Consumable::getType()
{
	return "Consumable";
}

// ==================== ItemFactory实现 ====================

Item* ItemFactory::createItem(const string& itemId)
{
	const auto& creators = getItemCreators();

	auto it = creators.find(itemId);

	if (it == creators.end())
	{
		return nullptr;
	}

	return it->second();
}

vector<string> ItemFactory::getAllItemIds()
{
	return
	{
		"rust_pipe",
		"scrap_knife",
		"electric_baton",
		"pulse_blade",
		"rail_pistol",
		"plasma_sword",
		"core_breaker",
		"first_aid_spray",
		"berserk_potion",
		"small_medkit",
		"large_medkit",
		"combat_stimulant",
		"nano_repair",
		"adrenaline",
		"core_serum"
	};
}

bool ItemFactory::isValidItemId(const string& itemId)
{
	const auto& creators = getItemCreators();

	return creators.find(itemId) != creators.end();
}