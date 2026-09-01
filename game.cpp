#include "mud_common.h"

#include <filesystem>
#include <sstream>

MudGame::MudGame() : isRunning(true), isCombatMode(false), currentEnemy(nullptr) {
    init();
}

MudGame::~MudGame() {
    currentEnemy = nullptr;
}

Player& MudGame::getPlayer() { return player; }
World& MudGame::getWorld() { return world; }
bool MudGame::getIsRunning() const { return isRunning; }
bool MudGame::getIsCombatMode() const { return isCombatMode; }

void MudGame::setIsRunning(bool running) { isRunning = running; }
void MudGame::setIsCombatMode(bool combat) { isCombatMode = combat; }

void MudGame::init() {
    selectColorTheme();
    player = Player();
    player.setName("阿明");
    player.setMaxHp(120);
    player.setHp(120);
    player.setAtkBase(15);
    player.setMoney(30);
    player.setMaxSp(25);
    player.setSp(25);
    player.setCurrentRoomId("1");
    initWorld();
    world.setCurrentRoom(1);
}

void MudGame::initWorld() {
    world.initDefaultWorld();

    Room* town = world.getRoom(1);
    if (town) {
        town->addExit("north", 2);
        town->addExit("east", 3);
    }

    CombatRoom* forest = dynamic_cast<CombatRoom*>(world.getRoom(2));
    if (forest) {
        auto spider = std::make_shared<Enemy>("毒蜘蛛", 42, 11, 16, 30);
        spider->generateDrops();
        forest->addEnemy(spider);
    }

    ShopRoom* market = dynamic_cast<ShopRoom*>(world.getRoom(3));
    if (market) {
        market->addWeapon(Weapon("钢铁长剑", 16, 55));
        market->addConsumable(Consumable("圣水", 40, 0, 0, 30));
    }

    player.addItem(new Weapon("木棍", 4, 0));
    player.equipWeapon(dynamic_cast<Weapon*>(player.getItem(0)));
}

void MudGame::selectColorTheme() { color.setTheme(1); }

void MudGame::drawCharMap() {
    std::cout << "\n========== 世界地图 ==========" << std::endl;
    std::cout << "1.村落广场 <-> 2.幽暗森林 <-> 4.古遗址\n";
    std::cout << "      ^             |\n";
    std::cout << "      |             v\n";
    std::cout << "3.小镇集市 <-> 5.洞穴深处\n";
    std::cout << "============================\n";
    std::cout << "当前位置：" << world.getCurrentRoom()->getName() << std::endl;
}

void MudGame::showMainUI() {
    color.applyOption();
    std::cout << "\n==== MUD RPG ====\n";
    std::cout << "1. 查看状态\n";
    std::cout << "2. 查看地图\n";
    std::cout << "3. 移动\n";
    std::cout << "4. 攻击\n";
    std::cout << "5. 交互NPC\n";
    std::cout << "6. 商店\n";
    std::cout << "7. 保存\n";
    std::cout << "8. 读取\n";
    std::cout << "9. 退出\n";
    color.resetColor();
}

void MudGame::showPlayerStatus() {
    player.showStatus(color);
    std::cout << "等级: " << player.getLevel() << " 经验: " << player.getExp() << "/" << player.getExpToNextLevel()
              << " SP: " << player.getSp() << "/" << player.getMaxSp() << std::endl;
    if (player.getEquipWeapon() != nullptr) {
        std::cout << "装备武器: " << player.getEquipWeapon()->getName() << " (+" << player.getEquipWeapon()->getAtkBonus() << ")" << std::endl;
    } else {
        std::cout << "装备武器: 无" << std::endl;
    }
    std::cout << "背包：";
    for (size_t i = 0; i < player.getInventory().size(); ++i) {
        std::cout << "[" << i << "]" << player.getInventory()[i]->getName() << " ";
    }
    std::cout << std::endl;
}

