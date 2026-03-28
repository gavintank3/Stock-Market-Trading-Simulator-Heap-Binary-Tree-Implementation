/*
 *Author: Luis E. Ortiz
 *Purpose: Code for simple stock market trading simulator using priority queues based on heaps and 
 a hash map to keep track of trade transaction records
# Update: 10/30/2023 - Refactored
*/
//Gavin Tank

#include <iostream>
#include <string>
#include <iomanip>
#include <list>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;



// Utility functions
void loadFile(string fname, fstream& file)
{
    file.open(fname.c_str());
    if (file.fail())
    {
        cout << "Cannot open file " << fname << endl;
    }
}


// Key structure for stock market trading
struct Key {
    double price;
    int timeStamp;
    Key() : price(0.0), timeStamp(0) { }
    Key(double p, int t) {
        price = p;
        timeStamp = t;
    }
};

// less-than key-comparison operator overloading
// INPUT: two keys, x and y (passed by reference)
// OUPUT: true iff the price of x < price of y, or prices are equal but time stamp of x < time stamp of y
// PRECONDITION: properly initialized keys
bool
operator < (const Key &x, const Key &y) {
    return ((x.price < y.price) || ((x.price == y.price) && (x.timeStamp < y.timeStamp)));
}

// utility/aux method to print the time stamp of a key
// INPUT: output stream out (passed by ref) and a pointer to time-structure object
// PRECONDITION: properly initialized inputs
// POSTCONDITION: string-formatted time stamp sent to out
void
printTimeStamp(ostream &out, const int t) {
    out << t;
}

// overloading output stream operator for keys
// INPUT: output stream out and a key (both passed by ref)
// OUTPUT: the output stream (passed by ref)
// PRECONDITION: properly initialized input
// POSTCONDITION: string-formatted key sent to out
ostream &
operator <<(ostream &out, const Key &k) {
    out << "(" << k.price << ",";
    printTimeStamp(out, k.timeStamp);
    out << ")";
    return out;
}

// Value structure for stock-market trading
struct Value {
    int numShares;
    int traderID;
    Value() : numShares(0), traderID(0) { };
    Value(int num, int id) {
        numShares = num;
        traderID = id;
    };
};

// overloading output stream operator for values
// INPUT: output stream out and a value (both passed by ref)
// OUTPUT: the output stream (passed by ref)
// PRECONDITION: properly initialized input
// POSTCONDITION: string-formatted value sent to out
ostream &
operator <<(ostream &out, const Value &v) {
    out << "(" << v.numShares << "," << v.traderID << ")";
    return out;
}

// Element structure used to represent elements of a node in a binary tree
struct Elem {
    Key* key;
    Value* value;
    Elem() : key(NULL), value(NULL) { }
    Elem(Key* k, Value* v) {
        key = k;
        value = v;
    }
    Elem(Elem* e) {
        key = e->key;
        value = e->value;
    }
    void clear() {
        if (key) delete [] key;
        if (value) delete [] value;
        key = NULL;
        value = NULL;
    }
};

// INPUT: two elements, x and y
// PRECONDITION: inputs are non-NULL
// POSTCONDITON: the data members of the inputs are swapped
void
swapElem(Elem* x, Elem* y) {
    Key* key = y->key;
    Value* value = y->value;
    y->key = x->key;
    y->value = x->value;
    x->key = key;
    x->value = value;
}

// Overloading less-than comparison operator for elements
// INPUT: two elements, x and y (passed by ref)
// OUTPUT: true iff the key of x < key of y
// PRECONDITION: input properly initialized (each element's key is non-NULL)
bool
operator < (const Elem& x, const Elem& y) {
    return (*(x.key) < *(y.key));
}

// Overloading greater-than comparison operator for elements
// INPUT: two elements, x and y (passed by ref)
// OUTPUT: true iff the key of x > key of y
// PRECONDITION: input properly initialized 
bool
operator > (const Elem& x, const Elem& y) {
    return (y < x);
}

// overloading output stream operator for elements
// INPUT: output stream out and an element (both passed by ref)
// OUTPUT: the output stream (passed by ref)
// PRECONDITION: properly initialized input
// POSTCONDITION: string-formatted element sent to out
ostream &
operator <<(ostream& out, const Elem& e) {
    out << fixed << setprecision(2) << (*(e.key)) << ":" << (*(e.value)) ;
    return out;
}

