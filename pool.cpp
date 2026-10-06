//stringpoolsimple.cpp, find and fix a leak in a small string pool

#include <iostream>
#include <string>

using namespace std;

class StringPool {
private:
    string* stringPool;
    int currentSize;
    int maxSize;

public:
    StringPool() : stringPool(nullptr), currentSize(0), maxSize(5) {
        stringPool = new string[maxSize];
        cout << "Pool ready, capacity " << maxSize << ".\n";
    }

    ~StringPool() {
        delete[] stringPool;                  //If we forget this and the whole array leaks
        stringPool = nullptr;
        cout << "Pool destroyed.\n";
    }

    // Copying would delete the same array twice.
    StringPool(const StringPool&)            = delete;
    StringPool& operator=(const StringPool&) = delete;

    bool addString(const string& text) {
        if (currentSize >= maxSize) {
            cout << "  ! full, rejected \"" << text << "\"\n";
            return false;
        }
        stringPool[currentSize] = text;
        cout << "  + [" << currentSize << "] \"" << text << "\"\n";
        ++currentSize;
        return true;
    }

    // The bug: the count drops, but the copy left in the tail slot still owns
    // its buffer. Nobody can reach that text again and nobody frees it.
    bool removeString(int index) {
        if (index < 0 || index >= currentSize) {
            cout << "  ! bad index " << index << '\n';
            return false;
        }
        cout << "  - [" << index << "] \"" << stringPool[index]
                  << "\" (not freed)\n";
        for (int i = index; i < currentSize - 1; ++i)
            stringPool[i] = stringPool[i + 1];
        --currentSize;
        return true;
    }

    // How much memory is stuck in slots past currentSize?
    size_t detectLeak() const {
        size_t leaked = 0;
        int slots = 0;
        for (int i = currentSize; i < maxSize; ++i) {
            if (!stringPool[i].empty()) {
                leaked += stringPool[i].capacity();
                ++slots;
            }
        }
        if (leaked == 0) {
            cout << "  leak check: clean\n";
        } else {
            cout << "  leak check: " << slots << " stale slot(s), ~"
                      << leaked << " bytes unreachable\n";
            for (int i = currentSize; i < maxSize; ++i)
                if (!stringPool[i].empty())
                    cout << "      slot " << i << " -> \"" << stringPool[i]
                              << "\" (" << stringPool[i].capacity() << " bytes)\n";
        }
        return leaked;
    }

    // Hand the orphaned buffers back to the heap.
    void fixLeak() {
        for (int i = currentSize; i < maxSize; ++i) {
            if (!stringPool[i].empty()) {
                cout << "  * freeing slot " << i << '\n';
                stringPool[i].clear();
                stringPool[i].shrink_to_fit();
            }
        }
    }

    // Same as removeString, but cleans up as it goes. Use this one for real.
    bool removeStringSafely(int index) {
        if (index < 0 || index >= currentSize) {
            cout << "  ! bad index " << index << '\n';
            return false;
        }
        cout << "  - [" << index << "] \"" << stringPool[index]
                  << "\" (freed)\n";
        for (int i = index; i < currentSize - 1; ++i)
            stringPool[i] = stringPool[i + 1];
        --currentSize;
        stringPool[currentSize].clear();
        stringPool[currentSize].shrink_to_fit();
        return true;
    }

    void displayStatus() const {
        cout << "  status: " << currentSize << '/' << maxSize << " in use\n";
        for (int i = 0; i < maxSize; ++i) {
            bool active = (i < currentSize);
            cout << "    [" << i << "] " << (active ? "used" : "free")
                      << "  \"" << stringPool[i] << "\"  cap="
                      << stringPool[i].capacity();
            if (!active && !stringPool[i].empty()) cout << "   <-- leaked";
            cout << '\n';
        }
    }

    int getCurrentSize() const { return currentSize; }
};

int main() {
    StringPool pool;

    // Long strings on purpose: short ones sit inside the string object
    // itself and never touch the heap, which would hide the leak.
    cout << "\n1) fill the pool\n";
    pool.addString("alpha-record-0001-payload-data");
    pool.addString("bravo-record-0002-payload-data");
    pool.addString("charlie-record-0003-payload-data");
    pool.addString("delta-record-0004-payload-data");
    pool.addString("echo-record-0005-payload-data");
    pool.addString("foxtrot-record-0006-overflow");
    pool.displayStatus();

    cout << "\n2) remove the buggy way\n";
    pool.removeString(4);
    pool.removeString(0);
    pool.displayStatus();

    cout << "\n3) detect\n";
    size_t leaked = pool.detectLeak();

    cout << "\n4) fix\n";
    pool.fixLeak();
    cout << "  reclaimed roughly " << leaked << " bytes\n";
    pool.detectLeak();
    pool.displayStatus();

    cout << "\n5) remove properly this time\n";
    pool.addString("golf-record-0007-payload-data");
    pool.removeStringSafely(0);
    pool.detectLeak();
    pool.displayStatus();

    cout << '\n';
    return 0;
}
