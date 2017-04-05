// Team 10 AI Module - Joseph Tindall
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

class NonPlayerControlled
{
public:
	NonPlayerControlled();
	NonPlayerControlled(const string n, const vector<string>& h, const bool d);
	
	void RemoveOneCard(const string ctbr);
	void AddOneCard(const string ctba);

	bool Play(vector<string>& discardPile);
	string Name() { return name; }

private:
	bool debug;	// Show all important conclusions/actions the npc has decided to take.
	string name;	// Will default to Larry if an empty string is given.
	string debugPrefix = "\t debug :: ";	// No non-dev should see this, edit however it helps you identify.
	vector<string> hand;	// Must be given a proper deck of 10 cards on creation of object.
	map<string, vector<string>> sets;

	void DetermineMelds();
};

NonPlayerControlled::NonPlayerControlled()
{
	cerr << "Non player character's cannot be created without three given parameters of string, vector<string> [size 10], and bool values.\n";
	system("pause");
	exit(1);
}
NonPlayerControlled::NonPlayerControlled(const string n, const vector<string>& h, const bool d)
{
	debug = d;

	try
	{
		if (n.empty())
			throw n;
		name = n;
	}
	catch (string e)
	{
		cerr << "Name parameter was empty, program will default to name of \"Larry\".\n";
		name = "Larry";
	}

	try
	{
		if (int(h.size()) != 10)
			throw int(h.size());
		hand = h;
	}
	catch (int e)
	{
		cerr << "Bot created with improper parameters. Given hand size was " << e << ", while only 10 will be accepted.\n";
		system("pause");
		exit(1);
	}
}

void NonPlayerControlled::DetermineMelds()
{
	// The function will first find temporary sets and rows, determine which of which to use for it's turn, then adjust class values with selection.
	// Find all possible sets the ai could match into.
	map<string, int> suitelessValueCount;
	map<string, vector<string>> possibleSets;

	for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
		if (suitelessValueCount.insert(pair<string, int>(string(*i).erase(0, 1), 1)).second == false)
			suitelessValueCount[string(*i).erase(0, 1)] += 1;

	for (map<string, int>::iterator i = suitelessValueCount.begin(); i != suitelessValueCount.end(); i++)
		if (i->second > 2)	// Since only sets of 3 and 4 are possible due to (Spade, Diamond, Heart, Clover), no fail safe is needed.
		{
			if (debug) cout << debugPrefix << i->first << " can be a set of " << i->second << " cards.\n";

			if (possibleSets.find(i->first) == possibleSets.end())
				possibleSets.insert(pair<string, vector<string> >(i->first, vector<string>()));

			if (debug) cout << debugPrefix << "Size comparision of found vs recorded: " << i->second << " : " << possibleSets.find(i->first)->second.size() << "\n";

			if (i->second != possibleSets.find(i->first)->second.size())
			{
				possibleSets.find(i->first)->second.clear();

				for (vector<string>::iterator j = hand.begin(); j != hand.end(); j++)
					if (string(*j).erase(0, 1) == i->first)
					{
						possibleSets.find(i->first)->second.push_back(*j);
						if (debug) cout << debugPrefix << *j << " added to set [" << i->first << "] in map<string, vector<string>> possibleSets.\n";
					}
			}
		} else {
			if (debug) cout << debugPrefix << i->first << " cannot be a set.\n";

			if (possibleSets.find(i->first) != possibleSets.end())
			{	// If the set is no longer possible, ensure it is not within the possible set list.
				possibleSets.erase(i->first);
				if (debug) cout << debugPrefix << i->first << " was deleted from map<string, vector<string>> possibleSets.\n";
			}
		}

	// Now that all possible sets are accounted for, attempt to find all possible rows for the ai.
	map<string, vector<string>> seperatedSuites;
}

void NonPlayerControlled::RemoveOneCard(const string ctbr)
{
	hand.erase(remove(hand.begin(), hand.end(), ctbr), hand.end());
	if (debug) cout << debugPrefix << ctbr << " was removed from vector<string> hand.\n";
}

void NonPlayerControlled::AddOneCard(const string ctba)
{
	hand.push_back(ctba);
	if (debug) cout << debugPrefix << ctba << " was added to vector<string> hand.\n";
}

bool NonPlayerControlled::Play(vector<string>& discardPile)
{
	DetermineMelds();

	return false;
}

int main()
{
	vector<string> botsHand;
	// Simply 10 cards that are "Randomly" obtained from deck.
	botsHand.push_back("sa");
	botsHand.push_back("ca");
	botsHand.push_back("da");
	botsHand.push_back("h4");
	botsHand.push_back("d5");
	botsHand.push_back("d6");
	botsHand.push_back("d7");
	botsHand.push_back("d8");
	botsHand.push_back("d9");
	botsHand.push_back("dk");

	NonPlayerControlled bot("Kanna", botsHand, true);

	if (bot.Play(botsHand))
		cout << bot.Name() << " claims victory.\n";
	else
		cout << "Player's turn.\n";

	system("pause");
	return 0;
}