// Binary tree (BT) ADT
class BT {
    public:
        // simple data structure used to create nodes for (linked-list) implementation of general BTs
        struct Node {
            Elem* elem;
            Node* left;
            Node* right;
            Node* parent;
            Node() : elem(NULL), left(NULL), right(NULL), parent(NULL) { }
            Node(Elem* e, Node* l, Node* r, Node* p) {
                elem = e;
                left = l;
                right = r;
                parent = p;
            }
        };

        typedef BT::Node Node;

    private:
        void deleteAll();
  
    protected:
        void printAux(const Node* w) const;

    public:
        Node* root;
        int n;

        BT() : root(NULL), n(0) { };
        ~BT();

        void makeChild(Node* c, Node* p, bool isLeftChild);
        Node* firstLeftAncestor(Node* w);
        Node* lastLeftDescendant(Node* w);
        Node* firstRightAncestor(Node* w);
        Node* lastRightDescendant(Node* w);

  Node* youngestDescendantType(Node* w, bool check_left) const;
  Node* youngestAncestorType(Node* w, bool check_left) const;

        Node* minChild(Node* w);
        Elem* removeNode(Node* z);
        int size() { return n; }
        bool empty() { return (n == 0); }
        void print() const;
        void printTree(Node* s, int space) const;

  void swapElem(Node* w, Node* z);
  
};

// INPUT: a pair of nodes w and z in the BT
// PRECONDITION: w and z non-NULL
// POSTCONDITION: w and z swap their elements
void 
BT::swapElem(Node* w, Node* z) {
  Elem* tmp = w->elem;
  w->elem = z->elem;
  z->elem = tmp;
}

// utility/aux function to print out a parenthetic string representation of a BT
// INPUT: a node w in the BT whose subtree is to be printed out; or NULL
// PRECONDITION: w has been properly initialized (non-NULL element)
void
BT::printAux(const Node* w) const {
    if (w) {
        cout << "[";
        cout << (*(w->elem));
        cout << "]";
        cout << "(";
        printAux(w->left);
        cout << "),(";
        printAux(w->right);
        cout << ")";
    }
}

// print out a parenthetic string representation of the whole BT
void
BT::print() const {
    printAux(root);
    cout << endl;
}

// prints out a string representation of the whole BST using a reverse inorder traversal
void
BT::printTree(Node* s, int space) const {
    int addSpace = 8;
    // base case
    if (!s)
    {
        return;
    }
    // add more whitespace
    space = space + addSpace;
    // print right
    this->printTree(s->right, space);

    cout << endl;
    for (int i = addSpace; i < space; i++)
        cout << " ";
    cout << *s->elem << endl;

    // print left
    this->printTree(s->left, space);
}

// INPUT: w, a node in the BT
// OUTPUT: e, the element of w
// PRECONDITION: the tree is not empty; and the left or right subtree, or both, of w are empty
// POSTCONDITION: the size of the BT is reduced by 1 after w is properly removed from the BT
Elem*
BT::removeNode(Node* w) {
  Elem* e = w->elem;
  Node* z = w->parent;
  // identify child if it exists
  Node* x = (w->left) ? w->left : w->right;
  makeChild(z, x, (!z) || (z->left == w));
  if (!z) root = x;
  n--;
  return e;
}

// POSTCONDITION: The BT is empty
void
BT::deleteAll()
{
    Node* w = root;
    while (w) {
        if (!(w->left || w->right)) {
            Node* x = w;
            w = w->parent;
            if (w) {
                if (w->left == x) w->left = NULL;
                else w->right = NULL;
            }
            if (x->elem) {
                x->elem->clear();
                delete [] x->elem;
            }
            delete [] x;
            n--;
            continue;
        }
        w = (w->left) ? w->left : w->right;
    }
}

// POSTCONDITION: The BT is empty
BT::~BT() {
    deleteAll();
}

// INPUT: w, a node in the BT
// OUTPUT: the child with the lowest key (if any)
// PRECONDITION: if w has both children, then their elements are non-NULL
BT::Node*
BT::minChild(Node* w) {
    if (!w) return NULL;
    Node* wL = w->left;
    Node* wR = w->right;
    if (!wR) return wL;
    if (!wL) return wR;
    Elem* eL = wL->elem;
    Elem* eR = wR->elem;
    return ((*eL < *eR) ? wL : wR);
}

