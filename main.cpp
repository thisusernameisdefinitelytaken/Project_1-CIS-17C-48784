/* 
 * File:   main.cpp
 * Author: Celeste Hernandez Mora
 *
 * Created on November 11, 2023, 6:01 PM
 */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <stack>
#include <list>
#include <utility>
using namespace std;

// the raw anatomy of a card
struct Card{
  int value;
  int symbol;
  pair<int, int> card;
};

// a deck; stack of cards to pull from
struct Deck{
  stack<pair<int, int>> fdeck;
} deck;

// player data for player & dealer
struct PData{
  list<pair<int, int>> pcVal;
  list<pair<string, string>> pcName;
  int pcSum;
};

// prototype functions
void create(Card [], int);
void shuffle(Card [], int);
void modDeck(Card [], int, bool);
PData assign(PData);
PData name(PData);
int sum(PData);
PData updateCard(PData, int);
void display(int);
void displayCards(PData, bool);
void displayP(PData, int);
int pullNew(PData, PData, bool);
int stand(PData, PData, bool);
int outcome(PData, PData, bool, bool);
bool splitCheck(PData);
PData splitPlay(PData, PData);
float betMod(float, int);

// ========================================

// main program
int main() {
  // set up random & declare variables
  srand(time(0));
  int size = 52;
  char response = '1';
  bool newRound = 0, cSplit = 0;
  Card cards[size];
  float sum = 0.0;

  // while user wants to play more rounds
  while (response == '1' && cSplit != 1) {
    // reset data per round
    int end = 0;
    float bet = 0;
    PData pdata, ddata;
    bool split = 0;

    // get bet
    while (bet < 5.00 || bet > 1000.00) {
    display(1);
    cin >> bet;
    }

    // new game begins
    display(2);

    // create the deck
    create(cards, size);
    shuffle(cards, size);
    modDeck(cards, size, newRound);

    // get 2 cards for each person
    pdata = updateCard(pdata, 2);
    ddata = updateCard(ddata, 2);

    // display player cards & 1 dealer card
    displayP(ddata, 1);
    cout << endl;
    displayP(pdata, 4);

    // check for blackjack
    cSplit = outcome(pdata, ddata, 0, 1);

    // split eligibility check, commence the next processes
    if (cSplit == 0) {
        split = splitCheck(pdata);
        end = pullNew(pdata, ddata, split);
    }

    // choose to continue to new round or not
    display(4);
    cin >> response;

    // output the earnings
    bet = betMod(bet, end);
    sum += bet;
    
    // if first round is a push
    if (end == 4 && newRound == 0) {
        sum = bet;
    }
    
    // output current earnings
    cout << setprecision(2) << fixed;
    cout << "Your earnings: $" << sum << endl;

    // adjust variables for the next loop
    newRound = 1;
    cSplit = 0;
    bet = 0.0;
  }

  cout << endl << "=-=-=- Game ended. -=-=-=";

  // exit
  return 0;
}

// ========================================

// assign unique values & symbols
void create(Card cards[], int size) {
  // declare variables
  int count = 1;
  int cdec = 0;

  // assign values
  for (int j = 0; j < size; j++) {
    cards[j].value = 2 + cdec;
    cards[j].symbol = count;
    cdec += 1;

    cards[j].card.first = cards[j].value;
    cards[j].card.second = cards[j].symbol;
    // if the symbol has all values now, go onto next symbol
    if (cdec == 13) {
      count++;
      cdec = 0;
    }

  }
}

// shuffle the cards into random positions
void shuffle(Card cards [], int size) {
  // swap each card to a random position
  for (int i = 0; i < size; i++)
    swap(cards[i], cards[i + rand() % (size - i)]);
}

// modify deck
void modDeck(Card cards [], int size, bool modFlag) {
    // first round's assignment
    if (modFlag == 0)
        // push the already-randomized card pairs
        for (int i = 0; i < size; i++)
            deck.fdeck.push({cards[i].card.first, cards[i].card.second});
    else { // reset & recreate for succeeding rounds
        while (!deck.fdeck.empty())
            deck.fdeck.pop();
        for (int i = 0; i < size; i++)
            deck.fdeck.push({cards[i].card.first, cards[i].card.second});
    }
}

// assign card
PData assign(PData pHand) {
    // new card in
    pHand.pcVal.push_back(deck.fdeck.top());

    // remove the now-taken card from the deck
    deck.fdeck.pop();

    // return the updated list of one's cards
    return pHand;
}

