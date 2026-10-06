//Lab Task 3
//Implement a train coach management system using a Circular Doubly Linked List. 
#include <iostream>
#include <limits>
#include <string>
#include <utility>
using namespace std;
//its thrown if input runs out, so main can exit normally and the destructor
//still frees the coaches.
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

//Reads a full line and trims it.
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
        cout << "  Input cannot be empty.\n";
    }
}
struct Coach {
    int    number;
    string type;
    int    capacity;
    int    passengers;
    Coach* next;
    Coach* prev;

    Coach(int coachNumber, const string& coachType, int cap, int pax)
        : number(coachNumber), type(coachType), capacity(cap), passengers(pax),
          next(nullptr), prev(nullptr) {}

    int availableSeats() const { return capacity - passengers; }
};
class Train {
public:
    Train() : head(nullptr), current(nullptr), size(0) {}
    ~Train() { clear(); }

    //Copying would leave two trains deleting the same nodes.
    Train(const Train&)            = delete;
    Train& operator=(const Train&) = delete;

    bool isEmpty() const { return head == nullptr; }
    bool contains(int number) const { return find(number) != nullptr; }

    //Adding at the end just means slotting in between the last coach and head.
    bool addCoach(int number, const string& type, int capacity, int passengers) {
        if (contains(number)) {
            cout << "  Coach " << number << " already exists. Not added.\n";
            return false;
        }
        Coach* coach = new Coach(number, type, capacity, passengers);

        if (isEmpty()) {
            coach->next = coach;
            coach->prev = coach;
            head    = coach;
            current = coach;
        } else {
            linkAfter(head->prev, coach);
        }
        ++size;
        cout << "  Coach " << number << " added at the end of the train.\n";
        return true;
    }

    bool insertAfter(int targetNumber, int number, const string& type, int capacity, int passengers) {
        Coach* target = find(targetNumber);
        if (target == nullptr) {
            cout << "  Coach " << targetNumber << " not found. Nothing inserted.\n";
            return false;
        }
        if (contains(number)) {
            cout << "  Coach " << number << " already exists. Not inserted.\n";
            return false;
        }
        linkAfter(target, new Coach(number, type, capacity, passengers));
        ++size;
        cout << "  Coach " << number << " inserted after coach " << targetNumber << ".\n";
        return true;
    }

    //if we remove the current coach, the next one takes over as current.
    void removeCoach(int number) {
        if (isEmpty()) { cout << "  The train has no coaches.\n"; return; }
        Coach* target = find(number);
        if (target == nullptr) {
            cout << "  Coach " << number << " not found.\n";
            return;
        }

        if (target->next == target) {
            head    = nullptr;   // that was the only coach
            current = nullptr;
        } else {
            target->prev->next = target->next;
            target->next->prev = target->prev;
            if (target == head)    head    = target->next;
            if (target == current) current = target->next;
        }
        delete target;
        --size;

        cout << "  Coach " << number << " removed.";
        if (isEmpty()) cout << " The train is now empty.\n";
        else           cout << " Current coach is now " << current->number << ".\n";
    }
    void moveForward() {
        if (isEmpty()) { cout << "  The train has no coaches.\n"; return; }
        current = current->next;
        displayCurrent();
    }
    void moveBackward() {
        if (isEmpty()) { cout << "  The train has no coaches.\n"; return; }
        current = current->prev;
        displayCurrent();
    }
    //Clockwise goes first to last using next, stopping when we're back at head.
    void displayClockwise() const {
        if (isEmpty()) { cout << "  The train has no coaches.\n"; return; }
        cout << "  Train clockwise (" << size << (size == 1 ? " coach" : " coaches") << ", first -> last):\n";
        const Coach* node = head;
        do {
            printCoach(node, node == current);
            node = node->next;
        } while (node != head);
    }
    //Anti-clockwise goes last to first using prev, stopping back at the last coach.
    void displayAntiClockwise() const {
        if (isEmpty()) { cout << "  The train has no coaches.\n"; return; }
        cout << "  Train anti-clockwise (" << size << (size == 1 ? " coach" : " coaches") << ", last -> first):\n";
        const Coach* start = head->prev;
        const Coach* node  = start;
        do {
            printCoach(node, node == current);
            node = node->prev;
        } while (node != start);
    }

    void search(int number) const {
        const Coach* coach = find(number);
        if (coach == nullptr) {
            cout << "  Coach " << number << " not found.\n";
            return;
        }
        cout << "  Coach found:\n";
        printCoach(coach, coach == current);
    }

    // One pass through the train. On a tie, the coach nearer the front wins.
    void findMaxAvailable() const {
        if (isEmpty()) { cout << "  The train has no coaches.\n"; return; }
        const Coach* best = head;
        const Coach* node = head->next;
        while (node != head) {
            if (node->availableSeats() > best->availableSeats()) best = node;
            node = node->next;
        }
        cout << "  Coach with the most empty seats (" << best->availableSeats() << "):\n";
        printCoach(best, best == current);
    }

    void displayCurrent() const {
        if (isEmpty()) { cout << "  The train has no coaches.\n"; return; }
        cout << "  Current coach:\n";
        printCoach(current, true);
    }

