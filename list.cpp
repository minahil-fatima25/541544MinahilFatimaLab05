// list.cpp, menu-driven singly linked list
#include <iostream>
#include <limits>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int value) : data(value), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;

public:
    LinkedList() : head(nullptr) {}        

    ~LinkedList() { destroyList(); }         //no node survives the list

    // Copying would leave two lists owning the same node
    LinkedList(const LinkedList&)            = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    // 1) new node becomes the head, old head hangs off it
    void insertAtHead(int value) {
        Node* node = new Node(value);
        node->next = head;
        head = node;
        cout << "  inserted " << value << " at the head\n";
    }

    void insertAtEnd(int value) {
        Node* node = new Node(value);
        if (head == nullptr) {
            head = node;
        } else {
            Node* cur = head;
            while (cur->next != nullptr) cur = cur->next;
            cur->next = node;
        }
        cout << "  inserted " << value << " at the end\n";
    }

    // 2) sits after the 2nd node, so we need at least 2 nodes to begin with
    void insertAtThird(int value) {
        if (head == nullptr || head->next == nullptr) {
            cout << "  fewer than 2 nodes, so adding it at the end instead\n";
            insertAtEnd(value);
            return;
        }
        Node* second = head->next;
        Node* node = new Node(value);
        node->next = second->next;
        second->next = node;
        cout << "  inserted " << value << " at position 3\n";
    }

    // 3) walk from head to tail, printing as we go
    void displayList() const {
        if (head == nullptr) {
            cout << "  list is empty\n";
            return;
        }
        cout << "  ";
        for (Node* cur = head; cur != nullptr; cur = cur->next)
            cout << cur->data << " -> ";
        cout << "NULL\n";
    }

    // 4) stop at the second-to-last node so we can null its link
    void deleteLast() {
        if (head == nullptr) {
            cout << "  nothing to delete, list is empty\n";
            return;
        }
        if (head->next == nullptr) {          // only one node left
            cout << "  deleted " << head->data << ", list is now empty\n";
            delete head;
            head = nullptr;
            return;
        }
        Node* prev = head;
        while (prev->next->next != nullptr) prev = prev->next;
        cout << "  deleted " << prev->next->data << " from the end\n";
        delete prev->next;
        prev->next = nullptr;
    }

    // 5)
    int countNodes() const {
        int count = 0;
        for (Node* cur = head; cur != nullptr; cur = cur->next) ++count;
        return count;
    }

    // 6) flip each link backwards, one node at a time
    void reverseList() {
        Node* prev = nullptr;
        Node* cur = head;
        while (cur != nullptr) {
            Node* next = cur->next;           // remember it before we overwrite
            cur->next = prev;
            prev = cur;
            cur = next;
        }
        head = prev;
        cout << "  list reversed\n";
    }

    // 7) returns the 1-based position, or 0 when the value isn't there
    int searchValue(int value) const {
        int pos = 1;
        for (Node* cur = head; cur != nullptr; cur = cur->next, ++pos)
            if (cur->data == value) return pos;
        return 0;
    }

    void destroyList() {
        while (head != nullptr) {
            Node* next = head->next;
            delete head;
            head = next;
        }
    }
};

// Keeps the menu alive if someone types letters instead of a number.
int readInt(const char* prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) return value;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  please enter a number\n";
    }
}

int main() {
    LinkedList list;
    bool running = true;

    while (running) {
        cout << "\n===== Singly Linked List =====\n"
             << "1. Insert at head\n"
             << "2. Insert at 3rd position\n"
             << "3. Display list\n"
             << "4. Delete last node\n"
             << "5. Count nodes\n"
             << "6. Reverse list\n"
             << "7. Search for a value\n"
             << "8. Exit\n";

        int choice = readInt("Choice: ");

        switch (choice) {
            case 1:
                list.insertAtHead(readInt("Value: "));
                list.displayList();
                break;

            case 2:
                list.insertAtThird(readInt("Value: "));
                list.displayList();
                break;

            case 3:
                list.displayList();
                break;

            case 4:
                list.deleteLast();
                list.displayList();
                break;

            case 5:
                cout << "  the list has " << list.countNodes() << " node(s)\n";
                break;

            case 6:
                list.reverseList();
                list.displayList();
                break;

            case 7: {
                int value = readInt("Value to find: ");
                int pos = list.searchValue(value);
                if (pos > 0)
                    cout << "  found " << value << " at position " << pos << '\n';
                else
                    cout << "  " << value << " is not in the list\n";
                break;
            }

            case 8:
                running = false;
                break;

            default:
                cout << "  pick a number between 1 and 8\n";
        }
    }

    cout << "\nFreeing every node before exit.\n";
    return 0;   // destructor runs here
}