void MudGame::showRoomInfo() {
    Room* room = world.getCurrentRoom();
    if (room == nullptr) {
        std::cout << "当前无房间信息。" << std::endl;
        return;
    }
    room->showInfo(color);
    if (auto* combat = dynamic_cast<CombatRoom*>(room)) {
        if (combat->hasEnemies()) {
            currentEnemy = combat->getFirstAliveEnemy();
            if (currentEnemy) {
                std::cout << "敌人 " << currentEnemy->getName() << " 出现了！" << std::endl;
            }
        }
    }
    if (auto* npcRoom = dynamic_cast<NpcRoom*>(room)) {
        for (const auto& npc : npcRoom->getNpcs()) {
            std::cout << "- NPC: " << npc->getName() << "（可交互）" << std::endl;
        }
    }
    if (auto* shopRoom = dynamic_cast<ShopRoom*>(room)) {
        if (shopRoom->getShopkeeper() != nullptr) {
            std::cout << "- 商店老板: " << shopRoom->getShopkeeper()->getName() << std::endl;
        }
    }
}

void MudGame::parseMainCmd(int opt) {
    switch (opt) {
        case 1:
            showPlayerStatus();
            break;
        case 2:
            drawCharMap();
            break;
        case 3: {
            std::cout << "请输入移动方向：north/south/east/west" << std::endl;
            string dir;
            std::cin >> dir;
            if (world.movePlayer(dir)) {
                showRoomInfo();
            } else {
                std::cout << "方向无效，无法前进。" << std::endl;
            }
            break;
        }
        case 4:
            if (currentEnemy != nullptr && currentEnemy->isAlive()) {
                battle(player, *currentEnemy);
            } else {
                std::cout << "当前房间没有可攻击的敌人。" << std::endl;
            }
            break;
        case 5:
            if (auto* room = dynamic_cast<NpcRoom*>(world.getCurrentRoom())) {
                if (!room->getNpcs().empty()) {
                    interactWithNpc(room->getNpcs()[0].get());
                }
            } else {
                std::cout << "这里没有可交互的 NPC。" << std::endl;
            }
            break;
        case 6:
            if (auto* room = dynamic_cast<ShopRoom*>(world.getCurrentRoom())) {
                visitShop(room->getShopkeeper());
            } else {
                std::cout << "这里没有商店。" << std::endl;
            }
            break;
        case 7:
            saveGame();
            break;
        case 8:
            loadGame();
            break;
        case 9:
            isRunning = false;
            std::cout << "游戏结束。" << std::endl;
            break;
        default:
            std::cout << "无效命令。" << std::endl;
            break;
    }
}

void MudGame::parseCombatCmd(int opt) {
    switch (opt) {
        case 1:
            playerAttack();
            break;
        case 2:
            playerSkill();
            break;
        case 3:
            playerRun();
            break;
        default:
            std::cout << "无效战斗命令。" << std::endl;
            break;
    }
}