/*
  # INPUT: a node w in the BT or NULL;
  # ancestor type flag: True iff searching for youngest left ancestor
  # OUTPUT: the node corresponding to the youngest right/left ancestor
  # of w: this is the node x in the tree
  # that is the first ancestor of w whose immediate descendant is a
  # left/right child, depending on input type flag (said differently, the node x is the first found in the path from w to
  # the root such that for a node z that is also an ancestor of w,
  # thus also in that path to the root from w, and where z could be w
  # itself, we have x.right = z or x.left = z, depending on input type flag); or NULL if w is NULL or the root
  # node of the tree Depending on the value of the input flag
  # (True/False), the output is the first ancestor of w that has a left/right relationship to its child that is also an ancestor of w
*/
BT::Node*
BT::youngestAncestorType(BT::Node* w, bool check_left) const {
  if (!w) return NULL;
  BT::Node* z = w;
  BT::Node* x = z->parent;
  while ((x && ((check_left ? x->right : x->left) == z))) {
    z = x;
    x = x->parent;
  }
  return x;
}

/*
  # INPUT: a node w in the BT or NULL;
  # descendant type flag: True iff searching for youngest left descendant
  # OUTPUT: a node corresponding to the youngest left/right descendant
  # of w, inclusive: this is the node x in the tree
  # that is the last left/right descendant of w, depending on input
  # type flag); or NULL if w is NULL Depending on the value of the input type
  # flag (True/False), the output is the (leftmost/rightmost) node in the subtree rooted at w
*/
BT::Node*
BT::youngestDescendantType(BT::Node* w, bool check_left) const {
  if (!w) return NULL;
  while (check_left ? w->left : w->right) 
    w = check_left ? w->left : w->right;
  return w;
}

// INPUT: a node w in a BT
// OUTPUT: z = the first ancestor node (i.e., node in a path from w to the root)
// such that w is in the left subtree of the BT rooted at z
// PRECONDITION: w is not NULL
BT::Node*
BT::firstLeftAncestor(Node* w) {
    return youngestAncestorType(w,true);
}

// INPUT: a node w in a BT
// OUTPUT: "leftmost" node of the BT rooted at w
// PRECONDITION: non-null w
BT::Node*
BT::lastLeftDescendant(Node* w) {
  // NAME: Gavin Tank
    return youngestDescendantType(w, true);

}

// INPUT: a node w in a BT
// OUTPUT: z = the first ancestor node (i.e., node in a path from w to the root)
// such that w is the right subtree of the BT rooted at z
// PRECONDITION:
// POSTCONDITION:
BT::Node*
BT::firstRightAncestor(Node* w) {
    // NAME: Gavin Tank  
	return youngestAncestorType(w, false);
  
}

// INPUT: a node w in a BT
// OUTPUT: "rightmost" node of the BT rooted at w
// PRECONDITION: w is not NULL
BT::Node*
BT::lastRightDescendant(Node* w) {
    return youngestDescendantType(w,false);
}

// INPUT: p, a node in the BT or NULL; c, a node in the BT or NULL; and a Boolean flag, isLeft, signaling whether c should become the left child of p
// PRECONDITION: 
// POSTCONDITON: the corresponding child of p is set to c, depending on the input; if c is not NULL, then p becomes its parent
void
BT::makeChild(Node* p, Node* c, bool isLeft) {
  if (p) {
    if (isLeft) p->left = c;
    else p->right = c;
  }
  if (c) c->parent = p;
}

// Complete Binary Tree (Complete BT) ADT
class CompleteBT : public BT
{
public:
    typedef BT::Node Node;
  
    Node* lastNode;
    Node* add(Elem* elem);
    Elem* remove();

    CompleteBT() : lastNode(NULL) {  };
  
private:
    Node* getParentOfNewLastNode();
    Node* getNewLastNode();
};

