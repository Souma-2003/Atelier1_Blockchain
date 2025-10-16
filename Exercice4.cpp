/*
-------------------------------------------------------------
                     ½ Module : Blockchain
Master 2 Intelligence Artificielle et Science des Données (IASD)
                     Laakel Gauzi Soumaya
                Année universitaire : 2025/2026
-------------------------------------------------------------
*/

#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>
#include <iomanip>
#include <chrono>
#include <openssl/sha.h>
#include <map>
#include <algorithm>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

// ------------------ Fonctions utilitaires ------------------
string sha256(const string &str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

// ------------------ Classe Transaction ------------------
class Transaction {
public:
    string id;
    string sender;
    string receiver;
    double amount;
    long timestamp;

    Transaction(string s, string r, double a) : sender(s), receiver(r), amount(a) {
        timestamp = time(nullptr);
        id = sha256(s + r + to_string(a) + to_string(timestamp));
    }

    string toString() const {
        return "TX_" + id.substr(0, 8) + ": " + sender + " → " + receiver + " [" + to_string(amount) + " tokens]";
    }

    void display() const {
        cout << " From  " << sender << " to " << receiver << " : " << amount << " tokens";
        cout << "   | ID: " << id.substr(0, 16) << "...\n";
    }
};

// ------------------ Arbre de Merkle ------------------
class MerkleTree {
private:
    vector<vector<string>> tree;

public:
    MerkleTree(const vector<Transaction> &transactions) {
        buildTree(transactions);
    }

    void buildTree(const vector<Transaction> &transactions) {
        tree.clear();
        if (transactions.empty()) {
            tree.push_back({sha256("")});
            return;
        }

        vector<string> currentLevel;
        for (const auto &tx : transactions)
            currentLevel.push_back(sha256(tx.toString()));
        tree.push_back(currentLevel);

        while (currentLevel.size() > 1) {
            currentLevel = buildMerkleLevel(currentLevel);
            tree.push_back(currentLevel);
        }
    }

    vector<string> buildMerkleLevel(const vector<string> &level) {
        vector<string> newLevel;
        for (size_t i = 0; i < level.size(); i += 2) {
            if (i + 1 < level.size())
                newLevel.push_back(sha256(level[i] + level[i + 1]));
            else
                newLevel.push_back(sha256(level[i] + level[i]));
        }
        return newLevel;
    }

    string getRoot() const {
        return tree.empty() ? "" : tree.back()[0];
    }

    void displayTree() const {
        cout << "\n MERKLE TREE STRUCTURE:\n";
        for (int level = tree.size() - 1; level >= 0; level--) {
            cout << "  Level " << (tree.size() - level - 1) << " : ";
            for (const auto &hash : tree[level])
                cout << hash.substr(0, 8) << "... ";
            cout << endl;
        }
    }
};

// ------------------ Classe Block ------------------
class Block {
public:
    int id;
    long timestamp;
    string previousHash;
    string merkleRoot;
    vector<Transaction> transactions;
    int nonce;
    string hash;
    string validator;
    double blockReward;

    Block(int idx, string prevHash, vector<Transaction> txs, double reward = 10.0)
        : id(idx), previousHash(prevHash), transactions(txs), blockReward(reward) {
        timestamp = time(nullptr);
        nonce = 0;
        validator = "";
        MerkleTree merkleTree(transactions);
        merkleRoot = merkleTree.getRoot();
        hash = calculateHash();
    }

    string calculateHash() const {
        stringstream ss;
        ss << id << timestamp << previousHash << merkleRoot << nonce << validator;
        return sha256(ss.str());
    }

    void mineBlock(int difficulty) {
        cout << "  Mining Block " << id << " with difficulty " << difficulty << "...\n";
        string target(difficulty, '0');
        nonce = 0;
        auto start = high_resolution_clock::now();
        int iterations = 0;
        while (hash.substr(0, difficulty) != target) {
            nonce++;
            hash = calculateHash();
            iterations++;
            if (iterations % 100000 == 0)
                cout << "    " << iterations << " iterations... Current hash: " << hash.substr(0, 16) << "...\n";
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "  Block mined! Nonce: " << nonce << " | Hash: " << hash << "\n";
        cout << "  Mining time: " << duration.count() << " ms | Iterations: " << iterations << "\n\n";
    }

    void validateBlock(const string &validateur) {
        auto start = high_resolution_clock::now();
        validator = validateur;
        hash = calculateHash();
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "  Block " << id << " validated by: " << validator << " (Time: " << duration.count() << " μs)\n\n";
    }

    void display() const {
        cout << "------------------------------------------------------------\n";
        cout << "  BLOCK " << setw(3) << id << "\n";
        cout << "  Hash: " << hash.substr(0, 32) << "...\n";
        cout << "  Previous: " << previousHash.substr(0, 24) << "...\n";
        cout << "  Merkle Root: " << merkleRoot.substr(0, 24) << "...\n";
        cout << "  Transactions: " << transactions.size() << " | Nonce: " << nonce << "\n";
        if (!validator.empty()) cout << "  Validator: " << validator << "\n";
        cout << "  Reward: " << blockReward << " tokens\n";
        cout << "------------------------------------------------------------\n";
        if (!transactions.empty()) {
            cout << "  Transactions details:\n";
            for (const auto &tx : transactions)
                tx.display();
        }
        cout << endl;
    }
};

// ------------------ Classe Blockchain ------------------
class Blockchain {
private:
    int difficulty;
    map<string, double> stakes;
    double totalStake;

public:
    vector<Block> chain;
    string chainName;

    Blockchain(string name = "GenericChain", int diff = 2) : chainName(name), difficulty(diff), totalStake(0) {
        vector<Transaction> genesisTx = {Transaction("system", "founder", 1000)};
        chain.emplace_back(0, "0", genesisTx, 0);
        chain[0].hash = chain[0].calculateHash();
        cout << chainName << " initialized with Genesis Block!\n";
    }

    void initializeValidators(const vector<string> &validatorNames, const vector<double> &initialStakes) {
        stakes.clear();
        totalStake = 0;
        for (size_t i = 0; i < validatorNames.size() && i < initialStakes.size(); i++) {
            stakes[validatorNames[i]] = initialStakes[i];
            totalStake += initialStakes[i];
        }
        cout << "Validators initialized with total stake: " << totalStake << " tokens\n";
    }

    void addBlockPoW(vector<Transaction> transactions, double reward = 10.0) {
        Block newBlock(chain.size(), chain.back().hash, transactions, reward);
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
        cout << "  Block " << (chain.size()-1) << " added via PoW\n";
    }

    void addBlockPoS(vector<Transaction> transactions, double reward = 10.0) {
        if (stakes.empty() || totalStake == 0) {
            cerr << " No validators available for PoS!\n";
            return;
        }
        double random = (double)rand() / RAND_MAX * totalStake;
        double cumulative = 0;
        string selectedValidator;
        for (const auto &pair : stakes) {
            cumulative += pair.second;
            if (random <= cumulative) {
                selectedValidator = pair.first;
                break;
            }
        }
        Block newBlock(chain.size(), chain.back().hash, transactions, reward);
        newBlock.validateBlock(selectedValidator);
        chain.push_back(newBlock);
        stakes[selectedValidator] += reward;
        totalStake += reward;
        cout << "  Block " << (chain.size()-1) << " added via PoS\n";
    }

    bool isChainValid() const {
        cout << "\nValidating " << chainName << "...\n";
        for (size_t i = 1; i < chain.size(); i++) {
            const Block &current = chain[i];
            const Block &previous = chain[i-1];
            if (current.previousHash != previous.hash) {
                cout << "  Invalid previous hash at block " << current.id << "\n";
                return false;
            }
            if (current.hash != current.calculateHash()) {
                cout << "  Invalid hash at block " << current.id << "\n";
                return false;
            }
            MerkleTree currentTree(current.transactions);
            if (current.merkleRoot != currentTree.getRoot()) {
                cout << "  Invalid Merkle Root at block " << current.id << "\n";
                return false;
            }
        }
        cout << chainName << " is valid! (" << chain.size() << " blocks)\n";
        return true;
    }

    void displayChain() const {
        cout << "\n" << chainName << " - BLOCKCHAIN (" << chain.size() << " blocks)\n";
        cout << "============================================================\n";
        for (const auto &block : chain)
            block.display();
    }

    void displayStakes() const {
        if (stakes.empty()) { cout << "No validators configured.\n"; return; }
        cout << "\nCURRENT STAKES DISTRIBUTION:\n";
        cout << "----------------------------------------------------------\n";
        cout << setw(15) << "Validator" << setw(12) << "Stake" << setw(12) << "Percentage\n";
        cout << "----------------------------------------------------------\n";
        for (const auto &pair : stakes) {
            double percentage = (pair.second / totalStake) * 100;
            cout << setw(15) << pair.first << setw(12) << pair.second 
                 << setw(11) << fixed << setprecision(2) << percentage << "%\n";
        }
        cout << "----------------------------------------------------------\n";
        cout << "Total Stake: " << totalStake << " tokens\n";
    }

    void displayStatistics() const {
        cout << "\n" << chainName << " STATISTICS:\n";
        cout << "  Total Blocks: " << chain.size() << "\n";
        int totalTx = 0;
        for (const auto &block : chain) totalTx += block.transactions.size();
        cout << "  Total Transactions: " << totalTx << "\n";
        cout << "  Chain Difficulty: " << difficulty << "\n";
        cout << "  Total Validators: " << stakes.size() << "\n";
    }
};

// ------------------ Programme principal ------------------
int main() {
    srand(time(nullptr));

    cout << "=================================================\n";
    cout << "         MINI BLOCKCHAIN FROM SCRATCH\n";
    cout << "=================================================\n\n";

    vector<Transaction> txs1 = {
        Transaction("sen1", "rec1", 50.5),
        Transaction("sen2", "rec2", 25.0),
        Transaction("sen3", "rec3", 15.75)
    };
    vector<Transaction> txs2 = {
        Transaction("sen4", "rec4", 10.0),
        Transaction("sen1", "rec2", 5.25),
        Transaction("sen5", "rec5", 8.5)
    };
    vector<Transaction> txs3 = {
        Transaction("sen6", "rec1", 3.0),
        Transaction("sen2", "rec5", 12.5),
        Transaction("sen5", "rec6", 7.8)
    };

    // ------------------- Test Proof of Work -------------------
    Blockchain powChain("PoW_Chain", 3);
    powChain.initializeValidators({"sen1", "sen2", "sen3"}, {100, 150, 200});

    auto powStart = high_resolution_clock::now();
    powChain.addBlockPoW(txs1, 12.5);
    powChain.addBlockPoW(txs2, 12.5);
    powChain.addBlockPoW(txs3, 12.5);
    auto powEnd = high_resolution_clock::now();
    chrono::duration<double, milli> powDuration = powEnd - powStart;

    powChain.displayStatistics();
    powChain.isChainValid();

    // ------------------- Test Proof of Stake -------------------
    Blockchain posChain("PoS_Chain");
    posChain.initializeValidators({"Validator_A", "Validator_B", "Validator_C", "Validator_D"}, {1000, 2000, 1500, 1200});

    auto posStart = high_resolution_clock::now();
    posChain.addBlockPoS(txs1, 15.0);
    posChain.addBlockPoS(txs2, 15.0);
    posChain.addBlockPoS(txs3, 15.0);
    auto posEnd = high_resolution_clock::now();
    chrono::duration<double, milli> posDuration = posEnd - posStart;

    posChain.displayStatistics();
    posChain.displayStakes();
    posChain.isChainValid();

    // ------------------- Analyse comparative améliorée -------------------
    cout << "\n\n PARTIE 4: ANALYSE COMPARATIVE\n";
    cout << "===============================\n";

    double powSpeed = powDuration.count() / 3.0; // 3 blocs ajoutés
    double posSpeed = posDuration.count() / 3.0; // 3 blocs ajoutés

    auto drawBar = [](double value, double maxVal, int length = 20) {
        int filled = static_cast<int>((value / maxVal) * length);
        string bar = string(filled, '=') + string(length - filled, ' ');
        return bar;
    };

    double maxTime = max(powDuration.count(), posDuration.count());

    cout << left;
    cout << "Metric                 | Proof of Work        | Proof of Stake\n";
    cout << "--------------------------------------------------------------\n";
    cout << setw(22) << "Total Time (ms)" 
         << " | " << setw(20) << (to_string(powDuration.count()) + " [" + drawBar(powDuration.count(), maxTime) + "]")
         << " | " << setw(20) << (to_string(posDuration.count()) + " [" + drawBar(posDuration.count(), maxTime) + "]") << "\n";
    cout << setw(22) << "Blocks Added" 
         << " | " << setw(20) << "3" 
         << " | " << setw(20) << "3" << "\n";
    cout << setw(22) << "Speed per Block (ms)" 
         << " | " << setw(20) << powSpeed 
         << " | " << setw(20) << posSpeed << "\n";
    cout << setw(22) << "Energy Consumption" 
         << " | " << setw(20) << "High " 
         << " | " << setw(20) << "Low " << "\n";
    cout << setw(22) << "Decentralization" 
         << " | " << setw(20) << "Good " 
         << " | " << setw(20) << "Limited " << "\n";
    cout << "--------------------------------------------------------------\n";

    // ------------------- Affichage détaillé des chaînes -------------------
    cout << "\n\n AFFICHAGE DETAILLE DES CHAINES:\n";
    cout << "=================================\n";

    cout << "\nPress Enter to view PoW Chain details...";
    cin.get();
    powChain.displayChain();

    cout << "\nPress Enter to view PoS Chain details...";
    cin.get();
    posChain.displayChain();


    return 0;
}