    //Flips every node's next and prev in place, so no coach data gets copied.
    //After the swap, the old next lives in prev, which is why we step with
    //prev. The old last coach (now head->next) becomes the new head.
    void reverse() {
        if (size < 2) {
            cout << (isEmpty() ? "  The train has no coaches.\n"
                               : "  Only one coach; direction is unchanged.\n");
            return;
        }
        Coach* node = head;
        do {
            swap(node->next, node->prev);
            node = node->prev;
        } while (node != head);

        head = head->next;
        cout << "  Train direction reversed. First coach is now " << head->number << ".\n";
    }

    // Sanity check: tail must wrap to head, every node's neighbours must point
    // back at it, and one lap should take exactly `size` steps.
    bool verifyIntegrity() const {
        if (isEmpty()) return size == 0 && current == nullptr;
        if (head->prev->next != head) return false;
        const Coach* node = head;
        int steps = 0;
        do {
            if (node->next == nullptr || node->prev == nullptr) return false;
            if (node->next->prev != node || node->prev->next != node) return false;
            node = node->next;
            if (++steps > size) return false;   // stops us looping forever on a broken ring
        } while (node != head);
        return steps == size;
    }

private:
    Coach* head;
    Coach* current;
    int    size;

    static void linkAfter(Coach* position, Coach* coach) {
        coach->next = position->next;
        coach->prev = position;
        position->next->prev = coach;
        position->next = coach;
    }

    Coach* find(int number) const {
        if (isEmpty()) return nullptr;
        Coach* node = head;
        do {
            if (node->number == number) return node;
            node = node->next;
        } while (node != head);
        return nullptr;
    }

    static void printCoach(const Coach* c, bool isCurrent) {
        cout << (isCurrent ? "  -> " : "     ")
             << "[Coach " << c->number << "] " << c->type
             << "  |  Passengers: " << c->passengers << "/" << c->capacity
             << "  |  Empty seats: " << c->availableSeats() << '\n';
    }

    //Delete everything after head until we loop back, then head itself.
    void clear() {
        if (isEmpty()) return;
        Coach* node = head->next;
        while (node != head) {
            Coach* following = node->next;
            delete node;
            node = following;
        }
        delete head;
        head    = nullptr;
        current = nullptr;
        size    = 0;
    }
};

//Makes sure the coach number is new and passengers don't exceed capacity.
void readCoach(const Train& train, int& number, string& type, int& capacity, int& passengers) {
    while (true) {
        number = readInt("  Coach number: ", 1);
        if (!train.contains(number)) break;
        cout << "  Coach " << number << " already exists. Enter a different number.\n";
    }
    type       = readLine("  Coach type (e.g. Economy, Business, Sleeper): ");
    capacity   = readInt("  Passenger capacity: ", 1);
    passengers = readInt("  Current passengers: ", 0, capacity);
}

void printMenu() {
    cout << "\nTrain Coach Navigation System\n"
         << "  1. Add Coach (at end)\n"
         << "  2. Insert Coach After a Coach Number\n"
         << "  3. Remove Coach by Number\n"
         << "  4. Move Forward\n"
         << "  5. Move Backward\n"
         << "  6. Display Train Clockwise\n"
         << "  7. Display Train Anti-clockwise\n"
         << "  8. Search Coach\n"
         << "  9. Find Maximum Available Capacity\n"
         << " 10. Display Current Coach\n"
         << " 11. Reverse Train Direction\n"
         << " 12. Verify CDLL Integrity\n"
         << "  0. Exit\n";
}

int main() {
    Train train;

    try {
        int n = readInt("Enter number of coaches: ", 0);
        for (int i = 1; i <= n; ++i) {
            cout << "\nCoach " << i << " of " << n << ":\n";
            int number, capacity, passengers; string type;
            readCoach(train, number, type, capacity, passengers);
            train.addCoach(number, type, capacity, passengers);
        }

        while (true) {
            printMenu();
            int choice = readInt("Enter choice: ", 0, 12);
            int number, capacity, passengers; string type;

            switch (choice) {
                case 1:
                    readCoach(train, number, type, capacity, passengers);
                    train.addCoach(number, type, capacity, passengers);
                    break;
                case 2: {
                    if (train.isEmpty()) {
                        cout << "  The train has no coaches. Use 'Add Coach' first.\n";
                        break;
                    }
                    // check the target exists before asking for all the details
                    int target = readInt("  Insert after coach number: ");
                    if (!train.contains(target)) {
                        cout << "  Coach " << target << " not found.\n";
                        break;
                    }
                    readCoach(train, number, type, capacity, passengers);
                    train.insertAfter(target, number, type, capacity, passengers);
                    break;
                }
                case 3:  train.removeCoach(readInt("  Coach number to remove: ")); break;
                case 4:  train.moveForward();          break;
                case 5:  train.moveBackward();         break;
                case 6:  train.displayClockwise();     break;
                case 7:  train.displayAntiClockwise(); break;
                case 8:  train.search(readInt("  Coach number to search: ")); break;
                case 9:  train.findMaxAvailable();     break;
                case 10: train.displayCurrent();       break;
                case 11: train.reverse();              break;
                case 12:
                    cout << (train.verifyIntegrity()
                             ? "  CDLL is valid: circular and doubly linked.\n"
                             : "  CDLL integrity check FAILED.\n");
                    break;
                case 0:
                    cout << "Exiting train system. Goodbye!\n";
                    return 0;
            }
        }
    } catch (const InputEnded&) {
        cout << "\nInput ended. Exiting.\n";
    }
    return 0;
}