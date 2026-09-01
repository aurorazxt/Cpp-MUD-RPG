#include "mud_common.h"

Npc::Npc() : Character("村民", 30, 4, 0), talkOptions(), hiddenHp(30), rewardItem(nullptr), isHelped(false) {
    talkOptions.push_back("你好");
    talkOptions.push_back("再见");
}

Npc::Npc(string n) : Character(std::move(n), 30, 5, 0), talkOptions(), hiddenHp(30), rewardItem(nullptr), isHelped(false) {
    talkOptions.push_back("你好");
    talkOptions.push_back("离开");
}

Npc::~Npc() {
    delete rewardItem;
    rewardItem = nullptr;
}

int Npc::getHiddenHp() const { return hiddenHp; }
Item* Npc::getRewardItem() const { return rewardItem; }
bool Npc::getIsHelped() const { return isHelped; }
const vector<string>& Npc::getTalkOptions() const { return talkOptions; }

void Npc::setHiddenHp(int hp) { hiddenHp = std::max(0, hp); }
void Npc::setRewardItem(Item* item) { rewardItem = item; }
void Npc::setIsHelped(bool helped) { isHelped = helped; }

void Npc::doTalk(ColorCtrl& col) {
    col.applyText();
    std::cout << name << "：";
    for (size_t i = 0; i < talkOptions.size(); ++i) {
        std::cout << i + 1 << "." << talkOptions[i] << " ";
    }
    std::cout << std::endl;
    col.resetColor();
}

void Npc::addTalkOption(const string& option) {
    talkOptions.push_back(option);
}

void Npc::onHelp(Player& p) {
    if (rewardItem != nullptr) {
        p.addItem(new Item(*rewardItem));
    }
    p.gainExp(15);
    p.setMoney(p.getMoney() + 5);
    isHelped = true;
    std::cout << name << "：感谢你帮我，我送你一个礼物。" << std::endl;
}

void Npc::onAttack(Player& p) {
    int playerDamage = p.getTotalAtk();
    if (playerDamage >= hiddenHp) {
        hiddenHp = 0;
        if (rewardItem != nullptr) {
            p.addItem(new Item(*rewardItem));
        }
        p.gainExp(25);
        p.setMoney(p.getMoney() + 10);
        std::cout << name << "：你打赢了我，拿走了我身上的东西。" << std::endl;
    } else {
        p.takeDamage(std::max(3, hiddenHp - playerDamage));
        std::cout << name << "：你的攻击不够强，我反手击中了你。" << std::endl;
    }
}

MerchantNpc::MerchantNpc(string n) : Npc(std::move(n)) {
    shopWeapons.push_back(Weapon("铁剑", 8, 25));
    shopWeapons.push_back(Weapon("长枪", 12, 40));
    shopWeapons.push_back(Weapon("重斧", 18, 60));
    shopConsumables.push_back(Consumable("小药瓶", 25, 0, 0, 15));
    shopConsumables.push_back(Consumable("力量药剂", 0, 5, 0, 20));
}

const vector<Weapon>& MerchantNpc::getShopWeapons() const { return shopWeapons; }
const vector<Consumable>& MerchantNpc::getShopConsumables() const { return shopConsumables; }

void MerchantNpc::addShopWeapon(const Weapon& w) { shopWeapons.push_back(w); }
void MerchantNpc::addShopConsumable(const Consumable& c) { shopConsumables.push_back(c); }

void MerchantNpc::showShop(ColorCtrl& col) {
    col.applyInfo();
    std::cout << "========== 商店 ==========" << std::endl;
    for (size_t i = 0; i < shopWeapons.size(); ++i) {
        std::cout << i + 1 << ". " << shopWeapons[i].getName() << " 伤害+" << shopWeapons[i].getAtkBonus()
                  << " 价格：" << shopWeapons[i].getPrice() << "金币" << std::endl;
    }
    for (size_t i = 0; i < shopConsumables.size(); ++i) {
        std::cout << i + 1 + shopWeapons.size() << ". " << shopConsumables[i].getName() << " 恢复+" << shopConsumables[i].getHpRestore()
                  << " 攻击+" << shopConsumables[i].getAtkBonusTemp() << " 价格：" << shopConsumables[i].getPrice() << "金币" << std::endl;
    }
    std::cout << "=========================" << std::endl;
    col.resetColor();
}

bool MerchantNpc::buyWeapon(Player& p, int idx) {
    if (idx < 0 || idx >= static_cast<int>(shopWeapons.size())) {
        return false;
    }
    const Weapon& weapon = shopWeapons[idx];
    if (p.getMoney() < weapon.getPrice()) {
        return false;
    }
    p.setMoney(p.getMoney() - weapon.getPrice());
    Weapon* clone = new Weapon(weapon.getName(), weapon.getAtkBonus(), weapon.getPrice());
    p.addItem(clone);
    p.equipWeapon(clone);
    return true;
}

bool MerchantNpc::buyConsumable(Player& p, int idx) {
    if (idx < 0 || idx >= static_cast<int>(shopConsumables.size())) {
        return false;
    }
    const Consumable& item = shopConsumables[idx];
    if (p.getMoney() < item.getPrice()) {
        return false;
    }
    p.setMoney(p.getMoney() - item.getPrice());
    p.addItem(new Consumable(item.getName(), item.getHpRestore(), item.getAtkBonusTemp(), item.getDuration(), item.getPrice()));
    return true;
}

int MerchantNpc::getWeaponPrice(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(shopWeapons.size())) {
        return -1;
    }
    return shopWeapons[idx].getPrice();
}

int MerchantNpc::getConsumablePrice(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(shopConsumables.size())) {
        return -1;
    }
    return shopConsumables[idx].getPrice();
}
