#include "mud_common.h"

#include <filesystem>
#include <fstream>

bool SaveManager::saveGame(const string& path, Player& player, World& world) {
    ofstream out(path, ios::out | ios::trunc);
    if (!out.is_open()) {
        return false;
    }
    out << "PLAYER\n";
    out << "name=" << player.getName() << "\n";
    out << "hp=" << player.getHp() << "\n";
    out << "maxHp=" << player.getMaxHp() << "\n";
    out << "atkBase=" << player.getAtkBase() << "\n";
    out << "money=" << player.getMoney() << "\n";
    out << "sp=" << player.getSp() << "\n";
    out << "maxSp=" << player.getMaxSp() << "\n";
    out << "level=" << player.getLevel() << "\n";
    out << "exp=" << player.getExp() << "\n";
    out << "expToNextLevel=" << player.getExpToNextLevel() << "\n";
    out << "room=" << player.getCurrentRoomId() << "\n";
    out << "ROOM_COUNT=" << world.getRoomCount() << "\n";
    for (const auto& [id, roomPtr] : world.getAllRooms()) {
        if (!roomPtr) {
            continue;
        }
        out << "ROOM=" << id << "|" << roomPtr->getName() << "|" << roomPtr->getDescription() << "\n";
        for (const auto& [dir, target] : roomPtr->getExits()) {
            out << "EXIT=" << id << "|" << dir << "|" << target << "\n";
        }
    }
    out.close();
    return true;
}

bool SaveManager::loadGame(const string& path, Player& player, World& world) {
    ifstream in(path);
    if (!in.is_open()) {
        return false;
    }

    map<int, map<string, int>> exitMap;
    map<int, string> roomNames;
    map<int, string> roomDesc;
    string line;
    while (getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        if (line == "PLAYER") {
            continue;
        }
        if (line.rfind("name=", 0) == 0) {
            player.setName(line.substr(5));
        } else if (line.rfind("hp=", 0) == 0) {
            player.setHp(stoi(line.substr(3)));
        } else if (line.rfind("maxHp=", 0) == 0) {
            player.setMaxHp(stoi(line.substr(6)));
        } else if (line.rfind("atkBase=", 0) == 0) {
            player.setAtkBase(stoi(line.substr(8)));
        } else if (line.rfind("money=", 0) == 0) {
            player.setMoney(stoi(line.substr(6)));
        } else if (line.rfind("sp=", 0) == 0) {
            player.setSp(stoi(line.substr(3)));
        } else if (line.rfind("maxSp=", 0) == 0) {
            player.setMaxSp(stoi(line.substr(6)));
        } else if (line.rfind("level=", 0) == 0) {
            player.setLevel(stoi(line.substr(6)));
        } else if (line.rfind("exp=", 0) == 0) {
            player.setExp(stoi(line.substr(4)));
        } else if (line.rfind("expToNextLevel=", 0) == 0) {
            player.setExpToNextLevel(stoi(line.substr(14)));
        } else if (line.rfind("room=", 0) == 0) {
            player.setCurrentRoomId(line.substr(5));
        } else if (line.rfind("ROOM=", 0) == 0) {
            size_t first = line.find('=');
            size_t second = line.find('|', first + 1);
            size_t third = line.find('|', second + 1);
            int id = stoi(line.substr(first + 1, second - first - 1));
            string name = line.substr(second + 1, third - second - 1);
            string desc = line.substr(third + 1);
            roomNames[id] = name;
            roomDesc[id] = desc;
        } else if (line.rfind("EXIT=", 0) == 0) {
            size_t first = line.find('=');
            size_t second = line.find('|', first + 1);
            size_t third = line.find('|', second + 1);
            int roomId = stoi(line.substr(first + 1, second - first - 1));
            string dir = line.substr(second + 1, third - second - 1);
            int target = stoi(line.substr(third + 1));
            exitMap[roomId][dir] = target;
        }
    }

    world = World();
    for (const auto& [id, name] : roomNames) {
        auto room = make_shared<NormalRoom>(id, name, roomDesc[id]);
        for (const auto& [dir, target] : exitMap[id]) {
            room->addExit(dir, target);
        }
        world.addRoom(room);
    }

    if (!player.getCurrentRoomId().empty()) {
        int current = stoi(player.getCurrentRoomId());
        if (world.hasRoom(current)) {
            world.setCurrentRoom(current);
        }
    }
    return true;
}

bool SaveManager::autoSave(Player& player, World& world) {
    filesystem::path dir = filesystem::current_path() / "save";
    filesystem::create_directories(dir);
    return saveGame((dir / "autosave.sav").string(), player, world);
}

vector<string> SaveManager::getSaveFiles() {
    vector<string> result;
    filesystem::path dir = filesystem::current_path() / "save";
    if (!filesystem::exists(dir) || !filesystem::is_directory(dir)) {
        return result;
    }
    for (const auto& entry : filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".sav") {
            result.push_back(entry.path().string());
        }
    }
    return result;
}

bool SaveManager::isValidSave(const string& path) {
    ifstream in(path);
    if (!in.is_open()) {
        return false;
    }
    string line;
    bool hasPlayer = false;
    while (getline(in, line)) {
        if (line == "PLAYER") {
            hasPlayer = true;
        }
    }
    return hasPlayer;
}

string SaveManager::getSaveDirectory() {
    return (filesystem::current_path() / "save").string();
}
