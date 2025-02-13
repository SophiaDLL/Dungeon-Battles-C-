#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <ctime>
#include <limits> 

using namespace std;

// ===================== CONFIGURABLE SETTINGS =====================
// Armor drop rates (out of 100)
const int LEATHER_DROP_RATE = 10;    // 10% chance to find Leather
const int CHAIN_DROP_RATE = 7;       // 7% chance to find Chain
const int IRON_DROP_RATE = 5;        // 5% chance to find Iron
const int GOLD_DROP_RATE = 3;        // 3% chance to find Gold
const int DIAMOND_DROP_RATE = 1;     // 1% chance to find Diamond
const int HEALTH_POTION_DROP_RATE = 10; // 10% chance to find Health Potion

// Enemy spawn rates (out of 100)
const int GOBLIN_SPAWN_RATE = 50;    // 50% chance to spawn Goblin
const int HOBGOBLIN_SPAWN_RATE = 40; // 40% chance to spawn Hobgoblin
const int WITCH_SPAWN_RATE = 9.5;     // 9.5% chance to spawn Witch
const int DRAGON_SPAWN_RATE = 0.5;    // 0.5% chance to spawn Dragon

// Armor defense values
const int LEATHER_DEFENSE = 15;
const int CHAIN_DEFENSE = 30;
const int IRON_DEFENSE = 45;
const int GOLD_DEFENSE = 70;
const int DIAMOND_DEFENSE = 100;
// ===================== END OF CONFIGURABLE SETTINGS =====================


// Character Base Class
class Character {
protected:
    string name;
    int health;
    int maxHealth; 
    int attackPower;
    int defense;
public:
    Character(string n, int h, int a, int d) : name(n), health(h), maxHealth(h), attackPower(a), defense(d) {}

    string getName() { return name; }
    int getHealth() { return health; }
    int getMaxHealth() { return maxHealth; }
    int getDefense() { return defense; } 
    bool isAlive() { return health > 0; }

    virtual void takeDamage(int damage) {
        int actualDamage = damage - defense;
        if (actualDamage < 0) actualDamage = 0;
        health -= actualDamage;
        if (health < 0) health = 0;
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth; // This will make sure that you as the player dose not Excceed you maximum health class
        cout << name << " healed for " << amount << " health! (Current Health: " << health << ")\n";
    }

    virtual void attack(Character& target) {
        cout << name << " attacks " << target.getName() << " for " << attackPower << " damage!\n";
        target.takeDamage(attackPower);
    }

    virtual void displayStats() {
        cout << "=====================\n";
        cout << "Name: " << name << "\n";
        cout << "Health: " << health << " / " << maxHealth << "\n";
        cout << "Attack: " << attackPower << "\n";
        cout << "Defense: " << defense << "\n";
        cout << "=====================\n";
    }
};

// Player Class
class Player : public Character {
private:
    int level;
    int experience;
    map<string, int> inventory; /
    map<string, int> armorDefense = {
        {"Leather", LEATHER_DEFENSE},
        {"Chain", CHAIN_DEFENSE},
        {"Iron", IRON_DEFENSE},
        {"Gold", GOLD_DEFENSE},
        {"Diamond", DIAMOND_DEFENSE}
    };
public:
    Player(string n) : Character(n, 100, 15, 5), level(1), experience(0) {}

    void gainExperience(int exp) {
        experience += exp;
        cout << name << " gained " << exp << " XP!\n";
        if (experience >= level * 50) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        maxHealth += 10; // Add to youre maximum health after each level
        if (maxHealth > 250) maxHealth = 250; // Cap max health at 250
        health = maxHealth; // Resetts youre hp to the max hp that you can be when you level up (level 15 = 150hp if going from level 15 to 16 then you will go from 150 to 160hp)
        experience = 0;
        cout << name << " leveled up! Now at Level " << level << "!\n";
        cout << "Max health increased to " << maxHealth << "!\n";
    }

    void addItem(string item) {
        inventory[item]++;
        cout << name << " found a " << item << "! (Total: " << inventory[item] << ")\n";

        if (armorDefense.find(item) != armorDefense.end()) {
            defense += armorDefense[item];
            cout << "Defense increased by " << armorDefense[item] << "! (Total Defense: " << defense << ")\n";
        }
    }

    void useItem(string item) {
        if (inventory.find(item) != inventory.end() && inventory[item] > 0) {
            if (item == "Health Potion") {
                heal(20); // Provides the ammount of health to heal when yourevw use the potion
            } else {
                cout << "You used a " << item << ", but nothing happened.\n";
            }
            inventory[item]--;
            if (inventory[item] == 0) {
                inventory.erase(item); // If youre inventory of health potions = 0 then it will remove the item from youre inventroy collum
            }
        } else {
            cout << "You don't have any " << item << "s!\n";
        }
    }

