// Assignment 07: Binary Search Tree Operations (C++17, from scratch)
// No std::set / std::map / other tree containers are used.

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

struct Node {
    int key;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    explicit Node(int k) : key(k), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
public:
    BinarySearchTree() = default;
    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    // Returns true if inserted, false if the key already exists (no duplicates).
    bool insert(int key) { return insert(root_, key); }

    bool search(int key) const {
        const Node* cur = root_.get();
        while (cur) {
            if (key == cur->key) return true;
            cur = (key < cur->key) ? cur->left.get() : cur->right.get();
        }
        return false;
    }

    // Returns true if a node was removed, false if the key was not found.
    bool remove(int key) { return remove(root_, key); }

    std::vector<int> inorder() const {
        std::vector<int> out;
        inorder(root_.get(), out);
        return out;
    }
    std::vector<int> preorder() const {
        std::vector<int> out;
        preorder(root_.get(), out);
        return out;
    }
    std::vector<int> postorder() const {
        std::vector<int> out;
        postorder(root_.get(), out);
        return out;
    }

    // Height in edges: empty tree = -1, single node = 0.
    int height() const { return height(root_.get()); }

    bool empty() const { return root_ == nullptr; }

private:
    std::unique_ptr<Node> root_;

    static bool insert(std::unique_ptr<Node>& node, int key) {
        if (!node) {
            node = std::make_unique<Node>(key);
            return true;
        }
        if (key == node->key) return false;
        return insert(key < node->key ? node->left : node->right, key);
    }

    static const Node* minNode(const Node* n) {
        while (n->left) n = n->left.get();
        return n;
    }

    static bool remove(std::unique_ptr<Node>& node, int key) {
        if (!node) return false;  // key not found
        if (key < node->key) return remove(node->left, key);
        if (key > node->key) return remove(node->right, key);

        // Found the node to delete.
        if (!node->left && !node->right) {
            node.reset();                          // Case 1: leaf
        } else if (!node->left) {
            node = std::move(node->right);         // Case 2: only right child
        } else if (!node->right) {
            node = std::move(node->left);          // Case 2: only left child
        } else {
            // Case 3: two children. Replace with inorder successor
            // (smallest key in right subtree), then delete that successor.
            int succ = minNode(node->right.get())->key;
            node->key = succ;
            remove(node->right, succ);
        }
        return true;
    }

    static void inorder(const Node* n, std::vector<int>& out) {
        if (!n) return;
        inorder(n->left.get(), out);
        out.push_back(n->key);
        inorder(n->right.get(), out);
    }
    static void preorder(const Node* n, std::vector<int>& out) {
        if (!n) return;
        out.push_back(n->key);
        preorder(n->left.get(), out);
        preorder(n->right.get(), out);
    }
    static void postorder(const Node* n, std::vector<int>& out) {
        if (!n) return;
        postorder(n->left.get(), out);
        postorder(n->right.get(), out);
        out.push_back(n->key);
    }
    static int height(const Node* n) {
        if (!n) return -1;
        return 1 + std::max(height(n->left.get()), height(n->right.get()));
    }
};

// ---------------- Demo / test harness ----------------

static int g_failures = 0;

static void check(bool cond, const std::string& msg) {
    std::cout << (cond ? "  [PASS] " : "  [FAIL] ") << msg << "\n";
    if (!cond) ++g_failures;
}

static void print(const std::string& label, const std::vector<int>& v) {
    std::cout << label << ": ";
    for (std::size_t i = 0; i < v.size(); ++i)
        std::cout << v[i] << (i + 1 < v.size() ? " " : "");
    std::cout << "\n";
}

static void show(const BinarySearchTree& t) {
    print("  inorder  ", t.inorder());
    print("  preorder ", t.preorder());
    print("  postorder", t.postorder());
    std::cout << "  height   : " << t.height() << "\n";
}

