void saveGame(){
    ofstream file("save.txt");
    
    // 存档玩家信息
    file << player.getName() << endl;
    file << player.getHP() << endl;
    file << player.getAttack() << endl;
    file << player.getMoney() << endl;
    file << *player.getEquipWeapon();  // 
    
    // 存档4个怪物信息
    file << enemy1.isAlive() << endl;
    file << enemy2.isAlive() << endl;
    file << enemy3.isAlive() << endl;
    file << enemy4.isAlive() << endl;
    
    // 存档2个NPC信息
    file << npc1.isAlive() << endl;
    file << npc1.getisHelped() << endl;
    file << npc2.isAlive() << endl;
    file << npc2.getisHelped() << endl;
    
    // 存档当前房间号
    file << world.getCurrentRoomId() << endl;
    
    file.close();
}
void loadGame(){
    ifstream file("save.txt");
    if (!file.is_open()) {
        cout << "没有找到存档文件！" << endl;
        return;
    }
    
    // 读取玩家信息
    string name;
    int hp, attack, money;
    file >> name;
    file >> hp;
    file >> attack;
    file >> money;
    
    player.setName(name);
    player.setHP(hp);
    player.setAttack(attack);
    player.setMoney(money);
    Weapon* weapon = new Weapon();
    file >> *weapon;
    player.setEquipWeapon(weapon);
    
    // 读取4个怪物信息
    bool enemy1Alive, enemy2Alive, enemy3Alive, enemy4Alive;
    file >> enemy1Alive;
    file >> enemy2Alive;
    file >> enemy3Alive;
    file >> enemy4Alive;
    enemy1.setAlive(enemy1Alive);
    enemy2.setAlive(enemy2Alive);
    enemy3.setAlive(enemy3Alive);
    enemy4.setAlive(enemy4Alive);
    
    // 读取2个NPC信息
    bool npc1Alive, npc1Helped, npc2Alive, npc2Helped;
    file >> npc1Alive;
    file >> npc1Helped;
    file >> npc2Alive;
    file >> npc2Helped;
    npc1.setAlive(npc1Alive);
    npc1.setisHelped(npc1Helped);
    npc2.setAlive(npc2Alive);
    npc2.setisHelped(npc2Helped);
    
    // 读取当前房间号（智能指针）
    int roomId;
    file >> roomId;
    world.setCurrentRoom(roomId);  // 根据房间号获取智能指针并加载
    file.close();
    cout << "读档成功！" << endl;
}