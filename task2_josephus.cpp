//task#2
//Writing a program to simulate the Josephus Problem using a circular linked list
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
using namespace std;
//this is one person standing in the circle
struct Person
{
    int id;
    Person* next;

    //this sets up a person who is not linked to anyone yet
    Person(int personId) : id(personId), next(nullptr) {}
};

class JosephusCircle
{
private:
    //this points to the last person so the next of tail is always the first person
    Person* tail;
    int size;

public:
    //this starts with an empty circle
    JosephusCircle() : tail(nullptr), size(0) {}

    //this blocks copying so the same nodes are never freed twice
    JosephusCircle(const JosephusCircle&) = delete;
    JosephusCircle& operator=(const JosephusCircle&) = delete;

    int getSize() const { return size; }

    //his builds the circle of n people with ids from 1 to n
    void createCircle(int n)
    {
        for (int i = 1; i <= n; i++)
        {
            Person* newPerson = new Person(i);

            if (tail == nullptr)
            {
                //this makes the first person point to itself to close the circle
                tail = newPerson;
                newPerson->next = newPerson;
            }
            else
            {
                // this slips the new person in after the tail and keeps the circle closed
                newPerson->next = tail->next;
                tail->next = newPerson;
                tail = newPerson;
            }
        }
        size = n;
    }

    // this goes round the circle once starting from the first person
    void displayCircle() const
    {
        if (tail == nullptr)
        {
            cout << "Circle is empty." << endl;
            return;
        }
        Person* first = tail->next;
        Person* temp = first;
        cout << "Circle: ";
        do
        {
            cout << temp->id << " -> ";
            temp = temp->next;
        } while (temp != first);
        cout << "(back to " << first->id << ")" << endl;
    }

    // this removes every kth person and fills the order array with their ids
    int eliminate(int k, int* order)
    {
        Person* prev = tail;
        Person* curr = tail->next;
        int index = 0;
        int round = 1;

        // this keeps going until only one person is left pointing to itself
        while (curr->next != curr)
        {
            // this skips full laps since going round the whole circle changes nothing
            int moves = (k - 1) % size;
            for (int step = 0; step < moves; step++)
            {
                prev = curr;
                curr = curr->next;
            }

            // this unlinks the person by joining their neighbours together
            prev->next = curr->next;
            order[index++] = curr->id;
            cout << "Round " << round++ << ": Person " << curr->id << " eliminated" << endl;

            // this keeps tail valid if the eliminated person was the tail
            if (curr == tail)
                tail = prev;

            // closing this now by freeing the node and dereferencing the pointer
            delete curr;
            curr = nullptr;
            size--;

            // this resumes counting from the person right after the one removed
            curr = prev->next;
        }

        //this moves tail onto the survivor so the destructor can still free it
        tail = curr;
        return curr->id;
    }

    //this breaks the circle first and then frees every node so nothing leaks
    ~JosephusCircle()
    {
        if (tail == nullptr)
            return;

        Person* curr = tail->next;
        tail->next = nullptr;

        while (curr != nullptr)
        {
            Person* nextPerson = curr->next;
            delete curr;
            curr = nextPerson;
        }

        // closing this now by dereferencing the tail pointer
        tail = nullptr;
    }
};

//this keeps asking until the user types a whole number that is at least the minimum
int readPositiveInt(const string& prompt, int minimum)
{
    int value;
    while (true)
    {
        cout << prompt;
        if (cin >> value && value >= minimum)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof())
        {
            cout << "\nInput ended. Exiting." << endl;
            exit(0);
        }
        cout << "Invalid input, please enter a whole number >= " << minimum << "." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int main()
{
    cout << "========== JOSEPHUS PROBLEM SIMULATION ==========" << endl;

    int n = readPositiveInt("Enter number of people (N): ", 1);
    int k = readPositiveInt("Enter step count (k): ", 1);

    JosephusCircle circle;
    circle.createCircle(n);

    cout << endl;
    circle.displayCircle();
    cout << "\nStarting from person 1, eliminating every " << k << " person(s)...\n" << endl;

    // this holds the eliminated ids in order and there are always n minus 1 of them
    int* order = new int[n > 1 ? n - 1 : 1];

    int survivor = circle.eliminate(k, order);

    cout << "\nElimination Order: ";
    if (n == 1)
        cout << "(none, only one person)";
    for (int i = 0; i < n - 1; i++)
    {
        cout << order[i];
        if (i < n - 2)
            cout << ", ";
    }
    cout << endl;

    cout << "Survivor: Person " << survivor << endl;

    // closing this now by freeing the array and dereferencing the pointer
    delete[] order;
    order = nullptr;

    // the destructor frees the survivor node when main ends
    return 0;
}