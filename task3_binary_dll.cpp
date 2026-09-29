//Task#3
//Writing a program to simulate binary arithmetic operations using Doubly Linked List (DLL)
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
using namespace std;
//this is one bit of the binary number with links on both sides
struct BitNode
{
    int bit;
    BitNode* prev;
    BitNode* next;

    //this sets up a bit that is not linked to anything yet
    BitNode(int b) : bit(b), prev(nullptr), next(nullptr) {}
};

class BinaryNumber
{
private:
    BitNode* head; // this points to the most significant bit
    BitNode* tail; // this points to the least significant bit
    int length;

    //this frees every node and resets the number to empty
    void clear()
    {
        BitNode* temp = head;
        while (temp != nullptr)
        {
            BitNode* nextNode = temp->next;
            delete temp;
            temp = nextNode;
        }

        // closing this now by dereferencing both ends
        head = tail = nullptr;
        length = 0;
    }

    //his adds a bit on the left side which is the most significant end
    void pushFront(int b)
    {
        BitNode* node = new BitNode(b);
        if (head == nullptr)
        {
            head = tail = node;
        }
        else
        {
            node->next = head;
            head->prev = node;
            head = node;
        }
        length++;
    }

    //this adds a bit on the right side which is the least significant end
    void pushBack(int b)
    {
        BitNode* node = new BitNode(b);
        if (tail == nullptr)
        {
            head = tail = node;
        }
        else
        {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        length++;
    }

    //this removes the leftmost bit and frees it
    void popFront()
    {
        if (head == nullptr)
            return;

        BitNode* oldHead = head;
        head = head->next;
        if (head != nullptr)
            head->prev = nullptr;
        else
            tail = nullptr;

        //closing this now by freeing the old head and dereferencing it
        delete oldHead;
        oldHead = nullptr;
        length--;
    }

    // this pads zeros on the left until the bits fill complete 8 bit blocks
    void padToByte()
    {
        if (length == 0)
            pushFront(0);
        while (length % 8 != 0)
            pushFront(0);
    }

    // this drops extra leading zeros and then pads back to whole blocks
    void trimLeadingZeros()
    {
        while (length > 1 && head->bit == 0)
            popFront();
        padToByte();
    }

public:
    // this starts the number off empty
    BinaryNumber() : head(nullptr), tail(nullptr), length(0) {}

    // this makes a deep copy so both numbers own separate nodes
    BinaryNumber(const BinaryNumber& other) : head(nullptr), tail(nullptr), length(0)
    {
        for (BitNode* temp = other.head; temp != nullptr; temp = temp->next)
            pushBack(temp->bit);
    }

    // this assigns using copy and swap so the old nodes are freed safely
    BinaryNumber& operator=(BinaryNumber other)
    {
        swap(head, other.head);
        swap(tail, other.tail);
        swap(length, other.length);
        return *this;
    }

    // this frees all the nodes when the number goes out of scope
    ~BinaryNumber()
    {
        clear();
    }

    bool isEmpty() const { return head == nullptr; }
    int getLength() const { return length; }

    // this reads a string of bits and stores them in grouped 8 bit blocks
    bool store(const string& input)
    {
        string bits;

        // this ignores spaces so the user can type the bits in groups
        for (char c : input)
        {
            if (c == ' ')
                continue;
            if (c != '0' && c != '1')
                return false;
            bits += c;
        }
        if (bits.empty())
            return false;

        clear();
        for (char c : bits)
            pushBack(c - '0');
        padToByte();
        return true;
    }

    // this prints the bits with a space after every 8 bit block
    void display() const
    {
        if (isEmpty())
        {
            cout << "(empty)";
            return;
        }
        int position = 0;
        for (BitNode* temp = head; temp != nullptr; temp = temp->next)
        {
            cout << temp->bit;
            position++;
            if (position % 8 == 0 && temp->next != nullptr)
                cout << " ";
        }
        cout << "  [" << length / 8 << " block(s)]";
    }

    // this walks the list and flips every bit into a new number
    BinaryNumber onesComplement() const
    {
        BinaryNumber result;
        for (BitNode* temp = head; temp != nullptr; temp = temp->next)
            result.pushBack(temp->bit == 0 ? 1 : 0);
        return result;
    }

    // this flips all bits and then adds one while keeping the same width
    BinaryNumber twosComplement() const
    {
        BinaryNumber ones = onesComplement();
        BinaryNumber one;
        one.pushBack(1);
        one.padToByte();

        // this drops the final carry since twos complement stays the same width
        return add(ones, one, false);
    }

    // this adds two numbers bit by bit from the right and carries over
    static BinaryNumber add(const BinaryNumber& a, const BinaryNumber& b, bool keepCarry = true)
    {
        BinaryNumber result;
        BitNode* x = a.tail;
        BitNode* y = b.tail;
        int carry = 0;

        // this keeps going while either number still has bits left
        while (x != nullptr || y != nullptr)
        {
            int sum = carry;
            if (x != nullptr)
            {
                sum += x->bit;
                x = x->prev;
            }
            if (y != nullptr)
            {
                sum += y->bit;
                y = y->prev;
            }
            result.pushFront(sum % 2);
            carry = sum / 2;
        }

        // this puts the last carry in front if we are allowed to grow
        if (carry == 1 && keepCarry)
            result.pushFront(1);

        result.padToByte();
        return result;
    }

    // this multiplies by adding the shifted first number for every 1 in the second number
    static BinaryNumber multiply(const BinaryNumber& a, const BinaryNumber& b)
    {
        BinaryNumber result;
        result.padToByte();

        BinaryNumber shifted(a);

        // this reads the second number from the least significant bit upward
        for (BitNode* y = b.tail; y != nullptr; y = y->prev)
        {
            if (y->bit == 1)
                result = add(result, shifted, true);

            // this shifts left by one by adding a zero at the right end
            shifted.pushBack(0);
        }

        result.trimLeadingZeros();
        return result;
    }

    // this converts the bits to decimal and fails if the value will not fit in 64 bits
    bool toDecimal(unsigned long long& value) const
    {
        value = 0;
        BitNode* temp = head;

        // this skips leading zeros since they add nothing to the value
        while (temp != nullptr && temp->bit == 0)
            temp = temp->next;

        int significantBits = 0;
        for (BitNode* check = temp; check != nullptr; check = check->next)
            significantBits++;
        if (significantBits > 64)
            return false;

        // this doubles the value and adds the next bit each step
        while (temp != nullptr)
        {
            value = value * 2 + temp->bit;
            temp = temp->next;
        }
        return true;
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

// this keeps asking until the user types only 0s and 1s
void readBinary(BinaryNumber& number, const string& label)
{
    string input;
    while (true)
    {
        cout << "Enter binary number " << label << ": ";
        getline(cin, input);
        checkInputStream();
        if (number.store(input))
            return;
        cout << "Invalid binary number. Use only 0 and 1." << endl;
    }
}

// this prints a label then the number and its decimal value
void showWithDecimal(const string& label, const BinaryNumber& number)
{
    cout << label;
    number.display();
    cout << endl;

    unsigned long long value;
    if (number.isEmpty())
        return;
    if (number.toDecimal(value))
        cout << "   Decimal: " << value << endl;
    else
        cout << "   Decimal: too large to show (more than 64 significant bits)" << endl;
}

// this makes sure a number has been entered before an operation uses it
bool ready(const BinaryNumber& number, const string& label)
{
    if (number.isEmpty())
    {
        cout << "Binary number " << label << " has not been entered yet." << endl;
        return false;
    }
    return true;
}

int main()
{
    BinaryNumber a, b, result;
    int choice;

    do
    {
        cout << "\n========== BINARY ARITHMETIC USING DLL ==========" << endl;
        cout << "1. Store Binary Number A" << endl;
        cout << "2. Store Binary Number B" << endl;
        cout << "3. Display A and B" << endl;
        cout << "4. 1's Complement (A and B)" << endl;
        cout << "5. 2's Complement (A and B)" << endl;
        cout << "6. Binary Addition (A + B)" << endl;
        cout << "7. Binary Multiplication (A x B)" << endl;
        cout << "8. Convert to Decimal (A, B and last result)" << endl;
        cout << "0. Exit" << endl;
        choice = readInt("Enter your choice: ");

        switch (choice)
        {
        case 1:
            readBinary(a, "A");
            cout << "Stored A: ";
            a.display();
            cout << endl;
            break;
        case 2:
            readBinary(b, "B");
            cout << "Stored B: ";
            b.display();
            cout << endl;
            break;
        case 3:
            cout << "A: ";
            a.display();
            cout << "\nB: ";
            b.display();
            cout << endl;
            break;
        case 4:
            if (ready(a, "A"))
            {
                result = a.onesComplement();
                cout << "A            : "; a.display(); cout << endl;
                cout << "1's comp of A: "; result.display(); cout << endl;
            }
            if (ready(b, "B"))
            {
                result = b.onesComplement();
                cout << "B            : "; b.display(); cout << endl;
                cout << "1's comp of B: "; result.display(); cout << endl;
            }
            break;
        case 5:
            if (ready(a, "A"))
            {
                result = a.twosComplement();
                cout << "A            : "; a.display(); cout << endl;
                cout << "2's comp of A: "; result.display(); cout << endl;
            }
            if (ready(b, "B"))
            {
                result = b.twosComplement();
                cout << "B            : "; b.display(); cout << endl;
                cout << "2's comp of B: "; result.display(); cout << endl;
            }
            break;
        case 6:
            if (ready(a, "A") && ready(b, "B"))
            {
                result = BinaryNumber::add(a, b);
                showWithDecimal("  A     = ", a);
                showWithDecimal("  B     = ", b);
                showWithDecimal("  A + B = ", result);
            }
            break;
        case 7:
            if (ready(a, "A") && ready(b, "B"))
            {
                result = BinaryNumber::multiply(a, b);
                showWithDecimal("  A     = ", a);
                showWithDecimal("  B     = ", b);
                showWithDecimal("  A x B = ", result);
            }
            break;
        case 8:
            if (ready(a, "A"))
                showWithDecimal("A           : ", a);
            if (ready(b, "B"))
                showWithDecimal("B           : ", b);
            if (!result.isEmpty())
                showWithDecimal("Last result : ", result);
            break;
        case 0:
            cout << "Exiting." << endl;
            break;
        default:
            cout << "Invalid choice, try again." << endl;
        }
    } while (choice != 0);

    // the destructors free every bit node when main ends
    return 0;
}