// get card names, adjust values of face cards
PData name(PData pHand) {
    // declare temporary variables
    list<pair<string, string>> cardName;
    string symbol = "";

    // symbol of card according to raw values
    switch (pHand.pcVal.back().second) {
        case 1: symbol = "Hearts";
        break;
        case 2: symbol = "Clubs";
        break;
        case 3: symbol = "Diamonds";
        break;
        case 4: symbol = "Spades";
        break;
    }

    // card values and names according to raw values, 
    // pair them with corresponding symbols
    switch (pHand.pcVal.back().first) {
        case 2: cardName.push_back(make_pair("2", symbol));
        break;
        case 3: cardName.push_back(make_pair("3", symbol));
        break;
        case 4: cardName.push_back(make_pair("4", symbol));
        break;
        case 5: cardName.push_back(make_pair("5", symbol));
        break;
        case 6: cardName.push_back(make_pair("6", symbol));
        break;
        case 7: cardName.push_back(make_pair("7", symbol));
        break;
        case 8: cardName.push_back(make_pair("8", symbol));
        break;
        case 9: cardName.push_back(make_pair("9", symbol));
        break;
        case 10: cardName.push_back(make_pair("10", symbol));
        break;
        case 11: { // King 
        cardName.push_back(make_pair("K", symbol));
        pHand.pcVal.back().first = 10;
        }
        break;
        case 12: { // Queen
        cardName.push_back(make_pair("Q", symbol));
        pHand.pcVal.back().first = 10;
        }
        break;
        case 13: { // Joker
        cardName.push_back(make_pair("J", symbol));
        pHand.pcVal.back().first = 10;
        }
        break;
        case 14: { // Ace
        cardName.push_back(make_pair("A", symbol));
        // Ace = 11 if value is 10 or less (10+11=21). Ace = 1 if higher.
        if (pHand.pcSum <= 10 || pHand.pcVal.size() == 1) {
            pHand.pcVal.back().first = 11;
        }
        else if (pHand.pcSum > 10) {
            pHand.pcVal.back().first = 1;
            }
        }
        break;
    }
    // assign temp name list to actual
    pHand.pcName.push_back(cardName.back());

    // return player data
    return pHand;
}

// get sum of card values
int sum(PData pdata) {
    // declare variable for sum
    int psum = 0, temp = 0;

    // add values
    for (list<pair<int, int>>::iterator it = pdata.pcVal.begin(); it != pdata.pcVal.end(); ++it) {
        temp = temp + (it->first);
    }

    // the ace check
    if (temp > 21) {
        for (list<pair<int, int>>::iterator it = pdata.pcVal.begin(); it != pdata.pcVal.end(); ++it) {
            if (it->first == 11) {
                it->first = 1;
            }
        }
    }

    // recalculate again for if the aces evolved
    for (list<pair<int, int>>::iterator it = pdata.pcVal.begin(); it != pdata.pcVal.end(); ++it) {
        psum = psum + (it->first);
    }

    // return the sum
    return psum;
}

// update a hand with a new card
PData updateCard(PData pdata, int amount) {
    // get desired amount of cards
    for (int j = 0; j < amount; j++) {
        pdata = assign(pdata);
        pdata = name(pdata);
        pdata.pcSum = sum(pdata);
    }

    // return updated hand
    return pdata;
}

// pull a new card or stand
int pullNew(PData pdata, PData ddata, bool split) {
    // declare variables
    char response = 0;
    int endCheck = 0;
    PData pdata2 = pdata;

    // if there will be a split
    if (split == 1) {
        // split apart the 2 cards into separate hands
        pdata.pcName.pop_back();
        pdata.pcVal.pop_back();
        pdata.pcSum = sum(pdata);
        pdata2.pcName.pop_front();
        pdata2.pcVal.pop_front();

        // adjust ace = 1 values to 11
        if (pdata2.pcVal.front().first == 1) {
            pdata2.pcVal.front().first = 11;
        }

        pdata2.pcSum = sum(pdata2);

        // output first hand
        cout << endl << "You are playing your first hand." << endl;
        displayP(pdata, 3);
        displayP(ddata, 1);
        cout << endl;
        displayP(pdata, 4);
    }

    // display menu
    if (pdata.pcSum != 21) {
        display(3);
        cin >> response;
     }

    // pull card until stand or bust
    while (response == '1' && pdata.pcSum < 21) {
        // add 1 card
        pdata = updateCard(pdata, 1);
        
        // outputs
        cout << endl;
        displayP(pdata, 3);
        displayP(ddata, 1);
        cout << endl;
        displayP(pdata, 4);

        // ask to pull card, stand, or terminate if not yet bust or 21
        if (pdata.pcSum < 21) {
            display(3);
            cin >> response;
        }
    }

    // if split is impending, commence it now
    if (split == 1) {
        endCheck = outcome(pdata, ddata, 0, 0);
        pdata2 = splitPlay(pdata2, ddata);

    }
      
    // stand
    else if (response == '2') {
        endCheck = stand(pdata, ddata, split);
      }
      
    // final outcome
    else {
        endCheck = outcome(pdata, ddata, 0, 1);
    }

    // return outcome
    return endCheck;
}