// OUTPUT: the node in the complete BT where any new node inserted would be placed
// PRECONDITION: non-empty tree
BT::Node*
CompleteBT::getParentOfNewLastNode() {
  // NAME: Gavin Tank
    if (lastNode->parent == NULL) return lastNode; // lastNode is the root
    Node* w = lastNode->parent;
    if (w->right == NULL) return w; // lastNode is a left child
	Node* x = firstLeftAncestor(w); // find first left ancestor of w
    if (x == NULL) { // no left ancestor of w
      return lastLeftDescendant(root); // find lowest left child from root
    }
	return lastLeftDescendant(x->right); // find lowest left child from right of x

}

// OUTPUT: the node in the BT that would become the last node of the complete BT should the last node be removed
// PRECONDITION: tree is not empty
BT::Node*
CompleteBT::getNewLastNode() {
  // NAME: Gavin Tank
	if (lastNode->parent == NULL) return NULL; // lastNode is the root
    Node* w = lastNode;
	if (w == w->parent->right) return w->parent->left; // lastNode is a right child
    Node* x = firstRightAncestor(w); // find first right ancestor of w
    if (x == NULL) { // no right ancestor of w
      return lastRightDescendant(root); // find lowest right child from root
	}
	return lastRightDescendant(x->left); // find lowest right child from left of x

}

// INPUT: an element e
// OUTPUT: the new node x in the complete BT containing e
// POSTCONDITION: x is inserted in the proper place in the complete BT and becomes its last node; if the tree was empty, then x becomes the root; the size of the tree increases by 1
BT::Node* 
CompleteBT::add(Elem* e) {
  // NAME: Gavin Tank
  if (n == 0) { // tree is empty
    Node* x = new Node(e, NULL, NULL, NULL); // create new root node for tree
	root = x; // update stats...
    lastNode = x;
    n++;
    return x;
  }
  Node* w = getParentOfNewLastNode(); // find parent of where a new last node would go
  Node* x = new Node(e, NULL, NULL, w); // create new node with parent w and element e
  makeChild(w, x, (w->left == NULL)); // make x a child of w
  lastNode = x; // update stats...
  n++;
  return x;

}

// OUTPUT: the element of the last node of the complete BT (to be removed); NULL if the tree is empty
// POSTCONDITON: the last node of the complete BT is removed from the tree and the last node is properly reset (to NULL if the tree becomes empty); the size of the tree decreases by 1
Elem*
CompleteBT::remove() {
  // NAME: Gavin Tank
	if (n == 0) return NULL; // tree is empty
    Elem* e = lastNode->elem; // get element of last node
    Node* w = getNewLastNode(); // find new last node
	removeNode(lastNode); // remove last node
	lastNode = w; // update lastNode
	return e;
}

// Heap data-structure implementation of a priority queue ADT
class Heap : public CompleteBT
{
public:
    typedef CompleteBT::Node Node;
  
    void insert(Elem* e);
    Elem* min();
    void removeMin();
  
private:
    void upHeapBubbling();
    void downHeapBubbling();
};

// INPUT: an element e to be inserted in the heap
// POSTCONDITION: a proper heap (after the insertion of e); a new node containing e is added to the heap, so that the size of the heap increases by 1
void
Heap::insert(Elem* e) {
  // NAME: Gavin Tank
	add(e); // add new node with element e to complete BT
	upHeapBubbling(); // perform up-heap bubbling to maintain heap-order property

}

// OUTPUT: the minimum (highest priority) element of the heap
Elem*
Heap::min() {
  // NAME: Gavin Tank
	return (root->elem ? root->elem : NULL); //since root is always the min element in a heap

}

// POSTCONDITION: the minimum (highest priority) element (i.e., the element at the root of the tree) is removed from the heap; the element of the last node is copied to the root; the last node is removed and the new last node is set; a down-heap bubbling operation is performed at the root with the new copied element to maintain the heap-order property as needed
void
Heap::removeMin() {
  // NAME: Gavin Tank
    if (n == 1) {
        remove();
        root = NULL; // update stats...
        lastNode = NULL;
        return;
    }
    swapElem(root, lastNode); // swap elements of minimum element (root) and last node
	remove(); // remove minimum element (now at last node)
	downHeapBubbling(); // perform down-heap bubbling to maintain heap-order property

}

