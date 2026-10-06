//Lab task # 2
//Implement a photo album management system using a Circular Doubly Linked List.
#include <iostream>
#include <limits>
#include <string>
using namespace std;
//Thrown if input runs out, so main can exit normally and the destructor
//still frees the album.
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
//Reads a full line (names and places can have spaces) and trims it.
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

struct Photo {
    int    id;
    string name;
    string dateTaken;
    string location;
    Photo* next;
    Photo* prev;

    Photo(int photoId, const string& photoName, const string& date, const string& place)
        : id(photoId), name(photoName), dateTaken(date), location(place),
          next(nullptr), prev(nullptr) {}
};

class PhotoAlbum {
public:
    PhotoAlbum() : head(nullptr), current(nullptr), size(0) {}
    ~PhotoAlbum() { clear(); }
    // Copying would leave two albums deleting the same nodes.
    PhotoAlbum(const PhotoAlbum&)            = delete;
    PhotoAlbum& operator=(const PhotoAlbum&) = delete;
    bool isEmpty() const { return head == nullptr; }
    bool contains(int id) const { return find(id) != nullptr; }
    //Adding at the end just means slotting in between the tail and head.
    bool addPhoto(int id, const string& name, const string& date, const string& location) {
        if (contains(id)) {
            cout << "  Photo ID " << id << " already exists. Not added.\n";
            return false;
        }
        Photo* photo = new Photo(id, name, date, location);

        if (isEmpty()) {
            makeSingleNode(photo);
        } else {
            insertAfter(head->prev, photo);
        }
        cout << "  Photo " << id << " added at the end of the album.\n";
        return true;
    }

//If current happens to be the tail, the new photo becomes the new tail
 //on its own since head doesn't change.
    bool insertAfterCurrent(int id, const string& name, const string& date, const string& location) {
        if (contains(id)) {
            cout << "  Photo ID " << id << " already exists. Not inserted.\n";
            return false;
        }
        Photo* photo = new Photo(id, name, date, location);

        if (isEmpty()) {
            makeSingleNode(photo);
            cout << "  Album was empty; photo " << id << " is now the first and current photo.\n";
        } else {
            insertAfter(current, photo);
            cout << "  Photo " << id << " inserted after current photo " << current->id << ".\n";
        }
        return true;
    }

    void removeById(int id) {
        if (isEmpty()) { cout << "  The album is empty.\n"; return; }
        Photo* target = find(id);
        if (target == nullptr) {
            cout << "  Photo ID " << id << " not found.\n";
            return;
        }
        unlinkAndDelete(target);
        cout << "  Photo " << id << " removed.";
        reportCurrent();
    }

    void removeCurrent() {
        if (isEmpty()) { cout << "  The album is empty.\n"; return; }
        int id = current->id;
        unlinkAndDelete(current);
        cout << "  Current photo " << id << " removed.";
        reportCurrent();
    }

    void moveNext() {
        if (isEmpty()) { cout << "  The album is empty.\n"; return; }
        current = current->next;
        displayCurrent();
    }

    void movePrevious() {
        if (isEmpty()) { cout << "  The album is empty.\n"; return; }
        current = current->prev;
        displayCurrent();
    }

    void displayCurrent() const {
        if (isEmpty()) { cout << "  The album is empty.\n"; return; }
        cout << "  Current photo:\n";
        printPhoto(current, true);
    }

    // Both displays start at current and stop once they get back to it.
    void displayForward() const {
        if (isEmpty()) { cout << "  The album is empty.\n"; return; }
        cout << "  Album (forward from current):\n";
        const Photo* node = current;
        do {
            printPhoto(node, node == current);
            node = node->next;
        } while (node != current);
    }

    void displayBackward() const {
        if (isEmpty()) { cout << "  The album is empty.\n"; return; }
        cout << "  Album (backward from current):\n";
        const Photo* node = current;
        do {
            printPhoto(node, node == current);
            node = node->prev;
        } while (node != current);
    }

    void search(int id) const {
        const Photo* photo = find(id);
        if (photo == nullptr) {
            cout << "  Photo ID " << id << " not found.\n";
            return;
        }
        cout << "  Photo found:\n";
        printPhoto(photo, photo == current);
    }

    // Counted by actually walking one lap of the ring.
    int countPhotos() const {
        if (isEmpty()) return 0;
        int count = 0;
        const Photo* node = head;
        do {
            ++count;
            node = node->next;
        } while (node != head);
        return count;
    }

    //Sanity check: tail must wrap to head, every node's neighbours must point
    //back at it, and a lap in either direction should take `size` steps.
    bool verifyIntegrity() const {
        if (isEmpty()) return size == 0 && current == nullptr;
        if (current == nullptr) return false;
        if (head->prev->next != head) return false;

        int forward = 0, backward = 0;
        const Photo* node = head;
        do {
            if (node->next == nullptr || node->prev == nullptr) return false;
            if (node->next->prev != node || node->prev->next != node) return false;
            node = node->next;
            if (++forward > size) return false;   // stops us looping forever on a broken ring
        } while (node != head);

        node = head;
        do {
            node = node->prev;
            if (++backward > size) return false;
        } while (node != head);

        return forward == size && backward == size;
    }

private:
    Photo* head;
    Photo* current;
    int    size;   // only used by the integrity check