void MudGame::parseCommand(const string& input) {
    string cmd = input;
    std::stringstream ss(cmd);
    std::string first;
    ss >> first;
    std::transform(first.begin(), first.end(), first.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (first == "status" || first == "s") {
        showPlayerStatus();
    } else if (first == "map" || first == "m") {
        drawCharMap();
    } else if (first == "look" || first == "l") {
        showRoomInfo();
    } else if (first == "move" || first == "go") {
        string dir;
        ss >> dir;
        if (world.movePlayer(dir)) {
            showRoomInfo();
        } else {
            std::cout << "方向不可用。" << std::endl;
        }
    } else if (first == "attack" || first == "a") {
        if (currentEnemy != nullptr && currentEnemy->isAlive()) {
            playerAttack();
        } else {
            std::cout << "当前没有敌人可攻击。" << std::endl;
        }
    } else if (first == "talk" || first == "t") {
        if (auto* room = dynamic_cast<NpcRoom*>(world.getCurrentRoom())) {
            if (!room->getNpcs().empty()) {
                interactWithNpc(room->getNpcs()[0].get());
            }
        }
    } else if (first == "shop" || first == "buy") {
        if (auto* room = dynamic_cast<ShopRoom*>(world.getCurrentRoom())) {
            visitShop(room->getShopkeeper());
        }
    } else if (first == "save") {
        saveGame();
    } else if (first == "load") {
        loadGame();
    } else if (first == "quit" || first == "exit") {
        isRunning = false;
    } else {
        std::cout << "未知命令，可输入：status/map/look/go/attack/talk/shop/save/load/quit" << std::endl;
    }
}

void MudGame::enterCombat(Enemy& enemy) {
    currentEnemy = &enemy;
    isCombatMode = true;
    std::cout << "战斗开始：你遭遇了 " << enemy.getName() << "！" << std::endl;
}

void MudGame::battle(Player& me, Enemy& en) {
    if (!isCombatMode) {
        enterCombat(en);
    }
    while (me.isAlive() && en.isAlive()) {
        std::cout << "1. 普攻  2. 技能  3. 逃跑" << std::endl;
        int choice = 0;
        std::cin >> choice;
        if (choice == 1) {
            playerAttack();
        } else if (choice == 2) {
            playerSkill();
        } else if (choice == 3) {
            playerRun();
            break;
        } else {
            std::cout << "输入无效。" << std::endl;
        }
        if (!me.isAlive() || !en.isAlive()) {
            break;
        }
        enemyTurn();
    }
}

void MudGame::playerAttack() {
    if (currentEnemy == nullptr) {
        std::cout << "当前没有敌人。" << std::endl;
        return;
    }

    int damage = player.getTotalAtk();
    currentEnemy->takeDamage(damage);
    std::cout << "你对 " << currentEnemy->getName() << " 造成了 " << damage << " 点伤害。" << std::endl;

    if (!currentEnemy->isAlive()) {
        endCombat(true);
        return;
    }
    enemyTurn();
}

void MudGame::playerSkill() {
    if (currentEnemy == nullptr) {
        std::cout << "当前没有敌人。" << std::endl;
        return;
    }
    if (player.getSp() < 8) {
        std::cout << "你的 SP 不足，无法施放技能。" << std::endl;
        return;
    }
    int damage = player.getTotalAtk() + 12;
    player.setSp(player.getSp() - 8);
    currentEnemy->takeDamage(damage);
    std::cout << "你发动技能，对 " << currentEnemy->getName() << " 造成了 " << damage << " 点伤害。" << std::endl;
    if (!currentEnemy->isAlive()) {
        endCombat(true);
        return;
    }
    enemyTurn();
}

void MudGame::playerRun() {
    if (currentEnemy == nullptr) {
        std::cout << "当前没有敌人可以逃跑。" << std::endl;
        return;
    }
    if (rand() % 2 == 0) {
        std::cout << "你成功逃离战斗。" << std::endl;
        isCombatMode = false;
        currentEnemy = nullptr;
    } else {
        std::cout << "逃跑失败，敌人追了上来！" << std::endl;
        enemyTurn();
    }
}

void MudGame::enemyTurn() {
    if (currentEnemy == nullptr || !currentEnemy->isAlive()) {
        return;
    }
    int dmg = currentEnemy->getAtkBase();
    player.takeDamage(dmg);
    std::cout << currentEnemy->getName() << " 对你造成了 " << dmg << " 点伤害。" << std::endl;
    if (!player.isAlive()) {
        std::cout << "你被击败了，游戏结束。" << std::endl;
        isRunning = false;
    }
}

void MudGame::endCombat(bool won) {
    if (won && currentEnemy != nullptr) {
        player.gainExp(currentEnemy->getExpValue());
        player.setMoney(player.getMoney() + currentEnemy->getDropGold());
        std::cout << "战斗胜利！你获得经验 " << currentEnemy->getExpValue() << " 和金币 " << currentEnemy->getDropGold() << "。" << std::endl;
        for (Item* item : currentEnemy->getDropItems()) {
            if (item) {
                player.addItem(new Item(*item));
            }
        }
    }
    isCombatMode = false;
    currentEnemy = nullptr;
    if (auto* room = dynamic_cast<CombatRoom*>(world.getCurrentRoom())) {
        room->clearDeadEnemies();
    }
}

void MudGame::interactWithNpc(Npc* npc) {
    if (npc == nullptr) {
        std::cout << "这里没有可交互的 NPC。" << std::endl;
        return;
    }
    std::cout << "1. 帮助  2. 攻击  3. 无视" << std::endl;
    int choice = 0;
    std::cin >> choice;
    switch (choice) {
        case 1:
            helpNpc(npc);
            break;
        case 2:
            attackNpc(npc);
            break;
        default:
            std::cout << "你无视了 " << npc->getName() << "。" << std::endl;
            break;
    }
}

void MudGame::helpNpc(Npc* npc) {
    if (npc == nullptr) {
        return;
    }
    npc->onHelp(player);
}

void MudGame::attackNpc(Npc* npc) {
    if (npc == nullptr) {
        return;
    }
    npc->onAttack(player);
}

void MudGame::visitShop(MerchantNpc* merchant) {
    if (merchant == nullptr) {
        std::cout << "商店不存在。" << std::endl;
        return;
    }
    merchant->showShop(color);
    std::cout << "请输入购买编号（武器/药品）: 1~N，输入 0 退出" << std::endl;
    int choice = -1;
    std::cin >> choice;
    if (choice == 0) {
        return;
    }
    if (choice <= static_cast<int>(merchant->getShopWeapons().size())) {
        if (merchant->buyWeapon(player, choice - 1)) {
            std::cout << "购买成功！" << std::endl;
        } else {
            std::cout << "金币不足或编号错误。" << std::endl;
        }
    } else {
        int idx = choice - 1 - static_cast<int>(merchant->getShopWeapons().size());
        if (merchant->buyConsumable(player, idx)) {
            std::cout << "购买成功！" << std::endl;
        } else {
            std::cout << "金币不足或编号错误。" << std::endl;
        }
    }
}

void MudGame::saveGame() {
    std::filesystem::path dir = std::filesystem::current_path() / "save";
    std::filesystem::create_directories(dir);
    std::string path = (dir / "game_save.sav").string();
    if (SaveManager::saveGame(path, player, world)) {
        std::cout << "游戏已保存到 " << path << std::endl;
    } else {
        std::cout << "保存失败。" << std::endl;
    }
}

void MudGame::loadGame() {
    std::filesystem::path path = std::filesystem::current_path() / "save/game_save.sav";
    if (SaveManager::loadGame(path.string(), player, world)) {
        std::cout << "游戏已读取。" << std::endl;
    } else {
        std::cout << "读取失败，未发现存档。" << std::endl;
    }
}

void MudGame::run() {
    std::cout << "欢迎来到 MUD RPG！" << std::endl;
    while (isRunning) {
        showMainUI();
        std::cout << "请输入命令：";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
            continue;
        }
        if (input == "1") {
            parseMainCmd(1);
        } else if (input == "2") {
            parseMainCmd(2);
        } else if (input == "3") {
            parseMainCmd(3);
        } else if (input == "4") {
            parseMainCmd(4);
        } else if (input == "5") {
            parseMainCmd(5);
        } else if (input == "6") {
            parseMainCmd(6);
        } else if (input == "7") {
            parseMainCmd(7);
        } else if (input == "8") {
            parseMainCmd(8);
        } else if (input == "9") {
            parseMainCmd(9);
        } else {
            parseCommand(input);
        }
    }
}