// PRECONDITION: the heap is not empty
// POSTCONDITION: the up-heap bubbling operation is performed at the last node after performing an insertion to maintain the heap-order property
void
Heap::upHeapBubbling() {
  // NAME: Gavin Tank
    Node* w = lastNode;
	while (w->parent && *(w->elem) < *(w->parent->elem)) { //since min heap, go up while parent element is greater than the current element
        swapElem(w, w->parent); // swap elements of w and its parent
        w = w->parent; // set w as parent and continue
    }

}

// POSTCONDITION: the down-heap bubbling operation is performed at the root, if the heap is not empty or it has at least two nodes, after performing a removal to maintain the heap-order property
void
Heap::downHeapBubbling() {
  // NAME: Gavin Tank
    Node* w = root;
	if (!w) return; // empty heap 
    Node* x = minChild(w);
	while (x && *(x->elem) < *(w->elem)) { // since min heap, go down while child element is less than current element
        swapElem(w, x); // swam elements of w and its child
        w = x; // set w as child
        x = minChild(w); // get new minChild of w and continue
    }

}

// DO NOT CHANGE ANYTHING BELOW THIS LINE

typedef Heap PriorityQueue;

// Ledger ADT for financial books/records
class Ledger {
    private:
        typedef list<Elem*> TransList;
        // a financial record data-structure 
        struct Record {
            int id;
            double balance;
            int holdings;
            TransList buyTrans;
            TransList sellTrans;
            Record() : id(0), balance(0.0), holdings(0)  { }
            Record(int i, double bal, int h) {
                id = i;
                balance = bal;
                holdings = h;
            }
            void deleteTransList(TransList& L) {
                for (TransList::const_iterator it = L.cbegin(); it != L.cend(); ++it)
                if (*it) {
                    Elem* e = *it;
                    e->clear();
                }
                L.clear();
            }
        };

        typedef Ledger::Record Record;
        void printRecord(const Record *r) const;
        void printTransList(const TransList& L) const;
        
    public:
        Ledger() {};
        ~Ledger() {
            for (HashMap::const_iterator it = book.cbegin(); it != book.cend(); ++it)
                if (it->second) {
                    Record* r = it->second;	
                    r->deleteTransList(r->buyTrans);
                    r->deleteTransList(r->sellTrans);
                }
            book.clear();
        };
        void trans(Elem* e, bool isBuyTrans);
        void buy(Elem* e);
        void sell(Elem* e);
        void print() const;

    private:
        typedef unordered_map<int, Record*> HashMap;
        HashMap book;
};

void
Ledger::printTransList(const TransList& L) const {
    cout << "(";
    TransList::const_iterator it = L.cbegin();
    if (it != L.cend()) {
        cout << *(*it);
        for (++it; it != L.cend(); ++it)
            cout << "," << (*(*it));
    }
    cout << ")";
}

void
Ledger::printRecord(const Record* r) const {
    cout << r->id << ":" << r->balance << ":" << r->holdings << ":";
    printTransList(r->buyTrans);
    cout << ":";
    printTransList(r->sellTrans);
}

void 
Ledger::print() const {
    for (HashMap::const_iterator it = book.cbegin(); it != book.cend(); ++it) {
        printRecord(it->second);
        cout << endl;  // cannot modify *it
    }
}

// INPUT: an element e and a Boolean flag signaling whether e corresponds to a buy transaction
// PRECONDITION: e is non-NULL, as are its key and value
// POSTCONDITION: the transaction is inserted into the ledger/book for the corresponding trader, and the trader record is updated (i.e., the trader's holdings/num of shares and balance/amount of money made or lost in all of the trader's transactions); the record for a new trader is created, and properly initialized, if this is the first transaction for the trader
void
Ledger::trans(Elem* e, bool isBuyTrans) {
    double price = e->key->price;
    int num = e->value->numShares;
    int id = e->value->traderID;
    Record* record = book[id];
    if (!record) {
        book[id] = new Record(id,0.0,0);
        record = book[id];
    }
    record->holdings += num;
    record->balance += num * price;
    if (isBuyTrans) record->buyTrans.push_back(e);
    else record->sellTrans.push_back(e);
}