// make a stand
int stand(PData pdata, PData ddata, bool hand1) {
    // declare variable
    int endFlag = 0;

    cout << endl << "-=- You have chosen to stand. -=-" << endl << endl;

    // dealer pulls a card until their value surpasses 16
    while (ddata.pcSum <= 16) {
        ddata = updateCard(ddata, 1);
    }

    // display based on regular hand
    if (hand1 == 0) {
        displayP(ddata, 2);
        cout << endl;
        displayP(pdata, 4);
        endFlag = outcome(pdata, ddata, 1, 0);
    }

    // display based on split hand
    else if (hand1 == 1) {
        displayP(ddata, 1);
        cout << endl;
        displayP(pdata, 4);
        endFlag = outcome(pdata, ddata, 0, 0);
    }

    // return outcome
    return endFlag;
}  

// primary outcome display
int outcome(PData pdata, PData ddata, bool midCheck, bool showCard) {
    // check for Blackjack or bust if the round is ongoing
    if (midCheck == 0) {
        // blackjack
        if (pdata.pcSum == 21) {
            if (showCard == 1) {
                cout << endl << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
                displayP(ddata, 2);
                cout << endl;
                displayP(pdata, 4);
                cout << endl;
            }

            cout << "=-=-=- 21! You win! -=-=-=" << endl;

            // display total values for dealer and player
            if (showCard == 1) {
                cout << setw(7) << "[D: " << ddata.pcSum << " - P: " << pdata.pcSum << "]"; 
            }
            cout << endl;

            // return Blackjack outcome
            return 1;
        }

        // bust
        else if (pdata.pcSum > 21) {
            if (showCard == 1) {
                cout << endl << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
                displayP(ddata, 2);
                cout << endl;
                displayP(pdata, 4);
                cout << endl;
            }

            cout << "=-=-=- Bust! Dealer's victory -=-=-=" << endl;

            if (showCard == 1) {
                cout << setw(7) << "[D: " << ddata.pcSum << " - P: " << pdata.pcSum << "]";
            }
            cout << endl;

            // return a loss
            return 2;
        }     
    }

    // determine outcomes after round concluded
    else {
        // Blackjack
        if (pdata.pcSum == 21) {
            cout << "=-=-=- 21! You win! -=-=-=" << endl;
            cout << setw(7) << "[D: " << ddata.pcSum << " - P: " << pdata.pcSum << "]"; 
            cout << endl;

            // return Blackjack
            return 1;
        }

        // push
        else if (ddata.pcSum == pdata.pcSum) {
            cout << "=-=-=- Push! -=-=-=" << endl;
            cout << setw(6) << "[D: " << ddata.pcSum << " - P: " << pdata.pcSum << "]";
            cout << endl;

            // return push outcome
            return 4;
        }

        // dealer busts
        else if (ddata.pcSum > 21) {
            cout << "=-= Dealer busts! Your victory =-=" << endl;
            cout << setw(12) << "[D: " << ddata.pcSum << " - P: " << pdata.pcSum << "]"; 
            cout << endl;

            // return a standard win
            return 3;
        }

        // player busts
        else if (pdata.pcSum > 21) {
            cout << "=-=-=- Bust! Dealer's victory -=-=-=" << endl;
            cout << setw(7) << "[D: " << ddata.pcSum << " - P: " << pdata.pcSum << "]"; 
            cout << endl;

            // return a loss
            return 2;
        }

        // dealer is closer to 21
        else if (ddata.pcSum > pdata.pcSum) {
            cout << "=-=-=- Dealer wins! -=-=-=" << endl;
            cout << setw(9) << "[D: " << ddata.pcSum << " - P: " << pdata.pcSum << "]";
            cout << endl;

            // return a loss
            return 2;
        }

        // player is closer to 21
        else if (ddata.pcSum < pdata.pcSum) {
            cout << "=-=-=- You win! -=-=-=" << endl;
            cout << setw(7) << "[D: " << ddata.pcSum << " - P: " << pdata.pcSum << "]"; 
            cout << endl;

            // return a win
            return 3;
        }
    }

    // if none of the outcomes, continue the game
    return 0;
}

