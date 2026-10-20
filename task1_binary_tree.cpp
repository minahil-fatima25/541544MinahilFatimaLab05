//Task 1: Binary Tree Representation and Operations

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <cstdlib>
using namespace std;

//node definitions

//this struct is doing the job of one node of the string based binary tree, implementing it with a string label and two child pointers
struct TreeNode {
    string data;
    TreeNode* left;
    TreeNode* right;
    TreeNode(const string& value) : data(value), left(nullptr), right(nullptr) {}
};

//this struct is doing the job of one node of the integer BST, implementing it with an int key and two child pointers
struct BSTNode {
    int key;
    BSTNode* left;
    BSTNode* right;
    BSTNode(int value) : key(value), left(nullptr), right(nullptr) {}
};

//small helper functions

//this function is removing all spaces from the user input, implementing it with the erase-remove idiom so "a(b, c)" behaves like "a(b,c)"
//time complexity: O(n), space complexity: O(n)
string removeSpaces(const string& text) {
    string cleaned = text;
    cleaned.erase(remove_if(cleaned.begin(), cleaned.end(),
                  [](unsigned char c) { return isspace(c); }), cleaned.end());
    return cleaned;
}

//this function is printing a labelled list on one line, implementing it with a simple loop over the vector
//time complexity: O(k) for k items, space complexity: O(1)
void printList(const string& label, const vector<string>& items) {
    cout << label;
    for (size_t i = 0; i < items.size(); i++) {
        cout << items[i] << (i + 1 < items.size() ? " " : "");
    }
    cout << endl;
}

//part 1: validate the string WITHOUT building the tree

//this function is checking if the string is a legal tree, implementing it with a stack of comma counters (one counter per open bracket)
//rules it enforces: balanced brackets, '(' only after a node name, exactly one comma per bracket pair, no junk after the root
//time complexity: O(n), space complexity: O(d) where d is the bracket nesting depth (stack of comma counters)
bool validateTreeString(const string& s, string& reason) {
    if (s.empty()) {
        reason = "the string is empty";
        return false;
    }

    vector<int> commasInBracket;   //the stack, top = commas seen in the innermost open bracket
    char previous = 0;             //0 = start of string, 'N' = a node name just ended, otherwise the last symbol
    size_t i = 0;

    while (i < s.size()) {
        char c = s[i];
        string where = " at position " + to_string(i + 1);

        if (isalnum(static_cast<unsigned char>(c))) {
            //a node name may only appear at the very start or right after '(' or ','
            if (!(previous == 0 || previous == '(' || previous == ',')) {
                reason = "unexpected node name" + where + " (a child must follow '(' or ',')";
                return false;
            }
            while (i < s.size() && isalnum(static_cast<unsigned char>(s[i]))) i++;
            previous = 'N';
            continue;
        }

        if (c == '(') {
            if (previous != 'N') {
                reason = "'(' must come directly after a node name" + where;
                return false;
            }
            commasInBracket.push_back(0);
        } else if (c == ',') {
            if (commasInBracket.empty()) {
                reason = "comma outside of any brackets" + where;
                return false;
            }
            if (commasInBracket.back() != 0) {
                reason = "a binary tree node cannot have more than two children" + where;
                return false;
            }
            commasInBracket.back()++;
        } else if (c == ')') {
            if (commasInBracket.empty()) {
                reason = "closing bracket without a matching '('" + where;
                return false;
            }
            if (commasInBracket.back() != 1) {
                reason = "a bracket pair must hold exactly two child slots separated by one comma" + where;
                return false;
            }
            commasInBracket.pop_back();
        } else {
            reason = string("illegal character '") + c + "'" + where;
            return false;
        }

        previous = c;
        i++;
    }

    if (!commasInBracket.empty()) {
        reason = "unbalanced brackets, some '(' were never closed";
        return false;
    }

    reason = "balanced brackets and correct child placement";
    return true;
}

//part 2: build the tree from the string

//this function is reading one subtree starting at pos, implementing it with recursive descent (name, then optional "(left,right)")
//an empty slot such as the left side of c(,f) simply returns nullptr
//time complexity: O(n), space complexity: O(h) for the recursion
TreeNode* parseNode(const string& s, size_t& pos) {
    if (pos >= s.size() || !isalnum(static_cast<unsigned char>(s[pos]))) {
        return nullptr;
    }

    size_t start = pos;
    while (pos < s.size() && isalnum(static_cast<unsigned char>(s[pos]))) pos++;
    TreeNode* node = new TreeNode(s.substr(start, pos - start));

    if (pos < s.size() && s[pos] == '(') {
        pos++;                          //skip '('
        node->left = parseNode(s, pos);
        pos++;                          //skip ','
        node->right = parseNode(s, pos);
        pos++;                          //skip ')'
    }
    return node;
}

//this function is the public entry for building the tree, implementing it by calling parseNode from position 0
//time complexity: O(n), space complexity: O(h)
TreeNode* buildTree(const string& s) {
    size_t pos = 0;
    return parseNode(s, pos);
}