// INPUT: an element e
// POSTCONDITION: a buy transaction is inserted into the ledger/book for the corresponding trader, and the trader record is updated to increase the trader's holdings by the number of shares bought and reduce the trader's balance by the amount of money the trader paid for the shares; if this is the first transaction for the trader, a new record is created, and properly initialized
void
Ledger::buy(Elem* e) {
    trans(e,true);
}

// INPUT: an element e
// POSTCONDITION: a sell transaction is inserted into the ledger/book for the corresponding trader, and the trader record is updated to reduce its holdings by the number of shares sold and increase the trader's balance by the amount of money the trader obtained for the shares; if this is the first transaction for the trader, a new record is created, and properly initialized
void
Ledger::sell(Elem* e) {
    trans(e,false);
}

// Stock Market ADT
class StockMarket {
    private:
        PriorityQueue buyOrders;
        PriorityQueue sellOrders;
        Ledger books;
        double bank;
        int counter = 0;

        void processTrade();
        void trade();
        void transAux(int num, double price, int id, int t, bool buyTrans);
        void buyAux(int num, double price, int id, int t);
        void sellAux(int num, double price, int id, int t);

    public:
        StockMarket() : bank(0.0) { };
  
        void buy(double price, int num, int id);
        void sell(double price, int num, int id);

        void print();
        void printBuy();
        void printSell();
        void printLedger();
        void printBank();
};

void
StockMarket::printBuy() {
    cout << "*** Buy Limit Orders ***" << endl;
    buyOrders.printTree(buyOrders.root, 0);
}

void
StockMarket::printSell() {
    cout << "*** Sell Limit Orders ***" << endl;
    sellOrders.printTree(sellOrders.root, 0);
}

void
StockMarket::printLedger() {
    cout << "*** Transaction Record ***" << endl;
    books.print();
}

void
StockMarket::printBank() {
    cout << "*** Bank Profit ***" << endl;
    cout << "$ " << bank << endl;
}

void
StockMarket::print() {
    printBuy();
    printSell();
    printLedger();
    printBank();
}

// INPUT: the number of shares and price involved in the trade, the trader's ID, and the time order was placed
// POSTCONDITION: a new element for the order is added to the respective limit-order book for the stock market depending on the trade type
void
StockMarket::transAux(int num, double price, int id, int t, bool buyTrans) {
    Key* k = new Key(price * ((buyTrans) ? -1.0 : 1.0), t);
    Value* v = new Value(num, id);
    Elem* e = new Elem(k, v);
    if (buyTrans) buyOrders.insert(e);
    else sellOrders.insert(e);
}

// INPUT: the number of shares and price for the buy order placed by the trader with the given input id, and the time the buy order was placed
// POSTCONDITION: a new element for the buy order is added to the respective buy limit-order book for the stock market
void
StockMarket::buyAux(int num, double price, int id, int t) {
    transAux(num, price, id, t, true);
}

// INPUT: the number of shares and price for the sell order placed by the trader with the given input id, and the time the sell order was placed
// POSTCONDITION: a new element for the sell order is added to the respective sell limit-order book for the stock market
void
StockMarket::sellAux(int num, double price, int id, int t) {
    transAux(num, price, id, t, false);
}

// INPUT: the price and number of shares for the buy order placed by the trader with the given input id
// POSTCONDITION: a new element for the buy order is added to the respective buy limit-order book for the stock market and a trade is executed if there is a matching sell order already in the limit-order books for the stock market
void
StockMarket::buy(double price, int num, int id) {
    buyAux(num, price, id, counter++);
    trade();
}

// INPUT: the price and number of shares for the sell order placed by the trader with the given input id
// POSTCONDITION: a new element for the sell order is added to the respective sell limit-order book for the stock market and a trade is executed if there is a matching buy order already in the limit-order books for the stock market
void
StockMarket::sell(double price, int num, int id) {
    sellAux(num, price, id, counter++);
    trade();
}