// check if first 2 cards can be split
bool splitCheck(PData pdata) {
    // declare variable
    bool splitFlag = 0;

    // check for matching letters
    // note for reference: can change to pcVal for matching numbers like 10s
    for (list<pair<string, string>>::iterator ite = pdata.pcName.begin(); ite != pdata.pcName.end(); ++ite) {
        for (list<pair<string, string>>::iterator it = pdata.pcName.begin(); it != pdata.pcName.end(); ++it) {
            if ((ite->first == it->first)&& (ite->second != it->second)) {
                splitFlag = 1; // can be split
            }
        }
    }
    
    // if it can be split, ask
    if (splitFlag == 1) {
        char response = 0;
        cout << endl;
        cout << "Would you like to split this pair?" << endl;
        cout << "[1] Yes " << endl;
        cout << "[Any] No " << endl;
        cin >> response;

        // yes to split
        if (response == '1') {
            cout << "You have chosen to split. " << endl;
            return 1;
        }
    }

    // no to split
    return 0;
}

// if split: play the second hand
PData splitPlay(PData p2data, PData ddata) {
    cout << endl << "You are playing your second hand." << endl;

    displayP(p2data, 3);
    displayP(ddata, 1);
    cout << endl;
    displayP(p2data, 4);

    pullNew(p2data, ddata, 0);

    // return the second hand
    return p2data;
}

// display output & menus
void display(int disp) {
    switch (disp) {
        case 1: {
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
            cout << " ----- B L A C K J A C K ----- " << endl;
            cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"<< endl << endl;
            cout << "Initiating new game ..." << endl;
            cout << "Place your bet ($5.00 - $1000.00): $";
        }
        break;
        case 2: {
            cout << "-=- Round Begin -=-" << endl << endl;
        }
        break;
        case 3: {
            cout << "What will you do?" << endl;
            cout << "[1] Pick a new card" << endl;
            cout << "[2] Stand" << endl; 
            cout << "[Any] Terminate" << endl;
        }
        break;
        case 4: {
            cout << endl;
            cout << "=-=-=- Round Ended -=-=-=" << endl;
            cout << "Would you like to play another round?" << endl;
            cout << "[1] Yes" << endl;
            cout << "[Any] No" << endl;
        }
        break;
    }
}

// display player or dealer cards
void displayCards(PData pdata, bool pflag) {
    // all cards
    if (pflag == 0) {
        for (list<pair<string, string>>::iterator it = pdata.pcName.begin(); it != pdata.pcName.end(); ++it)
        cout << "[" << it->first << " of " << it->second << "] ";
    }

    // just one card
    if (pflag == 1) {
        cout << "[" << pdata.pcName.back().first << " of " 
             << pdata.pcName.back().second << "]" << endl;
    }
}

// more output, now including totals
void displayP(PData pdata, int disp) {
    switch (disp) {
        case 1: { // one hidden card for the dealer
            cout << "Dealer's cards:" << endl;
            cout << "[ --- ] ";
            displayCards(pdata, 1);
        }
        break;
        case 2: {
            cout << "Dealer's cards:   -=Total: " <<  pdata.pcSum << "=-" << endl;
            displayCards(pdata, 0);
            cout << endl;
        }
        break;
        case 3: { // new card
            cout << "Your newest card: ";
            displayCards(pdata, 1);
            cout << endl << endl;
        }
        break;
        case 4: {
            cout << "Your cards:   -=Total: " <<  pdata.pcSum << "=-" << endl;
            displayCards(pdata, 0);
             cout << endl << endl;
        }
        break;
    }
}

// bet calculator
float betMod(float bet, int result) {
    switch (result) {
        case 1: bet *= 1.5; // Blackjack
        break;
        case 2: bet *= -1; // Loss
        break;
        case 3: bet += bet; // Win
        break;
        default: return 0; // Push
        break;
    }

    // return modified bet
    return bet;
}