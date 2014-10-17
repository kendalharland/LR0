#include <ctype.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <map>

using namespace std;

// Production type
struct AugmentedProduction {
    char lhs;
    string rhs;
    AugmentedProduction* next;

    AugmentedProduction() : next(NULL) {}

    AugmentedProduction(char _lhs, string _rhs)
    : lhs(_lhs), rhs(_rhs), next(NULL) {}

    ~AugmentedProduction() { if (next) delete next; }
};

// Type for representing an augmented grammar
typedef map<char, vector<string> > AugmentedGrammar;

// Type for representing our LR(0) items
typedef vector<AugmentedProduction> LR0Item;

// Type for representing GoTo rules
typedef map<char, LR0Item> GotoMap;

// return the closure of a production
void
closure( char lhs, AugmentedGrammar grammar, int& itemc, GotoMap& _goto )
{
    queue<char> next; // next non-terminal to evaluate
    int prodc = 0;    // production count for current non-terminal
    // current production. head is the beginning of the list of all productions
    AugmentedProduction *prod = new AugmentedProduction();
    next.push(lhs);

    do {
        // add the new production to the current closure
        prod->lhs = next.back();
        prod->rhs = "@" + grammar[prod->lhs][prodc];
        printf("%c->%s\t\t", prod->lhs, prod->rhs.c_str());
        // check if the next symbol is a nonterminal
        if (isupper(prod->rhs[1])) {
            // mark this as the next production, but only if we aren't already
            // evaluating this production.
            if (prod->rhs[1] != prod->lhs) {
                next.push(prod->rhs[1]);
            }
        }
        // if goto(prod->rhs[1]) is not yet a state, create a new state and
        // add the current production. else add the current production
        if (_goto.find(prod->rhs[1]) == _goto.end()) {
            printf("goto(%c) = I%d", prod->rhs[1], itemc++);
        }
        // add this production to the corresponding Goto
        _goto[prod->rhs[1]].push_back(AugmentedProduction(
            prod->lhs,
            prod->rhs.substr(1,1) + "@" + prod->rhs.substr(2)
        ));
        // modify the grammar to include this new production's rhs
        grammar[prod->lhs][prodc] = _goto[prod->rhs[1]].back().rhs;
        // printf("\t\tnew grammar: %c->%s", prod->lhs, grammar[prod->lhs][prodc].c_str());
        // only leave the current non-terminal if we've moved through all of its
        // grammar productions
        prodc++;
        if (prodc >= grammar[prod->lhs].size()) {
            next.pop();
            prodc = 0;
        }
        // move to the next production
        prod->next = new AugmentedProduction();
        prod = prod->next;
        printf("\n");
    }
    while (!next.empty());
}


// main
int main() {

    int itemc = 1; // counter for the number of LR(0) items
    AugmentedGrammar grammar;
    GotoMap _goto;

    grammar['\''] = {"E"};
    grammar['E']  = {"E+T","T"};
    grammar['T']  = {"T*F", "F"};
    grammar['F']  = {"(E)", "i"};

    closure('\'', grammar, itemc, _goto);

    // E production1, production2, ...
    // T production1, ...
    // F production1, ...
    //

    return 0;
}