    void displayInventory() {
        cout << name << "'s Inventory:\n";
        int index = 1;
        for (auto& item : inventory) {
            cout << index << ". " << item.first << ": " << item.second << "\n";
            index++;
        }
        cout << index << ". Back\n"; 

        cout << "Choose an item to use (or " << index << " to go back): ";
        int choice;
        cin >> choice;

        if (cin.fail()) { 
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            return;
        }

        if (choice >= 1 && choice < index) {
            auto it = inventory.begin();
            advance(it, choice - 1); 
            useItem(it->first); 
        } else if (choice == index) {
            cout << "Returning to the main menu.\n";
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    void attack(Character& target) override {
        int damage = attackPower + (rand() % 5); 
        cout << name << " swings at " << target.getName() << " for " << damage << " damage!\n";
        target.takeDamage(damage);
    }
};

// Enemy Class
class Enemy : public Character {
private:
    int blockChance; // Chance to block an attack
public:
    Enemy(string n, int h, int a, int d, int block) : Character(n, h, a, d), blockChance(block) {}

    void takeDamage(int damage) override {
        if (rand() % 100 < blockChance) {
            cout << name << " blocked the attack!\n";
        } else {
            Character::takeDamage(damage); 
        }
    }

    void attack(Character& target) override {
        int damage = attackPower - (rand() % 3);
        cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        target.takeDamage(damage);
    }
};

// Function to create a random enemy
Enemy createRandomEnemy() {
    int spawnRate = rand() % 100; 

    if (spawnRate < GOBLIN_SPAWN_RATE) {
        return Enemy("Goblin", 10 + rand() % 21, 5 + rand() % 8, 2, 5);
    } else if (spawnRate < GOBLIN_SPAWN_RATE + HOBGOBLIN_SPAWN_RATE) {
        return Enemy("Hobgoblin", 25 + rand() % 26, 8 + rand() % 10, 3, 10);
    } else if (spawnRate < GOBLIN_SPAWN_RATE + HOBGOBLIN_SPAWN_RATE + WITCH_SPAWN_RATE) {
        return Enemy("Witch", 40 + rand() % 31, 20 + rand() % 6, 4, 15);
    } else {
        return Enemy("Dragon", 120 + rand() % 121, 30 + rand() % 21, 10, 20);
    }
}

// Function to determine if an item is found during exploration
string findRandomItem() {
    int dropRate = rand() % 100; 

    if (dropRate < DIAMOND_DROP_RATE) {
        return "Diamond";
    } else if (dropRate < DIAMOND_DROP_RATE + GOLD_DROP_RATE) {
        return "Gold";
    } else if (dropRate < DIAMOND_DROP_RATE + GOLD_DROP_RATE + IRON_DROP_RATE) {
        return "Iron";
    } else if (dropRate < DIAMOND_DROP_RATE + GOLD_DROP_RATE + IRON_DROP_RATE + CHAIN_DROP_RATE) {
        return "Chain";
    } else if (dropRate < DIAMOND_DROP_RATE + GOLD_DROP_RATE + IRON_DROP_RATE + CHAIN_DROP_RATE + LEATHER_DROP_RATE) {
        return "Leather";
    } else {
        return "Health Potion"; // Remaining chance is for Health Potion
    }
}

// Battle System
void battle(Player& player, Enemy& enemy) {
    cout << "A wild " << enemy.getName() << " appears!\n";

    while (player.isAlive() && enemy.isAlive()) {
        cout << "\n";
        player.displayStats();
        enemy.displayStats();

        cout << "1. Attack\n2. Use Item\n3. Run\nChoose: ";
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            player.attack(enemy);
            if (enemy.isAlive()) {
                enemy.attack(player);
            } else {
                cout << enemy.getName() << " has been defeated!\n";
                player.gainExperience(25);
            }
        } else if (choice == 2) {
            player.displayInventory(); 
            if (!enemy.isAlive()) break; 
        } else if (choice == 3) {
            cout << "You fled the battle!\n";
            return;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    if (!player.isAlive()) {
        cout << "You have been defeated...\n";
    }
}

// Main Game Loop
int main() {
    srand(time(0));

    string playerName;
    cout << "Enter your character's name: ";
    cin >> playerName;
    Player player(playerName);

    cout << "Welcome, " << playerName << "! Your adventure begins...\n";

    bool playing = true;
    while (playing) {
        cout << "\nWhat would you like to do?\n";
        cout << "1. Explore\n2. View Stats\n3. View Inventory\n4. Quit\nChoose: ";
        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            int encounter = rand() % 100; 

            if (encounter < DIAMOND_DROP_RATE + GOLD_DROP_RATE + IRON_DROP_RATE + CHAIN_DROP_RATE + LEATHER_DROP_RATE + HEALTH_POTION_DROP_RATE) {
                string itemFound = findRandomItem();
                cout << "You found a " << itemFound << "!\n";
                player.addItem(itemFound);
            } else {
                Enemy enemy = createRandomEnemy(); // This will create you a lovely enemy to battle to the death (Unless its a dragon then you die atm cus no fair battles :3)
                battle(player, enemy);
            }
        } else if (choice == 2) {
            player.displayStats();
        } else if (choice == 3) {
            player.displayInventory(); 
        } else if (choice == 4) {
            playing = false;
            cout << "Thanks for playing!\n";
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