//this function is freeing the whole tree, implementing it with a recursive postorder delete (children first, then the node)
//time complexity: O(n), space complexity: O(h)
void deleteTree(TreeNode* node) {
    if (node == nullptr) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

//part 3: traversals

//this function is generating the preorder form as a string, implementing it with recursion (node first, then left, then right)
//the output uses the same bracket notation as the input, so a(b(d,e),c(,f(g,))) comes back exactly like that
//time complexity: O(n*h) worst case because every level copies the text of its children (about O(n) for small or balanced trees), space complexity: O(n)
string preorderString(TreeNode* node) {
    if (node == nullptr) return "";
    string result = node->data;
    if (node->left != nullptr || node->right != nullptr) {
        result += "(" + preorderString(node->left) + "," + preorderString(node->right) + ")";
    }
    return result;
}

//this function is collecting plain preorder node names, implementing it with the classic recursive root-left-right walk
//time complexity: O(n), space complexity: O(h) for the recursion
void preorderList(TreeNode* node, vector<string>& out) {
    if (node == nullptr) return;
    out.push_back(node->data);
    preorderList(node->left, out);
    preorderList(node->right, out);
}

//this function is doing inorder traversal without recursion, implementing it with an explicit stack (go left as far as possible, pop, visit, go right)
//time complexity: O(n), space complexity: O(h) for the stack
vector<string> inorderIterative(TreeNode* root) {
    vector<string> result;
    stack<TreeNode*> pending;
    TreeNode* current = root;

    while (current != nullptr || !pending.empty()) {
        while (current != nullptr) {
            pending.push(current);
            current = current->left;
        }
        current = pending.top();
        pending.pop();
        result.push_back(current->data);
        current = current->right;
    }
    return result;
}

//this function is doing postorder traversal without recursion, implementing it with one stack plus a "last visited" pointer
//a node is only visited once its right child is done (or missing), which is what makes it left-right-root
//time complexity: O(n), space complexity: O(h) for the stack
vector<string> postorderIterative(TreeNode* root) {
    vector<string> result;
    stack<TreeNode*> pending;
    TreeNode* current = root;
    TreeNode* lastVisited = nullptr;

    while (current != nullptr || !pending.empty()) {
        if (current != nullptr) {
            pending.push(current);
            current = current->left;
        } else {
            TreeNode* top = pending.top();
            if (top->right != nullptr && lastVisited != top->right) {
                current = top->right;       //right subtree is still waiting
            } else {
                result.push_back(top->data);
                lastVisited = top;
                pending.pop();
            }
        }
    }
    return result;
}

//part 4: height and depth

//this function is finding the height of a tree in edges, implementing it recursively (empty tree = -1, a single node = 0)
//time complexity: O(n), space complexity: O(h)
int treeHeight(TreeNode* node) {
    if (node == nullptr) return -1;
    return 1 + max(treeHeight(node->left), treeHeight(node->right));
}

//this function is printing the depth of every node, implementing it with a recursive preorder walk that carries the current depth down
//time complexity: O(n), space complexity: O(h)
void printDepths(TreeNode* node, int depth) {
    if (node == nullptr) return;
    cout << "  node " << node->data << " -> depth " << depth
         << (node->left == nullptr && node->right == nullptr ? "  (leaf)" : "") << endl;
    printDepths(node->left, depth + 1);
    printDepths(node->right, depth + 1);
}

//part 5: BST from a sequence of integers

//this function is inserting one key into the BST, implementing it recursively (smaller goes left, bigger goes right, duplicates are skipped)
//time complexity: O(h), which is O(log n) on average and O(n) for a skewed tree, space complexity: O(h)
BSTNode* insertBST(BSTNode* root, int key, bool& inserted) {
    if (root == nullptr) {
        inserted = true;
        return new BSTNode(key);
    }
    if (key < root->key) {
        root->left = insertBST(root->left, key, inserted);
    } else if (key > root->key) {
        root->right = insertBST(root->right, key, inserted);
    } else {
        inserted = false;       //same key already exists
    }
    return root;
}

//this function is building a BST from a list, implementing it by inserting the numbers in order so the first number becomes the root
//time complexity: O(m*h) for m values, so O(m log m) on average and O(m^2) in the worst case, space complexity: O(m)
BSTNode* buildBST(const vector<int>& values) {
    BSTNode* root = nullptr;
    for (int value : values) {
        bool inserted = false;
        root = insertBST(root, value, inserted);
        if (!inserted) {
            cout << "  note: " << value << " is a duplicate, skipped" << endl;
        }
    }
    return root;
}

//this function is collecting the BST keys in sorted order, implementing it with a recursive inorder walk
//time complexity: O(n), space complexity: O(h)
void inorderBST(BSTNode* node, vector<string>& out) {
    if (node == nullptr) return;
    inorderBST(node->left, out);
    out.push_back(to_string(node->key));
    inorderBST(node->right, out);
}

//this function is freeing the BST, implementing it with a recursive postorder delete
//time complexity: O(n), space complexity: O(h)
void deleteBST(BSTNode* node) {
    if (node == nullptr) return;
    deleteBST(node->left);
    deleteBST(node->right);
    delete node;
}

//safe user input

//this function is reading one full line from the user, implementing it with getline and quitting politely if the input stream is closed
//time complexity: O(L) for a typed line of length L, space complexity: O(L)
string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        cout << endl << "input closed, exiting." << endl;
        exit(0);
    }
    return line;
}

