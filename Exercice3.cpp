

/*
-------------------------------------------------------------
                     ½ Module : Blockchain
Master 2 Intelligence Artificielle et Science des Données (IASD)
                     Laakel Gauzi Soumaya
                Année universitaire : 2025/2026
-------------------------------------------------------------
*/


#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <chrono> 

using namespace std;
using namespace std::chrono;

// Structure pour un utilisateur
struct User {
    std::string name;
    int stake; // nombre de tokens mis en stake
};

// Classe Block
class Block {
public:
    int index;
    std::string prevHash;
    std::string creator;
    std::string hash;
    int reward;

    Block(int idx, const std::string& prev, const std::string& user, int rewardTokens) {
        index = idx;
        prevHash = prev;
        creator = user;
        reward = rewardTokens;
        hash = generateHash();
    }

    std::string generateHash() const {
        // Simple hash simulé par concaténation + index
        return prevHash + "_" + creator + "_hash_" + std::to_string(index);
    }

    void displayBlock() const {
        std::cout << "Index: " << index 
                  << " | Createur: " << creator
                  << " | Hash: " << hash
                  << " | Prev Hash: " << prevHash
                  << " | Recompense: " << reward << " tokens\n";
    }
};

// Classe Blockchain
class Blockchain {
public:
    std::vector<Block> chain;

    Blockchain() {
        // Genesis block
        chain.emplace_back(0, "0", "Genesis", 0);
    }

    std::string getLastHash() const {
        return chain.back().hash;
    }

    void addBlock(const std::string& creator, int reward) {
        Block newBlock(chain.size(), getLastHash(), creator, reward);
        chain.push_back(newBlock);
    }

    void displayChain() const {
        for (const auto& block : chain) {
            block.displayBlock();
        }
        std::cout << "-----------------------------\n";
    }
};

// Fonction pour choisir le créateur du bloc suivant selon le stake
std::string chooseCreator(std::vector<User>& users) {
    int totalStake = 0;
    for (const auto& u : users) totalStake += u.stake;

    std::cout << "Probabilites selon le stake:\n";
    for (const auto& u : users) {
        double prob = 100.0 * u.stake / totalStake;
        std::cout << "  " << u.name << ": " << u.stake << " tokens -> " 
                  << std::fixed << std::setprecision(2) << prob << "%\n";
    }

    int r = rand() % totalStake;
    int cumulative = 0;

    std::cout << "Tirage aleatoire (0-" << totalStake-1 << "): " << r << "\n";

    for (auto& u : users) {
        cumulative += u.stake;
        if (r < cumulative) {
            std::cout << "-> Createur choisi: " << u.name << "\n";
            return u.name;
        }
    }
    return users.back().name;
}

// Trouver un utilisateur par nom
User* findUser(std::vector<User>& users, const std::string& name) {
    for (auto& u : users) {
        if (u.name == name) return &u;
    }
    return nullptr;
}

int main() {
    srand(time(0));

    // Définir quelques utilisateurs avec leur stake initial
    std::vector<User> users = {
        {"val1", 50},
        {"val2", 30},
        {"val3", 20}
    };

    Blockchain myChain;
    const int rewardPerBlock = 10; // récompense en tokens par bloc

    // Simuler la création de 5 blocs
    for (int i = 0; i < 5; ++i) {
        std::cout << "=== Bloc " << i+1 << " ===\n";

        // Démarrer la mesure du temps pour ce bloc
        auto start = high_resolution_clock::now();

        std::string creator = chooseCreator(users);

        // Ajouter le bloc à la blockchain
        myChain.addBlock(creator, rewardPerBlock);

        // Ajouter la récompense au stake de l’utilisateur
        User* userPtr = findUser(users, creator);
        if (userPtr) {
            userPtr->stake += rewardPerBlock;
        }

        // Fin de la mesure pour ce bloc
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();

        // Afficher le stake actuel et le temps de validation
        std::cout << "Stake apres minage:\n";
        for (const auto& u : users) {
            std::cout << "  " << u.name << ": " << u.stake << " tokens\n";
        }

        std::cout << " Temps de validation du bloc " << i+1 
                  << " : " << duration << " microsecondes\n";
        std::cout << "-----------------------------\n";
    }

    // Afficher la blockchain finale
    std::cout << "\nBlockchain finale :\n";
    myChain.displayChain();

    return 0;
}
