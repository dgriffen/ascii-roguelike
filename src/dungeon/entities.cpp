#include <iostream>
#include <vector>
#include <memory>
#include <dungeon/entities.h>
#include <util/util.h>
#include <cstdio>

Entity::Entity(int row, int col) {
    this->row = row;
    this->col = col;
    this->alive = true;
    this->print = 'U';
}

Monster::Monster(int row, int col): Entity::Entity(row, col) {
    speed = better_rand(15) + 5;
    pc_last_seen[0] = row;
    pc_last_seen[1] = col;  
    smart = better_rand(1);
    telepathic = better_rand(1);
    tunneling = better_rand(1);
    erratic = better_rand(1);

    print = '0';
    print |= smart ? 1 : 0;
    print |= (telepathic ? 1 : 0) << 1;
    print |= (tunneling ? 1 : 0) << 2;
    print |= (erratic ? 1 : 0) << 3;

    if (print > '9') {
        print -= '9';
        print += 'a' - 1;
    }
}

void Entity::no_op() {}

Player::Player(int row, int col): Entity::Entity(row, col) {
    for(int i = 0; i < 10; i++) {
        carry[i] = 0;
    }
    for(int i = 0; i < 12; i++) {
        equipment[i] = 0;
    }
    hp = 300;
    base_damage.parse_str("0+1d4");
    speed = 10;
    print = '@';
}

template <typename E>
EIdx EntityStore::add_entity(E entity) {
    entity.index = list.size() + 1;
    list.push_back(std::make_unique<E>(entity));
    return entity.index;
}

EIdx EntityStore::spawn_player(int row, int col) {
    Player player(row, col);
    EIdx j = add_entity(player);
    return j;
}

EIdx EntityStore::spawn_monster(MonsterDescription& desc, int row, int col) {
    Monster monster(row, col);
    monster.print = desc.symbol;
    monster.speed = desc.speed.roll();
    monster.smart = desc.smart;
    monster.telepathic = desc.telepathic;
    monster.tunneling = desc.tunneling;
    monster.erratic = desc.erratic;
    monster.hp = desc.hp.roll();
    monster.damage = desc.damage;
    monster.index = list.size() + 1;

    if (desc.color.compare("RED") == 0) {
        monster.color =  10;
    } else if (desc.color.compare("GREEN") == 0) {
        monster.color =  11;
    } else if (desc.color.compare("BLUE") == 0) {
        monster.color =  12;
    } else if (desc.color.compare("CYAN") == 0) {
        monster.color =  13;
    } else if (desc.color.compare("YELLOW") == 0) {
        monster.color =  14;
    } else if (desc.color.compare("MAGENTA") == 0) {
        monster.color =  15;
    } else if (desc.color.compare("WHITE") == 0) {
        monster.color =  16;
    } else if (desc.color.compare("BLACK") == 0) {
        monster.color =  17;
    } else {
        monster.color =  3;
    }
    list.push_back(std::make_unique<Monster>(monster));
    return monster.index;
}

size_t EntityStore::size() {
    return list.size();
}

Result<Entity *, Unit> EntityStore::get(EIdx index) {
    if (index > list.size()) {
        return Result<Entity *, Unit>(unit());
    }

    return list[index - 1].get();
}

MonsterDescription::MonsterDescription() {
    smart = false;
    telepathic = false;
    tunneling = false;
    erratic = false;
    pass = false;
    pickup = false;
    destroy = false;
}

void MonsterDescription::print() {
    using namespace std;
    cout << name << endl;
    cout << description;
    cout << symbol << endl;
    cout << color << endl;
    speed.print();
    if (smart) {
        cout << "SMART ";
    }
    if (telepathic) {
        cout << "TELE ";
    }
    if (tunneling) {
        cout << "TUNNEL ";
    }
    if (erratic) {
        cout << "ERRATIC ";
    }
    if (pass) {
        cout << "PASS ";
    }
    if (pickup) {
        cout << "PICKUP ";
    }
    if (destroy) {
        cout << "DESTROY ";
    }
    cout << endl;
    hp.print();
    damage.print();
}

Monster MonsterDescription::generate(int row, int col) {
    Monster monster(row, col);
    monster.print = symbol;
    monster.speed = speed.roll();
    monster.smart = smart;
    monster.telepathic = telepathic;
    monster.tunneling = tunneling;
    monster.erratic = erratic;
    monster.hp = hp.roll();
    monster.damage = damage;

    if (color.compare("RED") == 0) {
        monster.color =  10;
    } else if (color.compare("GREEN") == 0) {
        monster.color =  11;
    } else if (color.compare("BLUE") == 0) {
        monster.color =  12;
    } else if (color.compare("CYAN") == 0) {
        monster.color =  13;
    } else if (color.compare("YELLOW") == 0) {
        monster.color =  14;
    } else if (color.compare("MAGENTA") == 0) {
        monster.color =  15;
    } else if (color.compare("WHITE") == 0) {
        monster.color =  16;
    } else if (color.compare("BLACK") == 0) {
        monster.color =  17;
    } else {
        monster.color =  3;
    }
    return monster;
}

bool is_player(Entity *entity) {
  return dynamic_cast<Player *>(entity);
}