//this function is turning a typed line into a list of integers, implementing it with a stringstream and stoi, and it rejects words and half numbers like 12abc
//time complexity: O(L) for a line of length L, space complexity: O(m) for m numbers
bool readIntegers(const string& line, vector<int>& values) {
    values.clear();
    istringstream stream(line);
    string token;
    while (stream >> token) {
        try {
            size_t used = 0;
            int value = stoi(token, &used);
            if (used != token.size()) throw invalid_argument("extra characters");
            values.push_back(value);
        } catch (...) {
            cout << "  '" << token << "' is not a whole number, please try again." << endl;
            return false;
        }
    }
    if (values.empty()) {
        cout << "  you did not type any numbers, please try again." << endl;
        return false;
    }
    return true;
}

//menu actions

//this function is running every tree operation on one string, implementing it by validating first and then building, traversing and measuring the tree only if the string is valid
//time complexity: O(n) for the tree operations (preorderString can reach O(n*h)), space complexity: O(n)
void runTreeOperations(const string& treeText) {
    cout << "\n  input string : " << treeText << endl << endl;

    //step 1: validate first, so we never try to build a broken tree
    cout << "[1] validating the string" << endl;
    string reason;
    bool valid = validateTreeString(treeText, reason);
    cout << "  result: " << (valid ? "VALID" : "INVALID") << " (" << reason << ")" << endl;
    if (!valid) {
        cout << "  the tree was not built because the string is not valid." << endl;
        return;
    }

    //step 2: build the tree
    TreeNode* root = buildTree(treeText);
    cout << endl << "[2] tree built from the string (root = " << root->data << ")" << endl << endl;

    //step 3: traversals
    cout << "[3] traversals" << endl;
    cout << "  preorder (recursive, string form) : " << preorderString(root) << endl;
    vector<string> pre;
    preorderList(root, pre);
    printList("  preorder (recursive, node list)   : ", pre);
    printList("  inorder  (iterative, stack)       : ", inorderIterative(root));
    printList("  postorder (iterative, stack)      : ", postorderIterative(root));
    cout << endl;

    //step 4: height and depth
    cout << "[4] height and depth" << endl;
    int height = treeHeight(root);
    cout << "  height of the tree = " << height << " edges (" << height + 1 << " levels)" << endl;
    printDepths(root, 0);

    deleteTree(root);
}

//this function is building a BST from a list of integers and showing it, implementing it with buildBST followed by a recursive inorder walk
//time complexity: O(m*h) to build plus O(m) to print, space complexity: O(m)
void runBSTDemo(const vector<int>& numbers) {
    cout << "\n  BST from the sequence:";
    for (int value : numbers) cout << " " << value;
    cout << endl;

    BSTNode* bstRoot = buildBST(numbers);
    vector<string> sorted;
    inorderBST(bstRoot, sorted);
    printList("  root = " + to_string(bstRoot->key) + ", inorder (sorted) : ", sorted);
    deleteBST(bstRoot);
}

//this function is printing the menu, implementing it with plain cout lines
//time complexity: O(1), space complexity: O(1)
void printMenu() {
    cout << endl;
    cout << "========== TASK 1: BINARY TREE ==========" << endl;
    cout << " 1. Run all operations on the lab string a(b(d,e),c(,f(g,)))" << endl;
    cout << " 2. Type my own tree string and run all operations" << endl;
    cout << " 3. Only validate a tree string (no tree is built)" << endl;
    cout << " 4. Build a BST from the lab sequence 50 30 70 20 40 60 80" << endl;
    cout << " 5. Type my own integers and build a BST" << endl;
    cout << " 0. Exit" << endl;
}

//this function is the program entry, implementing it with a menu loop that keeps asking until the user chooses 0
//time complexity: each menu action costs what its function costs above, the loop itself is O(1) per choice
int main() {
    while (true) {
        printMenu();
        string choice = readLine("choose an option: ");

        if (choice == "1") {
            runTreeOperations("a(b(d,e),c(,f(g,)))");
        } else if (choice == "2") {
            string text = removeSpaces(readLine("  type the tree string, e.g. a(b,c): "));
            runTreeOperations(text);
        } else if (choice == "3") {
            string text = removeSpaces(readLine("  type the tree string to check: "));
            string reason;
            bool valid = validateTreeString(text, reason);
            cout << "  " << (valid ? "VALID" : "INVALID") << " (" << reason << ")" << endl;
        } else if (choice == "4") {
            runBSTDemo({50, 30, 70, 20, 40, 60, 80});
        } else if (choice == "5") {
            vector<int> numbers;
            //keep asking until the numbers are usable
            while (!readIntegers(readLine("  type integers separated by spaces: "), numbers)) {}
            runBSTDemo(numbers);
        } else if (choice == "0") {
            cout << "goodbye!" << endl;
            break;
        } else {
            cout << "  invalid option, please type a number from 0 to 5." << endl;
        }
    }
    return 0;
}