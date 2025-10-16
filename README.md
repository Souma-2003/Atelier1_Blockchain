# 🧱 ATELIER 1 : Mini Blockchain en C++ – From Scratch

## 🎓 Master 2 Intelligence Artificielle et Science des Données (IASD)

**Étudiante :** Laakel Gauzi Soumaya

**Année universitaire :** 2025 / 2026

---

## 🧠 Introduction générale

Cet atelier a pour objectif de **concevoir une mini blockchain en C++** entièrement **from scratch**, en comprenant et codant :

* l’**arbre de Merkle** pour résumer les transactions,
* le **Proof of Work (PoW)** pour sécuriser les blocs,
* le **Proof of Stake (PoS)** comme alternative plus rapide,
* et la **mini-blockchain intégrée** combinant tous ces éléments.

Chaque exercice construit progressivement les composants essentiels d’une blockchain.

---

# 🌳 EXERCICE 1 – Arbre de Merkle

![Diagramme Merkle Tree](images/tree.png)


### 🎯 **Objectif**

L’objectif de cet exercice est d’implémenter **un arbre de Merkle basique en C++**, afin de :

* Calculer la **racine de Merkle (Merkle Root)** à partir d’un ensemble de transactions.
* Générer et vérifier une **preuve de Merkle (Merkle Proof)**.
* Tester la **validation** ou **l’invalidation** d’une transaction si elle est modifiée.

---

### ⚙️ **Principe**

Un **arbre de Merkle** est une structure arborescente utilisée pour vérifier l’intégrité d’un ensemble de données (comme les transactions d’un bloc dans la blockchain).

1. Chaque transaction est d’abord **hachée avec SHA-256**.
2. Les paires de hachages sont ensuite combinées et **rehachées** pour former le niveau supérieur.
3. Ce processus continue jusqu’à obtenir **un seul hachage** au sommet :
    **La racine de Merkle (Merkle Root)**.
4. Lorsqu’on veut vérifier une transaction, on utilise son **Merkle Proof**, c’est-à-dire le chemin de hachage reliant cette transaction à la racine.

   * Si la racine calculée à partir de la preuve est identique à la racine originale, la transaction est **valide**.
   * Sinon, elle a été **modifiée**.

---

### 🧠 **Exemple d’exécution**

#### 🔹 Cas 1 : Vérification d’une transaction inchangée

**Merkle Proof d'une transaction valide**

![Diagramme Merkle Tree](images/image.png)

```
Original Merkle Root: d8240269a716f4371fa47ab3ce2fed8da7e68179f756a2b2e9987594ab65d1e0

Choose option: 1
Enter transaction number to verify (1-8): 3

===== Result =====
Calculated Merkle Root : d8240269a716f4371fa47ab3ce2fed8da7e68179f756a2b2e9987594ab65d1e0
Original Merkle Root   : d8240269a716f4371fa47ab3ce2fed8da7e68179f756a2b2e9987594ab65d1e0
✅ PROOF VALID: T3 is in the original tree.
```

➡️ La transaction T3 est intacte, le hachage est cohérent avec la racine initiale.

---

#### 🔹 Cas 2 : Transaction modifiée

**Merkle Proof d'une transaction invalide**

![Diagramme Merkle Tree](images/photo2.png)
![Diagramme Merkle Tree](images/photo3.png)

```
Choose option: 2
Enter transaction number to modify (1-8): 4
Current value: T4
Enter new value: blockchain

Choose option: 1
Enter transaction number to verify (1-8): 4

===== Result =====
Calculated Merkle Root : 0c30c5f08219b748e871bb4c1b04e5b062be72f69d5dfee4126b95acde4b51c9
Original Merkle Root   : d8240269a716f4371fa47ab3ce2fed8da7e68179f756a2b2e9987594ab65d1e0
❌ PROOF INVALID: blockchain has been modified!
```

➡️ La transaction T4 a été altérée, donc la racine calculée diffère de la racine originale.

---

### 🧩 **Résultat attendu**

* Affichage de la **structure symbolique** de l’arbre.
* Possibilité de **vérifier** une transaction.
* Détection immédiate d’une **modification**.
* Maintien de la racine originale pour assurer la **traçabilité**.

