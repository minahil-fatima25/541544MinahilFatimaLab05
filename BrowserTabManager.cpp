//Lab task # 1
//Implement a browser tab manager using a Circular Doubly Linked List. 

#include <iostream>
#include <limits>
#include <string>
using namespace std;

/*thrown if input runs out,so main can exit normally and the destructor
still frees the tabs*/
struct InputEnded {};
int readInt(const string& prompt,
            int minValue = numeric_limits<int>::min(),
            int maxValue = numeric_limits<int>::max()) {
    while (true) {
        cout << prompt;
        int value;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (value >= minValue && value <= maxValue) return value;
            cout << "  Value must be between " << minValue << " and " << maxValue << ".\n";
        } else {
            if (cin.eof()) throw InputEnded{};
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  Invalid input. Please enter a whole number.\n";
        }
    }
}
//It Reads a full line where (titles can have spaces)and trims it
string readLine(const string& prompt) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) throw InputEnded{};
        size_t first = line.find_first_not_of(" \t\r");
        if (first != string::npos) {
            size_t last = line.find_last_not_of(" \t\r");
            return line.substr(first, last - first + 1);
        }
        cout << "Input cannot be empty.\n";
    }
}

struct Tab {
    int    id;
    string title;
    string url;
    Tab*   next;
    Tab*   prev;

    Tab(int tabId, const string& tabTitle, const string& tabUrl)
        : id(tabId), title(tabTitle), url(tabUrl), next(nullptr), prev(nullptr) {}
};

class TabManager {
public:
    TabManager() : current(nullptr), size(0), nextId(1) {}
    ~TabManager() { clear(); }
    // Copying would leave two managers deleting the same nodes.
    TabManager(const TabManager&)            = delete;
    TabManager& operator=(const TabManager&) = delete;

    bool isEmpty() const { return current == nullptr; }
    // New tab goes right after the current one. IDs are handed out
    // automatically so they never clash.
    void openTab(const string& title, const string& url) {
        Tab* tab = new Tab(nextId++, title, url);

        if (isEmpty()) {
            // a single node just points to itself both ways
            tab->next = tab;
            tab->prev = tab;
            current   = tab;
        } else {
            tab->next = current->next;
            tab->prev = current;
            current->next->prev = tab;
            current->next = tab;
        }
        ++size;
        cout << "  Opened tab #" << tab->id << " (\"" << tab->title << "\").\n";
    }
    // Removes the current tab and moves focus to the next one.
    void closeCurrentTab() {
        if (isEmpty()) {
            cout << "  No tabs are open.\n";
            return;
        }

        Tab* victim = current;
        int  closedId = victim->id;

        if (victim->next == victim) {
            current = nullptr;   // that was the last tab
        } else {
            victim->prev->next = victim->next;
            victim->next->prev = victim->prev;
            current = victim->next;
        }

        delete victim;
        --size;

        cout << "  Closed tab #" << closedId << ".";
        if (isEmpty()) cout << " No tabs remain open.\n";
        else           cout << " Active tab is now #" << current->id << ".\n";
    }

    void moveNext() {
        if (isEmpty()) { cout << "  No tabs are open.\n"; return; }
        current = current->next;
        displayCurrent();
    }

    void movePrevious() {
        if (isEmpty()) { cout << "  No tabs are open.\n"; return; }
        current = current->prev;
        displayCurrent();
    }

    void displayCurrent() const {
        if (isEmpty()) { cout << "  No tabs are open.\n"; return; }
        cout << "  Current tab:\n";
        printTab(current, true);
    }

    // Both displays stop once we're back at the tab we started from.
    void displayForward() const {
        if (isEmpty()) { cout << "  No tabs are open.\n"; return; }
        cout << "  Tabs (forward, " << size << " open):\n";
        const Tab* node = current;
        do {
            printTab(node, node == current);
            node = node->next;
        } while (node != current);
    }

    void displayBackward() const {
        if (isEmpty()) { cout << "  No tabs are open.\n"; return; }
        cout << "  Tabs (backward, " << size << " open):\n";
        const Tab* node = current;
        do {
            printTab(node, node == current);
            node = node->prev;
        } while (node != current);
    }

    void search(int id) const {
        const Tab* tab = find(id);
        if (tab == nullptr) {
            cout << "  Tab #" << id << " not found.\n";
            return;
        }
        cout << "  Tab found:\n";
        printTab(tab, tab == current);
    }

    // Sanity check: every node's neighbours should point back at it, and one
    // lap around the ring should take exactly `size` steps.
    bool verifyIntegrity() const {
        if (isEmpty()) return size == 0;
        const Tab* node = current;
        int steps = 0;
        do {
            if (node->next == nullptr || node->prev == nullptr) return false;
            if (node->next->prev != node || node->prev->next != node) return false;
            node = node->next;
            if (++steps > size) return false;   // stops us looping forever on a broken ring
        } while (node != current);
        return steps == size;
    }

private:
    Tab* current;
    int  size;
    int  nextId;

    Tab* find(int id) const {
        if (isEmpty()) return nullptr;
        Tab* node = current;
        do {
            if (node->id == id) return node;
            node = node->next;
        } while (node != current);
        return nullptr;
    }

    static void printTab(const Tab* tab, bool isCurrent) {
        cout << (isCurrent ? "  -> " : "     ")
             << "[ID " << tab->id << "] " << tab->title
             << "  |  " << tab->url << '\n';
    }

    // Delete everything after current until we loop back, then current itself.
    void clear() {
        if (isEmpty()) return;
        Tab* node = current->next;
        while (node != current) {
            Tab* following = node->next;
            delete node;
            node = following;
        }
        delete current;
        current = nullptr;
        size = 0;
    }
};

void printMenu() {
    cout << "\nBrowser Tab Manager\n"
         << " 1. Open New Tab\n"
         << " 2. Close Current Tab\n"
         << " 3. Move Next\n"
         << " 4. Move Previous\n"
         << " 5. Display Current Tab\n"
         << " 6. Display All Tabs Forward\n"
         << " 7. Display All Tabs Backward\n"
         << " 8. Search Tab by ID\n"
         << " 9. Verify CDLL Integrity\n"
         << " 0. Exit\n";
}

int main() {
    TabManager tabs;

    try {
        while (true) {
            printMenu();
            int choice = readInt("Enter choice: ", 0, 9);

            switch (choice) {
                case 1: {
                    string title = readLine("  Website title: ");
                    string url   = readLine("  URL: ");
                    tabs.openTab(title, url);
                    break;
                }
                case 2: tabs.closeCurrentTab();  break;
                case 3: tabs.moveNext();         break;
                case 4: tabs.movePrevious();     break;
                case 5: tabs.displayCurrent();   break;
                case 6: tabs.displayForward();   break;
                case 7: tabs.displayBackward();  break;
                case 8: tabs.search(readInt("  Tab ID to search: ")); break;
                case 9:
                    cout << (tabs.verifyIntegrity()
                             ? "  CDLL is valid: circular and doubly linked.\n"
                             : "  CDLL integrity check FAILED.\n");
                    break;
                case 0:
                    cout << "Closing browser. Goodbye!\n";
                    return 0;
            }
        }
    } catch (const InputEnded&) {
        cout << "\nInput ended. Exiting.\n";
    }
    return 0;
}