int main() {
    BinarySearchTree t;
    std::cout << "=== Empty tree ===\n";
    check(t.height() == -1, "height of empty tree is -1");
    check(!t.search(5), "search on empty tree returns false");
    check(!t.remove(5), "remove on empty tree returns false");

    /* Insert 12 distinct integers. Resulting tree:
                    50
                 /      \
               30        70
              /  \      /  \
            20    40  60    80
           /  \         \     \
         10   25        65     90
                               /
                             85
    */
    std::cout << "\n=== Insertion ===\n";
    const std::vector<int> keys = {50, 30, 70, 20, 40, 60, 80, 10, 25, 65, 90, 85};
    for (int k : keys) t.insert(k);
    show(t);
    check(t.inorder() == std::vector<int>({10, 20, 25, 30, 40, 50, 60, 65, 70, 80, 85, 90}),
          "inorder is sorted after inserts");
    check(!t.insert(40), "duplicate insert (40) is rejected");
    check(t.inorder().size() == 12, "size still 12 after duplicate attempt");

    std::cout << "\n=== Search ===\n";
    check(t.search(65), "search(65) present -> true");
    check(!t.search(99), "search(99) absent  -> false");

    std::cout << "\n=== Remove: Case 1, leaf (10) ===\n";
    check(t.remove(10), "remove(10) returns true");
    show(t);
    check(t.inorder() == std::vector<int>({20, 25, 30, 40, 50, 60, 65, 70, 80, 85, 90}),
          "inorder still sorted, 10 gone");
    check(!t.search(10), "search(10) now false");

    std::cout << "\n=== Remove: Case 2, one child (60, has only right child 65) ===\n";
    check(t.remove(60), "remove(60) returns true");
    show(t);
    check(t.inorder() == std::vector<int>({20, 25, 30, 40, 50, 65, 70, 80, 85, 90}),
          "inorder still sorted, 60 gone");
    check(!t.search(60) && t.search(65), "60 gone, its child 65 still present");

    std::cout << "\n=== Remove: Case 3, two children (30; successor is 40) ===\n";
    check(t.remove(30), "remove(30) returns true");
    show(t);
    check(t.inorder() == std::vector<int>({20, 25, 40, 50, 65, 70, 80, 85, 90}),
          "inorder still sorted, 30 gone");
    check(t.preorder().at(1) == 40, "40 (inorder successor) took 30's position");

    std::cout << "\n=== Remove: root with two children (50; successor is 65) ===\n";
    check(t.remove(50), "remove(50) returns true");
    show(t);
    check(t.inorder() == std::vector<int>({20, 25, 40, 65, 70, 80, 85, 90}),
          "inorder still sorted, 50 gone");
    check(t.preorder().front() == 65, "65 is the new root");

    std::cout << "\n=== Remove absent key ===\n";
    check(!t.remove(999), "remove(999) returns false, tree unchanged");
    check(t.inorder().size() == 8, "size still 8");

    std::cout << "\n=== Edge case: worst-case (sorted inserts) height ===\n";
    BinarySearchTree degenerate;
    for (int i = 1; i <= 10; ++i) degenerate.insert(i);
    std::cout << "  height after inserting 1..10 in order: " << degenerate.height() << "\n";
    check(degenerate.height() == 9, "degenerate tree height is n-1 = 9");

    std::cout << "\n=== Average vs worst case height (n = 1000) ===\n";
    {
        // Deterministic shuffle: mt19937 engine output is fixed by the standard.
        std::vector<int> v(1000);
        for (int i = 0; i < 1000; ++i) v[i] = i;
        std::mt19937 rng(210);
        for (int i = 999; i > 0; --i) std::swap(v[i], v[rng() % (i + 1)]);
        BinarySearchTree randomTree, sortedTree;
        for (int k : v) randomTree.insert(k);
        for (int i = 0; i < 1000; ++i) sortedTree.insert(i);
        std::cout << "  random order inserts : height " << randomTree.height()
                  << "  (log2(1000) is about 10)\n";
        std::cout << "  sorted order inserts : height " << sortedTree.height()
                  << "  (n - 1 = 999)\n";
        check(randomTree.height() < 40, "random-order tree height is O(log n)-sized");
        check(sortedTree.height() == 999, "sorted-order tree height is n - 1");
    }

    std::cout << "\n=== Randomized stress test vs sorted std::vector reference ===\n";
    {
        BinarySearchTree s;
        std::vector<int> ref;  // sorted, distinct; plain vector, not a tree container
        std::mt19937 rng(7);
        bool ok = true;
        for (int step = 0; step < 5000 && ok; ++step) {
            int key = static_cast<int>(rng() % 200);
            int op = static_cast<int>(rng() % 3);
            auto it = std::lower_bound(ref.begin(), ref.end(), key);
            bool present = (it != ref.end() && *it == key);
            if (op == 0) {
                ok = (s.insert(key) == !present);
                if (!present) ref.insert(it, key);
            } else if (op == 1) {
                ok = (s.remove(key) == present);
                if (present) ref.erase(it);
            } else {
                ok = (s.search(key) == present);
            }
            if (step % 50 == 0 || !ok) ok = ok && (s.inorder() == ref);
        }
        check(ok && s.inorder() == ref, "5000 random insert/remove/search ops match reference");
    }

    std::cout << "\n=== Edge case: remove until empty ===\n";
    for (int k : t.inorder()) t.remove(k);
    check(t.empty() && t.height() == -1, "tree empty after removing every key");

    std::cout << "\n" << (g_failures == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED") << "\n";
    return g_failures == 0 ? 0 : 1;
}