    void makeSingleNode(Photo* photo) {
        photo->next = photo;
        photo->prev = photo;
        head    = photo;
        current = photo;
        ++size;
    }

    void insertAfter(Photo* position, Photo* photo) {
        photo->next = position->next;
        photo->prev = position;
        position->next->prev = photo;
        position->next = photo;
        ++size;
    }

    //Unhooks the node and frees it. If it was head or current, those slide
    //forward to the next photo.
    void unlinkAndDelete(Photo* node) {
        if (node->next == node) {
            head    = nullptr;   // that was the only photo
            current = nullptr;
        } else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            if (node == head)    head    = node->next;
            if (node == current) current = node->next;
        }
        delete node;
        --size;
    }

    Photo* find(int id) const {
        if (isEmpty()) return nullptr;
        Photo* node = head;
        do {
            if (node->id == id) return node;
            node = node->next;
        } while (node != head);
        return nullptr;
    }

    void reportCurrent() const {
        if (isEmpty()) cout << " The album is now empty.\n";
        else           cout << " Current photo is now " << current->id << ".\n";
    }

    static void printPhoto(const Photo* p, bool isCurrent) {
        cout << (isCurrent ? "  -> " : "     ")
             << "[ID " << p->id << "] " << p->name
             << "  |  Date: " << p->dateTaken
             << "  |  Location: " << p->location << '\n';
    }

    //Delete everything after head until we loop back, then head itself.
    void clear() {
        if (isEmpty()) return;
        Photo* node = head->next;
        while (node != head) {
            Photo* following = node->next;
            delete node;
            node = following;
        }
        delete head;
        head    = nullptr;
        current = nullptr;
        size    = 0;
    }
};

//Keeps asking for an ID until it gets one that isn't already taken.
void readPhoto(const PhotoAlbum& album, int& id, string& name, string& date, string& location) {
    while (true) {
        id = readInt("  Photo ID: ", 1);
        if (!album.contains(id)) break;
        cout << "  Photo ID " << id << " already exists. Enter a different ID.\n";
    }
    name     = readLine("  Photo name: ");
    date     = readLine("  Date taken (e.g. 2026-09-29): ");
    location = readLine("  Location: ");
}

void printMenu() {
    cout << "\nCircular Photo Album\n"
         << "  1. Add Photo (at end)\n"
         << "  2. Insert Photo After Current\n"
         << "  3. Remove Photo by ID\n"
         << "  4. Remove Current Photo\n"
         << "  5. Move Next\n"
         << "  6. Move Previous\n"
         << "  7. Display Album Forward\n"
         << "  8. Display Album Backward\n"
         << "  9. Search Photo by ID\n"
         << " 10. Count Photos\n"
         << " 11. Display Current Photo\n"
         << " 12. Verify CDLL Integrity\n"
         << "  0. Exit\n";
}

int main() {
    PhotoAlbum album;

    try {
        int n = readInt("Enter number of photos: ", 0);
        for (int i = 1; i <= n; ++i) {
            cout << "\nPhoto " << i << " of " << n << ":\n";
            int id; string name, date, location;
            readPhoto(album, id, name, date, location);
            album.addPhoto(id, name, date, location);
        }

        while (true) {
            printMenu();
            int choice = readInt("Enter choice: ", 0, 12);
            int id; string name, date, location;

            switch (choice) {
                case 1:
                    readPhoto(album, id, name, date, location);
                    album.addPhoto(id, name, date, location);
                    break;
                case 2:
                    readPhoto(album, id, name, date, location);
                    album.insertAfterCurrent(id, name, date, location);
                    break;
                case 3:  album.removeById(readInt("  Photo ID to remove: ")); break;
                case 4:  album.removeCurrent();    break;
                case 5:  album.moveNext();         break;
                case 6:  album.movePrevious();     break;
                case 7:  album.displayForward();   break;
                case 8:  album.displayBackward();  break;
                case 9:  album.search(readInt("  Photo ID to search: ")); break;
                case 10: cout << "  Total photos in album: " << album.countPhotos() << '\n'; break;
                case 11: album.displayCurrent();   break;
                case 12:
                    cout << (album.verifyIntegrity()
                             ? "  CDLL is valid: circular and doubly linked.\n"
                             : "  CDLL integrity check FAILED.\n");
                    break;
                case 0:
                    cout << "Exiting photo album. Goodbye!\n";
                    return 0;
            }
        }
    } catch (const InputEnded&) {
        cout << "\nInput ended. Exiting.\n";
    }
    return 0;
}