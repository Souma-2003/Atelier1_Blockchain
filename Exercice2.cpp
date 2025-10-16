/*
-------------------------------------------------------------
                     ½ Module : Blockchain
Master 2 Intelligence Artificielle et Science des Données (IASD)
                     Laakel Gauzi Soumaya
                Année universitaire : 2025/2026
-------------------------------------------------------------
Exercice 2 : Proof of Work (PoW)

L’objectif de cet exercice est d’implémenter le mécanisme de Proof of Work (PoW)
au sein d’une blockchain simplifiée en langage C++.
Le principe consiste à trouver une valeur (nonce) telle que le hash du bloc
commence par un certain nombre de zéros, selon la difficulté choisie.
-------------------------------------------------------------
*/

#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>
#include <iomanip>
#include <chrono>
#include <openssl/sha.h> // bibliothèque pour le hachage SHA-256

// ------------------------------------------------------------
// Fonction : sha256()
// Objectif : calculer le hash SHA-256 d'une chaîne donnée
// Entrée   : string à hacher
// Sortie   : représentation hexadécimale du hash
// ------------------------------------------------------------
std::string sha256(const std::string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// ------------------------------------------------------------
// Classe : Block
// Objectif : représenter un bloc de la blockchain
// Chaque bloc contient :
//  - un index (position dans la chaîne)
//  - le hash du bloc précédent
//  - des données (transactions ou texte)
//  - un timestamp
//  - un nonce (utilisé pour le minage)
//  - le hash du bloc actuel
// ------------------------------------------------------------
class Block {
public:
    int index;
    std::string prevHash;
    long timestamp;
    std::string data;
    int nonce;
    std::string hash;

    // Constructeur du bloc
    Block(int idx, std::string prev, std::string d) {
        index = idx;
        prevHash = prev;
        data = d;
        nonce = 0;
        timestamp = std::time(nullptr); // enregistre l’heure de création du bloc
        hash = calculateHash();         // calcule le hash initial
    }

    // --------------------------------------------------------
    // Fonction : calculateHash()
    // Objectif : recalculer le hash du bloc à partir de ses données
    // --------------------------------------------------------
    std::string calculateHash() const {
        std::stringstream ss;
        ss << index << prevHash << timestamp << data << nonce;
        return sha256(ss.str());
    }

    // --------------------------------------------------------
    // Fonction : mineBlock()
    // Objectif : simuler le processus de Proof of Work
    // Principe : on incrémente le nonce jusqu’à obtenir un hash
    //            commençant par "difficulty" zéros.
    // --------------------------------------------------------
    void mineBlock(int difficulty) {
        std::string target(difficulty, '0'); // ex : "00" si difficulté = 2
        nonce = 0;

        std::cout << "Debut du minage du bloc " << index
                  << " avec difficulte " << difficulty << "...\n";

        // Début de la mesure du temps de minage
        auto start = std::chrono::high_resolution_clock::now();

        // Boucle jusqu’à ce que le hash commence par le bon nombre de zéros
        do {
            nonce++;
            hash = calculateHash();

            // Affichage périodique du progrès (tous les 1000 essais)
            if (nonce % 1000 == 0) {
                std::cout << "Nonce: " << nonce
                          << " | Hash: " << hash << "\r" << std::flush;
            }
        } while (hash.substr(0, difficulty) != target);

        // Fin de la mesure du temps
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();

        // Affichage des résultats
        std::cout << "\nBloc mine ! Nonce trouve: " << nonce
                  << " | Hash final: " << hash << "\n";
        std::cout << "\nTemps de validation du bloc " << index << " : "
                  << duration << " ms\n\n";
    }
};

// ------------------------------------------------------------
// Classe : Blockchain
// Objectif : gérer la chaîne de blocs
//  - Initialise la blockchain avec un bloc "genesis"
//  - Permet d’ajouter de nouveaux blocs avec minage
// ------------------------------------------------------------
class Blockchain {
public:
    std::vector<Block> chain;
    int difficulty;

    // Constructeur : initialise la chaîne avec un bloc genesis
    Blockchain(int diff) {
        difficulty = diff;
        chain.emplace_back(Block(0, "0", "Bloc Genesis"));
    }

    // --------------------------------------------------------
    // Fonction : addBlock()
    // Objectif : créer un nouveau bloc et le miner avant de l’ajouter
    // --------------------------------------------------------
    void addBlock(const std::string &data) {
        Block newBlock(chain.size(), chain.back().hash, data);
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
    }

    // --------------------------------------------------------
    // Fonction : displayChain()
    // Objectif : afficher les informations de chaque bloc
    // --------------------------------------------------------
    void displayChain() const {
        for (const Block &block : chain) {
            std::cout << "Index: " << block.index << "\n";
            std::cout << "Hash: " << block.hash << "\n";
            std::cout << "Prev Hash: " << block.prevHash << "\n";
            std::cout << "Nonce: " << block.nonce << "\n";
            std::cout << "-------------------------------\n";
        }
    }
};

// ------------------------------------------------------------
// Fonction principale : main()
// Objectif : exécuter plusieurs tests avec différentes difficultés
// ------------------------------------------------------------
int main() {
    // Test avec différents niveaux de difficulté
    for (int diff = 1; diff <= 3; diff++) {
        std::cout << "\n==============================\n";
        std::cout << ">>> Test avec difficulte " << diff << "\n";
        std::cout << "==============================\n";

        Blockchain myChain(diff);          // création de la blockchain
        myChain.addBlock("Donnees bloc 1"); // ajout d’un bloc avec minage
        myChain.displayChain();             // affichage du résultat
    }

    return 0;
}
