#include "MUD_GAME.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

// ---------------- 新增：本地配置结构体，仅cpp内部可见，不碰头文件 ----------------
struct NpcConfig
{
    string npcName;
    vector<string> talkLines;      // 对话选项文本
    string helpTip;                // 帮助时输出文本
    string rewardType;             // Weapon / Consumable
    string rewardName;
    int rewardPrice{};
    int rewardAtkBonus{};         // weapon用
    int rewardRestore{};           // consumable用
    int rewardTempAtk{};          // consumable用
    int rewardDuration{};         // consumable用
    string rewardDesc;
};

// 从txt读取全部npc配置，私有工具函数，不写进头文件
static vector<NpcConfig> loadNpcConfigFromTxt(const string& filePath)
{
    vector<NpcConfig> cfgList;
    ifstream fin(filePath);
    if (!fin.is_open())
    {
        cout << "[警告]无法打开npc_config.txt，NPC无外部对话配置\n";
        return cfgList;
    }
    string line;
    NpcConfig curCfg;
    bool inBlock = false;

    auto trim = [](string s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        if (start == string::npos) return string("");
        return s.substr(start, end - start + 1);
        };

    while (getline(fin, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line.substr(0, 5) == "[Npc=")
        {
            // 开启新npc块
            if (inBlock) cfgList.push_back(curCfg);
            curCfg = NpcConfig{};
            size_t left = line.find('=');
            size_t right = line.find(']');
            curCfg.npcName = line.substr(left + 1, right - left - 1);
            curCfg.npcName = trim(curCfg.npcName);
            inBlock = true;
            continue;
        }
        if (line == "---")
        {
            if (inBlock)
            {
                cfgList.push_back(curCfg);
                curCfg = NpcConfig{};
                inBlock = false;
            }
            continue;
        }
        if (!inBlock) continue;

        size_t eqPos = line.find('=');
        if (eqPos == string::npos) continue;
        string key = trim(line.substr(0, eqPos));
        string val = trim(line.substr(eqPos + 1));

        if (key.substr(0, 5) == "talk_")
        {
            curCfg.talkLines.push_back(val);
        }
        else if (key == "help_tip")
        {
            curCfg.helpTip = val;
        }
        else if (key == "reward_type")
        {
            curCfg.rewardType = val;
        }
        else if (key == "reward_name")
        {
            curCfg.rewardName = val;
        }
        else if (key == "reward_price")
        {
            curCfg.rewardPrice = stoi(val);
        }
        else if (key == "reward_atkBonus")
        {
            curCfg.rewardAtkBonus = stoi(val);
        }
        else if (key == "reward_Restore")
        {
            curCfg.rewardRestore = stoi(val);
        }
        else if (key == "reward_atkBounsTemp")
        {
            curCfg.rewardTempAtk = stoi(val);
        }
        else if (key == "reward_duration")
        {
            curCfg.rewardDuration = stoi(val);
        }
        else if (key == "reward_desc")
        {
            curCfg.rewardDesc = val;
        }
    }
    if (inBlock) cfgList.push_back(curCfg);
    fin.close();
    return cfgList;
}

// 根据npc名字查找配置，仅cpp内部使用
static const NpcConfig* findNpcConfig(const vector<NpcConfig>& cfgList, const string& npcName)
{
    for (auto& cfg : cfgList)
    {
        if (cfg.npcName == npcName)
        {
            return &cfg;
        }
    }
    return nullptr;
}

//================ Enemy实现 =================
Enemy::Enemy() :Character(), DropGold(0), expvalue(100), level(1)
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
    DropGold = getMoney();
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
    if (rewardItem != nullptr)
    {
        delete rewardItem;
        rewardItem = nullptr;
    }
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

// ==========重写doTalk：读取txt配置、多选择交互、帮助分支、发放奖励 ==========
void Npc::doTalk(ColorCtrl& color)
{
    //加载npc配置文件，只读
    vector<NpcConfig> npcCfgs = loadNpcConfigFromTxt("npc_config.txt");
    const NpcConfig* pCfg = findNpcConfig(npcCfgs, this->getName());

    color.applyInfo();
    cout << "\n【" << getName() << "】\n";

    //如果读到外部txt对话，覆盖内置talkOptions
    if (pCfg != nullptr && !pCfg->talkLines.empty())
    {
        talkOptions.clear();
        for (auto& s : pCfg->talkLines)
        {
            talkOptions.push_back(s);
        }
    }

    if (talkOptions.empty())
    {
        cout << getName() << "默默看着你，没有说话。\n";
        color.resetColor();
        return;
    }

    //打印对话选项
    for (size_t i = 0; i < talkOptions.size(); i++)
    {
        cout << i + 1 << ". " << talkOptions[i] << endl;
    }
    cout << (talkOptions.size() + 1) << ".结束对话离开\n";
    color.resetColor();

    cout << "\n请输入你的选择数字：";
    int opt;
    cin >> opt;
    int maxOpt = (int)talkOptions.size() + 1;
    if (opt < 1 || opt > maxOpt)
    {
        cout << "无效选择，你结束了交谈。\n";
        return;
    }
    if (opt == maxOpt)
    {
        cout << "你转身离开。\n";
        return;
    }

    //处理选中对话分支
    int selectedIndex = opt - 1;
    string selectedTalk = talkOptions[selectedIndex];
    cout << "\n>你：" << selectedTalk << "\n";

    //约定：第2个选项（下标1）=帮助选项
    bool triggerHelp = (selectedIndex == 1);
    if (triggerHelp)
    {
       
        
            if (this->isHelped)
            {
                cout << getName() << "：谢谢你，我已经得到过你的帮助了。\n";
                return;
            }
            if (pCfg == nullptr)
            {
                cout << getName() << "：感谢你的好意，但我没有东西可以回报你。\n";
                return;
            }
            cout << pCfg->helpTip << "\n";

            Item* newReward = nullptr;
            if (pCfg->rewardType == "Weapon")
            {
                Weapon* w = new Weapon(pCfg->rewardName, pCfg->rewardAtkBonus, pCfg->rewardPrice);
                w->description = pCfg->rewardDesc;
                newReward = w;
            }
            else if (pCfg->rewardType == "Consumable")
            {
                Consumable* c = new Consumable(pCfg->rewardName, pCfg->rewardRestore, pCfg->rewardTempAtk, pCfg->rewardDuration);
                c->description = pCfg->rewardDesc;
                newReward = c;
            }

            if (newReward != nullptr)
            {
                this->setRewardItem(newReward);
                
            }
            // =========重点！这里删掉 setIsHelped(true); =========
            cout << getName() << "：非常感谢！稍后会把答谢的物品交给你。\n";
        
       
    }
    else
    {
        cout << getName() << "：只是点点头，听你说完。\n";
    }
}

void Npc::addTalkOption(string option)
{
    talkOptions.push_back(option);
}

// onHelp：发放奖励道具到玩家背包
void Npc::onHelp(Player& player)
{
    
    if (!isHelped)
    {
        isHelped = true;
        if (rewardItem != nullptr)
        {
            
            if (player.addItem(rewardItem))
            {
                cout << getName() << "给了你奖励【" << rewardItem->name << "】，已放入背包！\n";
                //重要：npc不再持有这个指针，所有权交给player背包
                rewardItem = nullptr;
            }
            else
            {
                cout << "背包已满！无法接收NPC的奖励！\n";
                //背包满，释放内存，防止内存泄漏
                delete rewardItem;
                rewardItem = nullptr;
            }
        }
    }
}

void Npc::onAttack(Player& player)
{
    cout << getName() << "躲开了你的攻击！\n";
}