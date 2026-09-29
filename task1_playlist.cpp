//Task#1 
//Writing a program to implement a playlist management system using a doubly linked list
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
using namespace std;

//this is one song in the playlist holding its details and both links
struct Song
{
    int id;
    string name;
    int minutes;
    int seconds;
    Song* prev;
    Song* next;

    //this sets up a fresh song that has no neighbours yet
    Song(int songId, const string& songName, int mins, int secs)
        : id(songId), name(songName), minutes(mins), seconds(secs), prev(nullptr), next(nullptr) {}
};

class Playlist
{
private:
    Song* head;
    Song* tail;
    Song* current;
    int count;

    //this prints one song in a single neat line
    void printSong(const Song* s) const
    {
        cout << "ID: " << s->id << " | " << s->name << " | "
             << s->minutes << ":" << (s->seconds < 10 ? "0" : "") << s->seconds << endl;
    }

public:
    //this starts the playlist off completely empty
    Playlist() : head(nullptr), tail(nullptr), current(nullptr), count(0) {}

    //this blocks copying so two playlists never end up freeing the same nodes
    Playlist(const Playlist&) = delete;
    Playlist& operator=(const Playlist&) = delete;

    bool isEmpty() const { return head == nullptr; }
    int size() const { return count; }

    //this walks from the front and hands back the song whose id matches
    Song* search(int id) const
    {
        Song* temp = head;
        while (temp != nullptr)
        {
            if (temp->id == id)
                return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    //this prints the details of a song after looking it up by id
    void showSearchResult(int id) const
    {
        Song* found = search(id);
        if (found == nullptr)
        {
            cout << "Song with ID " << id << " not found." << endl;
            return;
        }
        cout << "Song found: ";
        printSong(found);
    }

    bool addSong(int id, const string& name, int mins, int secs)
    {
        //this stops two songs from sharing the same id
        if (search(id) != nullptr)
            return false;

        //this allocates the new song on the heap
        Song* newSong = new Song(id, name, mins, secs);

        if (head == nullptr)
        {
            //this makes the new song both first and last since the list was empty
            head = tail = newSong;
            current = newSong;
        }
        else
        {
            //this hooks the new song after the old tail and moves the tail forward
            tail->next = newSong;
            newSong->prev = tail;
            tail = newSong;
        }
        count++;
        return true;
    }

    bool deleteSong(int id)
    {
        Song* target = search(id);
        if (target == nullptr)
            return false;

        //this moves the player off the song that is about to be removed
        if (current == target)
            current = (target->next != nullptr) ? target->next : target->prev;

        //this links the song before the target straight to the song after it
        if (target->prev != nullptr)
            target->prev->next = target->next;
        else
            head = target->next;

        //this links the song after the target back to the song before it
        if (target->next != nullptr)
            target->next->prev = target->prev;
        else
            tail = target->prev;

        //closing this now by freeing the node and dereferencing the pointer
        delete target;
        target = nullptr;
        count--;
        return true;
    }

    //this goes from head to tail using the next links
    void displayForward() const
    {
        if (isEmpty())
        {
            cout << "Playlist is empty." << endl;
            return;
        }
        cout << "\nPlaylist (Forward) - " << count << " song(s):" << endl;
        Song* temp = head;
        int position = 1;
        while (temp != nullptr)
        {
            cout << position++ << ". ";
            printSong(temp);
            temp = temp->next;
        }
    }

    // this goes from tail to head using the prev links
    void displayBackward() const
    {
        if (isEmpty())
        {
            cout << "Playlist is empty." << endl;
            return;
        }
        cout << "\nPlaylist (Backward) - " << count << " song(s):" << endl;
        Song* temp = tail;
        int position = count;
        while (temp != nullptr)
        {
            cout << position-- << ". ";
            printSong(temp);
            temp = temp->prev;
        }
    }

    // this shows which song the player is sitting on right now
    void playCurrent() const
    {
        if (current == nullptr)
        {
            cout << "Nothing to play, playlist is empty." << endl;
            return;
        }
        cout << "Now playing: ";
        printSong(current);
    }

    // this moves the player one song forward if there is one
    void playNext()
    {
        if (current == nullptr)
        {
            cout << "Playlist is empty." << endl;
            return;
        }
        if (current->next == nullptr)
        {
            cout << "Already at the last song." << endl;
            playCurrent();
            return;
        }
        current = current->next;
        playCurrent();
    }

    // this moves the player one song backward if there is one
    void playPrevious()
    {
        if (current == nullptr)
        {
            cout << "Playlist is empty." << endl;
            return;
        }
        if (current->prev == nullptr)
        {
            cout << "Already at the first song." << endl;
            playCurrent();
            return;
        }
        current = current->prev;
        playCurrent();
    }

    // this reverses the list in place by only swapping the links
    void reverse()
    {
        if (head == nullptr || head == tail)
            return;

        Song* temp = head;
        Song* swapper = nullptr;

        // this swaps the next and prev links of every node one at a time
        while (temp != nullptr)
        {
            swapper = temp->prev;
            temp->prev = temp->next;
            temp->next = swapper;

            // this moves forward which is now held in prev after the swap
            temp = temp->prev;
        }

        // this swaps head and tail so the list now reads the other way round
        swapper = head;
        head = tail;
        tail = swapper;
    }

    // this walks the whole list and frees every song so nothing leaks
    ~Playlist()
    {
        Song* temp = head;
        while (temp != nullptr)
        {
            Song* nextSong = temp->next;
            delete temp;
            temp = nextSong;
        }

        // closing this now by dereferencing all the pointers
        head = tail = current = nullptr;
    }
};

// this exits cleanly if the input stream closes on us
void checkInputStream()
{
    if (cin.eof())
    {
        cout << "\nInput ended. Exiting." << endl;
        exit(0);
    }
}

// this keeps asking until the user types a proper whole number
int readInt(const string& prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        checkInputStream();
        cout << "Invalid input, please enter a whole number." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// this checks that a piece of text is made only of digits
bool allDigits(const string& text)
{
    if (text.empty())
        return false;
    for (char c : text)
        if (c < '0' || c > '9')
            return false;
    return true;
}

// this reads a song name and does not accept an empty one
string readName()
{
    string name;
    while (true)
    {
        cout << "Enter song name: ";
        getline(cin, name);
        checkInputStream();
        if (!name.empty())
            return name;
        cout << "Song name cannot be empty." << endl;
    }
}

// this reads the duration as mm colon ss and splits it into minutes and seconds
void readDuration(int& mins, int& secs)
{
    string input;
    while (true)
    {
        cout << "Enter duration (mm:ss): ";
        getline(cin, input);
        checkInputStream();

        size_t colon = input.find(':');
        if (colon != string::npos)
        {
            string minPart = input.substr(0, colon);
            string secPart = input.substr(colon + 1);

            // this keeps the parts short so stoi never overflows
            if (allDigits(minPart) && allDigits(secPart) && minPart.size() <= 4 && secPart.size() <= 2)
            {
                mins = stoi(minPart);
                secs = stoi(secPart);
                if (secs < 60 && (mins > 0 || secs > 0))
                    return;
            }
        }
        cout << "OOPSSS!!Invalid duration!!! Use mm:ss with seconds from 00 to 59(e.g. 3:45)" << endl;
    }
}

int main()
{
    Playlist playlist;
    int choice;

    do
    {
        cout << "\n========== PLAYLIST MANAGER ==========" << endl;
        cout << "1. Add Song" << endl;
        cout << "2. Delete Song" << endl;
        cout << "3. Display Playlist Forward" << endl;
        cout << "4. Display Playlist Backward" << endl;
        cout << "5. Search Song" << endl;
        cout << "6. Play Current Song" << endl;
        cout << "7. Play Next Song" << endl;
        cout << "8. Play Previous Song" << endl;
        cout << "9. Reverse Playlist" << endl;
        cout << "0. Exit" << endl;
        choice = readInt("Enter your choice: ");

        switch (choice)
        {
        case 1:
        {
            int id = readInt("Enter song ID: ");
            string name = readName();
            int mins = 0, secs = 0;
            readDuration(mins, secs);
            if (playlist.addSong(id, name, mins, secs))
                cout << "Song added successfully." << endl;
            else
                cout << "A song with ID " << id << " already exists." << endl;
            break;
        }
        case 2:
        {
            int id = readInt("Enter song ID to delete: ");
            if (playlist.deleteSong(id))
                cout << "Song deleted successfully." << endl;
            else
                cout << "Song with ID " << id << " not found." << endl;
            break;
        }
        case 3:
            playlist.displayForward();
            break;
        case 4:
            playlist.displayBackward();
            break;
        case 5:
            playlist.showSearchResult(readInt("Enter song ID to search: "));
            break;
        case 6:
            playlist.playCurrent();
            break;
        case 7:
            playlist.playNext();
            break;
        case 8:
            playlist.playPrevious();
            break;
        case 9:
            playlist.reverse();
            cout << "Playlist reversed." << endl;
            playlist.displayForward();
            break;
        case 0:
            cout << "Exiting playlist manager." << endl;
            break;
        default:
            cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 0);

    //the destructor frees every remaining song when main ends
    return 0;
}