// PRECONDITION: there are matching orders in the limit-order books for the stock market
// POSTCONDITION: any possible trade is executed, and properly documented/recorded; the limit-order books for the stock market are properly updated and maintained; the stock market's bank balance is increased if there is a margin over the markets' spread (i.e., the buy limit-order price is higher than the sell limit-order price)
void
StockMarket::processTrade() {
    Elem* buyMin = buyOrders.min();
    Elem* sellMin = sellOrders.min();
    Elem* buyLimitOrder = new Elem(buyMin);
    Elem* sellLimitOrder = new Elem(sellMin);
  
    double priceBuy = -buyLimitOrder->key->price;
    double priceSell = sellLimitOrder->key->price;
    int timeBuy = buyLimitOrder->key->timeStamp;
    int timeSell = sellLimitOrder->key->timeStamp;
    int numBuy = buyLimitOrder->value->numShares;
    int numSell = sellLimitOrder->value->numShares;
    int idBuy = buyLimitOrder->value->traderID;
    int idSell = sellLimitOrder->value->traderID;

    double priceDiff = priceBuy - priceSell;

    int numTrade;
    int numRemain;

    Elem* buyTrade;
    Elem* sellTrade;
    Key* k;
    Value* v;

    sellOrders.removeMin();
    buyOrders.removeMin();

    if (numBuy > numSell) {
        numTrade = numSell;
        sellTrade = sellLimitOrder;
        k = new Key(-priceBuy, timeBuy);
        v = new Value(numTrade, idBuy);
        buyTrade = new Elem(k, v);
        numRemain = numBuy-numSell;
        // add leftover buys as a new order
        if (numRemain > 0) buyAux(numRemain, priceBuy, idBuy, timeBuy);
    }
    else {
        numTrade = numBuy;
        buyTrade = buyLimitOrder;
        k = new Key(priceSell,timeSell);
        v = new Value(numTrade, idSell);
        sellTrade = new Elem(k, v);
        numRemain = numSell - numBuy;
        // add leftover sells as a new order
        if (numRemain > 0) sellAux(numRemain, priceSell, idSell, timeSell);
    }
    books.buy(buyTrade);
    books.sell(sellTrade);
    bank += priceDiff * numTrade;
}

// POSTCONDITION: all possible trades are processed/executed and recorded/documented, the market's limit-order books are properly updated/maintained, and the market profit from the respective trades (if any) is updated/increased
void
StockMarket::trade() {
    if (buyOrders.empty() || sellOrders.empty()) return;
    bool tradeAvail = true;
    while (tradeAvail && !(buyOrders.empty() || sellOrders.empty())) {
        Elem* buyLimitOrder = buyOrders.min();
        Elem* sellLimitOrder = sellOrders.min();
    
        double buyPrice = -buyLimitOrder->key->price;
        double sellPrice = sellLimitOrder->key->price;

        double marketSpread = sellPrice - buyPrice;

        // process trades if lowest sell <= highest buy
        tradeAvail = (marketSpread <= 0.0);
        if (tradeAvail) processTrade();
    }
}

int main() {
    string inputFilename = "input.txt";
    string line;

    StockMarket M;
    // open input file
    fstream inputFile;
    loadFile(inputFilename, inputFile);
    while (getline(inputFile, line))
    {
        // trim whitespace
        // echo input
        cout << line << endl;
        // parse input using a stringstream
        stringstream lineSS(line);
        string token;
        string command = "";
        // store tokens in a vector
        vector<string> tokens;
        while (getline(lineSS, token, ' '))
        {
            // trim whitespace
            token.erase(token.find_last_not_of(" \n\r\t") + 1);
            if (token.length() > 0)
            {
                tokens.push_back(token);
            }
        }
        if (tokens.size() > 0)
        {
            command = tokens[0]; // first token is the command
        }
        if (tokens.size() == 1)
        {
            if (command == "print")
            {
                M.print();
            }
        }
        if (tokens.size() > 1)
        {
            if (command == "buy") // buy # shares @ specific price, id
            {
                M.buy(stof(tokens[2]), stoi(tokens[1]), stoi(tokens[3]));
            }
            if (command == "sell") // sell # shares @ specific price, id
            {
                M.sell(stof(tokens[2]), stoi(tokens[1]), stoi(tokens[3]));
            }
            if (command == "print")
            {
                if (tokens[1] == "buy")
                {
                    M.printBuy();
                }
                if (tokens[1] == "sell")
                {
                    M.printSell();
                }
                if (tokens[1] == "ledger")
                {
                    M.printLedger();
                }
                if (tokens[1] == "bank")
                {
                    M.printBank();
                }
            }
        }
        
    }
    inputFile.close();
    return EXIT_SUCCESS;
}

