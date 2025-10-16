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
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

using namespace std;

// ------------------ Fonction SHA-256 ------------------
string sha256(const string &str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

// ------------------ Construire un niveau ------------------
vector<string> buildMerkleLevel(const vector<string> &level) {
    vector<string> newLevel;
    for (size_t i = 0; i < level.size(); i += 2) {
        if (i + 1 < level.size())
            newLevel.push_back(sha256(level[i] + level[i + 1]));
        else
            newLevel.push_back(sha256(level[i] + level[i])); // duplicate if odd
    }
    return newLevel;
}

// ------------------ Construire tout l'arbre ------------------
vector<vector<string>> buildMerkleTree(const vector<string> &transactions) {
    vector<vector<string>> tree;
    vector<string> currentLevel;
    for (const auto &tx : transactions)
        currentLevel.push_back(sha256(tx));
    tree.push_back(currentLevel);
    while (currentLevel.size() > 1) {
        currentLevel = buildMerkleLevel(currentLevel);
        tree.push_back(currentLevel);
    }
    return tree;
}

// ------------------ Dessin ASCII ------------------
void dessinerArbreMerkle() {
    cout << "\n\033[1;36m===== Arbre de Merkle symbolique =====\033[0m\n";
    cout << "                      ROOT\n";
    cout << "             /                     \\\n";
    cout << "        H1234                      H5678\n";
    cout << "      /       \\                /         \\\n";
    cout << "   H12         H34         H56            H78\n";
    cout << "  /  \\         /  \\       /  \\           /  \\\n";
    cout << "H1   H2     H3   H4    H5   H6        H7    H8\n";
    cout << "=====================================================\n";
}

// ------------------ Calcul Merkle Proof ------------------
string calculateHashWithProof(const string &txHash, const vector<string> &proof, const vector<bool> &leftRight) {
    string hash = txHash;
    for (size_t i = 0; i < proof.size(); i++) {
        if (leftRight[i])
            hash = sha256(proof[i] + hash);
        else
            hash = sha256(hash + proof[i]);
    }
    return hash;
}

// ------------------ Visualiser parcours ------------------
void visualiserParcours(int index) {
    vector<string> nodeNames = {"H1","H2","H3","H4","H5","H6","H7","H8"};
    vector<string> level2Names = {"H12","H34","H56","H78"};
    vector<string> level3Names = {"H1234","H5678"};

    cout << "\n\033[1;33m Visualization of verification path \033[0m\n\n";

    // Niveau 1
    cout << "Level 1 (Leaves):      ";
    for (size_t i = 0; i < 8; i++) {
        if (i == index) cout << "\033[1;32m[" << nodeNames[i] << "]\033[0m ";
        else if (i == (index % 2 == 0 ? index + 1 : index - 1)) cout << "\033[1;35m[" << nodeNames[i] << "]\033[0m ";
        else cout << nodeNames[i] << " ";
    }
    cout << "\n            | \n            v\n";

    int level2 = index / 2;
    cout << "Level 2:                 ";
    for (size_t i = 0; i < 4; i++) {
        if (i == level2) cout << "\033[1;32m[" << level2Names[i] << "]\033[0m ";
        else if (i == (level2 % 2 == 0 ? level2 + 1 : level2 - 1)) cout << "\033[1;35m[" << level2Names[i] << "]\033[0m ";
        else cout << level2Names[i] << " ";
    }
    cout << "\n            | \n            v\n";

    int level3 = level2 / 2;
    cout << "Level 3:                        ";
    for (size_t i = 0; i < 2; i++) {
        if (i == level3) cout << "\033[1;32m[" << level3Names[i] << "]\033[0m ";
        else cout << "\033[1;35m[" << level3Names[i] << "]\033[0m ";
    }
    cout << "\n            | \n            v\n";
    cout << "ROOT\n";
    cout << "\033[1;36m--------------------------------------------\033[0m\n";
}

// ------------------ Afficher Merkle Proof Table ------------------
void afficherMerkleProofTable(const vector<vector<string>> &tree, int index) {
    vector<string> nodeNames = {"H1","H2","H3","H4","H5","H6","H7","H8"};
    vector<string> level2Names = {"H12","H34","H56","H78"};
    vector<string> level3Names = {"H1234","H5678"};

    string currentHash = tree[0][index];

    cout << "\n| Niveau   | Noeud courant | Voisin (Merkle Proof) | Hash resultat |\n";
    cout << "| -------- | ------------- | -------------------- | ------------- |\n";

    // Niveau feuille
    int siblingIndex = (index % 2 == 0) ? index + 1 : index - 1;
    string sibling = tree[0][siblingIndex];
    string h = (index % 2 == 0) ? sha256(currentHash + sibling) : sha256(sibling + currentHash);
    cout << "| Feuille  | \033[1;32m" << nodeNames[index] << "\033[0m | \033[1;35m" << nodeNames[siblingIndex] << "\033[0m | " << h.substr(0,16) << "... |\n";

    // Niveau 2
    int parentIndex = index / 2;
    siblingIndex = (parentIndex % 2 == 0) ? parentIndex + 1 : parentIndex - 1;
    siblingIndex = min(siblingIndex, (int)tree[1].size() - 1);
    sibling = tree[1][siblingIndex];
    h = (parentIndex % 2 == 0) ? sha256(tree[1][parentIndex] + sibling) : sha256(sibling + tree[1][parentIndex]);
    cout << "| Niveau 2 | \033[1;32m" << level2Names[parentIndex] << "\033[0m | \033[1;35m" << level2Names[siblingIndex] << "\033[0m | " << h.substr(0,16) << "... |\n";

    // Niveau 3
    parentIndex /= 2;
    siblingIndex = (parentIndex == 0) ? 1 : 0;
    sibling = tree[2][siblingIndex];
    h = (parentIndex == 0) ? sha256(tree[2][parentIndex] + sibling) : sha256(sibling + tree[2][parentIndex]);
    cout << "| Niveau 3 | \033[1;32m" << level3Names[parentIndex] << "\033[0m | \033[1;35m" << level3Names[siblingIndex] << "\033[0m | " << h.substr(0,16) << "... |\n";

    // Final
    cout << "| Final    | -             | -                     | " << tree[3][0].substr(0,16) << "... |\n";
}

// ------------------ Helper: build proof arrays ------------------
void buildProofFromIndex(const vector<vector<string>> &tree, int index, vector<string> &proof, vector<bool> &leftRight) {
    proof.clear();
    leftRight.clear();
    int idx = index;
    for (size_t level = 0; level < tree.size() - 1; level++) {
        int pairIndex = (idx % 2 == 0) ? idx + 1 : idx - 1;
        if (pairIndex < (int)tree[level].size()) {
            proof.push_back(tree[level][pairIndex]);
            leftRight.push_back(idx % 2 == 1);
        }
        idx /= 2;
    }
}

// ------------------ Main ------------------
int main() {
    vector<string> originalTransactions = {"T1","T2","T3","T4","T5","T6","T7","T8"};
    vector<string> transactions = originalTransactions;

    dessinerArbreMerkle();

    auto originalTree = buildMerkleTree(transactions);
    string originalMerkleRoot = originalTree.back()[0];

    cout << "\nOriginal Merkle Root: " << originalMerkleRoot << "\n";

    while (true) {
        cout << "\nMenu:\n";
        cout << " 1 - Verify a transaction (1..8)\n";
        cout << " 2 - Modify a transaction (user will input new value)\n";
        cout << " 3 - Show current transactions\n";
        cout << " 4 - Reset transactions to original\n";
        cout << " 0 - Exit\n";
        cout << "Choose option: ";
        int opt; cin >> opt;

        if (opt == 0) {
            cout << "Program terminated.\n";
            break;
        }

        if (opt == 1) {
            int choix;
            cout << "Enter transaction number to verify (1-8): ";
            cin >> choix;
            if (choix < 1 || choix > 8) {
                cout << "Invalid transaction number.\n";
                continue;
            }
            int index = choix - 1;
            string tx = transactions[index];
            string txHash = sha256(tx);

            auto currentTree = buildMerkleTree(transactions);

            visualiserParcours(index);
            afficherMerkleProofTable(currentTree, index);

            vector<string> proof;
            vector<bool> leftRight;
            buildProofFromIndex(currentTree, index, proof, leftRight);

            string calculatedRoot = calculateHashWithProof(txHash, proof, leftRight);

            cout << "\n===== Result =====\n";
            cout << "Calculated Merkle Root : " << calculatedRoot << "\n";
            cout << "Original Merkle Root   : " << originalMerkleRoot << "\n";

            if (calculatedRoot == originalMerkleRoot)
                cout << "\033[1;32m PROOF VALID: " << tx << " is in the original tree.\033[0m\n";
            else
                cout << "\033[1;31m PROOF INVALID: " << tx << " has been modified!\033[0m\n";
        }
        else if (opt == 2) {
            cout << "Enter transaction number to modify (1-8): ";
            int pos; cin >> pos;
            if (pos < 1 || pos > 8) continue;
            cout << "Current value: " << transactions[pos-1] << "\n";
            cout << "Enter new value (no spaces): ";
            string newVal; cin >> newVal;
            transactions[pos-1] = newVal;
            cout << "Transaction modified. Original Merkle Root remains unchanged.\n";
        }
        else if (opt == 3) {
            cout << "\nCurrent transactions:\n";
            for (size_t i = 0; i < transactions.size(); i++)
                cout << " " << i+1 << " : " << transactions[i] << "\n";
        }
        else if (opt == 4) {
            transactions = originalTransactions;
            cout << "Transactions reset to original.\n";
        }
        else {
            cout << "Unknown option.\n";
        }
    }

    return 0;
}
