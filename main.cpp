#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

// Type for representing an augmented grammar
typedef map<char, vector<string> > AugmentedGrammar;

// Type for representing the global set of gotos, maps productions to LR(0) item ids
typedef map<string, int> GotoMap;


// Production type
struct AugmentedProduction
{
    char lhs;   // left hand side of production
    string rhs; // right hand side of production
    AugmentedProduction* next;

    AugmentedProduction() : next(NULL) {}

    AugmentedProduction(char _lhs, string _rhs)
    : lhs(_lhs), rhs(_rhs), next(NULL) {}

    ~AugmentedProduction() { if (next) delete next; }
};


// Class for representing LR(0) items.
class LR0Item
{
private:
    // list of productions
    vector<AugmentedProduction*> productions;

public:
    // list of out-edges
    map<char, int> gotos;

    // constructor
    LR0Item() {}

    // destructor
    ~LR0Item() {}

    // add production
    void push(AugmentedProduction *p) { productions.push_back(p); }

    // return the number of productions
    int Size() { return int(productions.size()); }

    bool Contains (string prodStr) {
        for (auto it = productions.begin(); it != productions.end(); it++) {
            string thisStr = string(&(*it)->lhs, 1) + "->" + (*it)->rhs;
            //cout << " Comparing: " << thisStr << " , " << prodStr << endl;
            if (strcmp(prodStr.c_str(), thisStr.c_str()) == 0) {
                return true;
            }
        }
        return false;
    }

    // get a production by reference
    AugmentedProduction* operator[](const int index) {
        return productions[index];
    }
};


/**
 * void add_closure
 * --------------------------------------------------------------------------
 * If the 'next' is the current input symbol and next is a nonterminal, then
 * the set of LR(0) items reachable from this item on next includes the set
 * of LR(0) items reachable from this item on FIRST(next). Therefore we must
 * add all grammar productions with a lhs of next to this item */
void
add_closure(AugmentedProduction *p, LR0Item& item, AugmentedGrammar& grammar)
{
    // get lookahead
    char next = p->rhs[p->rhs.find('@')+1];
    string plhs = string(&next,1);
    // only continue if lookahead is a non-terminal
    if (!isupper(next)) {
        return;
    }

    // iterate over each grammar production beginning with p->rhs[next]
    for (int i = 0; i<grammar[next].size(); i++) {
        string prhs = "@" + grammar[next][i];
        // if the grammar production for the next input symbol does not yet
        // exist for this item, add it to the item's set of productions
        string prodStr = plhs + "->" + prhs;
        if (!item.Contains(prodStr)) {
            item.push(new AugmentedProduction(next, prhs));
        }
    }
}


void
gen_gotos(vector<LR0Item>& lr0items, AugmentedGrammar& grammar, int& itemid,
         GotoMap& _goto)
{
    printf("I%d:\n", itemid);

    map<char, int> localGoto;
    AugmentedProduction *prod;

    // ensure that the current item contains te full closure of it's productions
    for (int i = 0; i<lr0items[itemid].Size(); i++)
    {
        prod = lr0items[itemid][i];
        add_closure(prod, lr0items[itemid], grammar);
    }

    int nextpos;

    // iterate over each production in this LR(0) item
    for (int i = 0; i<lr0items[itemid].Size(); i++)
    {
        // get the current production
        prod = lr0items[itemid][i];
        printf("\t%c->%s\t\t", prod->lhs, prod->rhs.c_str());
        // get lookahead
        nextpos = prod->rhs.find('@')+1;
        if (nextpos == string::npos || nextpos == prod->rhs.length()) {
            printf("\n");
            continue;
        }
        char next = prod->rhs[nextpos];
        // if there is no goto defined for the current input symbol from this
        // item, assign one.
        if (lr0items[itemid].gotos.find(next) == lr0items[itemid].gotos.end()) {
            // if there is a global goto defined for the entire production, use
            // that one instead of creating a new one
            string currentProd = string(&(prod->lhs),1) + "->" + prod->rhs;
            if (_goto.find(currentProd) == _goto.end()) {
                lr0items.push_back(LR0Item()); // create new state (item)
                // new right-hand-side is identical with '@' moved one space to
                // the right
                string newRhs = prod->rhs;
                int atpos = newRhs.find('@');
                swap(newRhs[atpos], newRhs[atpos+1]);
                // add item and update gotos
                lr0items.back().push(new AugmentedProduction(prod->lhs, newRhs));
                lr0items[itemid].gotos[next] = lr0items.size()-1;
                _goto[currentProd] = lr0items.size()-1;
            } else {
                // use existing global item
                lr0items[itemid].gotos[next] = _goto[currentProd];
            }
            printf("goto(%c)=I%d", next, _goto[currentProd]);
        } else {
            // there is a goto defined, add the current production to it
            // move @ one space to right for new rhs
            string newRhs = prod->rhs;
            string newLhs = string(&prod->lhs, 1);
            int atpos = newRhs.find('@');
            swap(newRhs[atpos], newRhs[atpos+1]);
            // add production to next item if it doesn't already contain it
            int nextItem = lr0items[itemid].gotos[next];
            if (!(lr0items[nextItem].Contains(newLhs + "->" + newRhs))) {
                lr0items[nextItem].push(new AugmentedProduction(
                    prod->lhs,
                    newRhs
                ));
            }
        }
        printf("\n");
    }
}

// main
int main() {

    int itemid = -1; // counter for the number of LR(0) items
    AugmentedGrammar grammar;
    vector<LR0Item> lr0items = {LR0Item()}; // push start state
    GotoMap _goto;

    grammar['\''] = {"E"};
    grammar['E']  = {"E+T","T"};
    grammar['T']  = {"T*F", "F"};
    grammar['F']  = {"(E)", "i"};

    lr0items[0].push( new AugmentedProduction('\'', "@E"  ) );
    lr0items[0].push( new AugmentedProduction('E',  "@E+T") );
    lr0items[0].push( new AugmentedProduction('E',  "@T"  ) );
    lr0items[0].push( new AugmentedProduction('T',  "@T*F") );
    lr0items[0].push( new AugmentedProduction('T',  "@F"  ) );
    lr0items[0].push( new AugmentedProduction('F',  "@(E)") );
    lr0items[0].push( new AugmentedProduction('F',  "@i"  ) );

    while (++itemid < int(lr0items.size())) {
        gen_gotos(lr0items, grammar, itemid, _goto);
    }

    return 0;
}
