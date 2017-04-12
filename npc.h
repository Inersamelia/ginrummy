#ifndef HEADERFILE_NPC
#define HEADERFILE_NPC

/* 
	Author: Joseph Tindall
	Team 10 (ECGR 2104-002)

This is the class for the ai designed to play against the player. It will always expect three inputs:
	(1) string: Name of the bot for the game, can be blank but will default to Larry.
	(2) vector<string>: This vector must be of size 10 (due to player's holding 10 cards in their hand).
		- Card convention is the first unique suite letter followed by a value of 1 through 13. 1 is ace, 11 is jack, 12 is queen, 13 is king.
	(3) bool: This boolean variable determines if the bot will give debug output.

The bot is currently programmed for one output:
	(1) int: When NonPlayerControlled->Play(const string discardCard) is used, it will return:
		- 0: The bot has not won in any way. The player's turn should proceed.
		- 1: The bot has declared a standard victory. After discarding a card, they have no deadwood left.
		- 2: The bot has declared a big gin victory. After drawing a card, they hae no deadwood left.

Example Creation:
	NonPlayerControlled bot("Kanna", vector<string>(10)={"d2","c2","s2","d7","d8","d9","h9","h10","h11","h12"}, true);

Example Usage:
	if (botObject.Play(getTopDiscardPile()) != 0) cout << "The bot has declared victory!\n";
*/

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
using namespace std;

class NonPlayerControlled
{
public:
	NonPlayerControlled();
	NonPlayerControlled(const string n, const vector<string>& h, const bool d);

	const unsigned int Play(const string discardCard);	// Core function, call this to have the bot complete one turn.
	void OutputHand();	// For the end of game, show the bot's cards, seperated by what type of meld it is.

private:
	bool debug;	// Show all important conclusions/actions the npc has decided to take.
	int currentTurn = 0;
	string name;	// Will default to Larry if an empty string is given.
	string debugPrefix = "\t!! ";	// No non-dev should see this, edit accordingly to however it helps you identify.

	vector<string> hand;	// Must be given a proper deck of 10 cards on creation of object.
	map<string, vector<string>> sets; // Cards that properly fit into a set but not also a row.
	vector<vector<string>> rows; // cards that fit into a row of at least 3.
	vector<string> deadwood;	// Cards in neither rows or sets.

	void DetermineMelds(); // This will set the above essential variables to best possible rows and sets combinations.
	void DetermineDeadwood();
	const unsigned int CountSetCards();
	const unsigned int CountRowCards();
};

#endif
