// Team 10 AI
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

	bool Play(const string discardCard);

private:
	bool debug;	// Show all important conclusions/actions the npc has decided to take.
	string name;	// Will default to Larry if an empty string is given.
	string debugPrefix = "\t !:: ";	// No non-dev should see this, edit accordingly to however it helps you identify.

	vector<string> hand;	// Must be given a proper deck of 10 cards on creation of object.
	map<string, vector<string>> sets;
	vector<vector<string>> rows;

	void DetermineMelds(); // This will set the above essential variables to best possible rows and sets combinations.
	unsigned int CountSetCards();
	unsigned int CountRowCards();
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
	sort(hand.begin(), hand.end()); // Ensure the vector is in efficent looping order.

	// The function will first find temporary sets and rows, determine which of which to use for it's turn, then adjust class values with selection.
	// Find all possible sets that could be matched into.
	map<string, unsigned int> suitelessValueCount;
	map<string, vector<string>> possibleSets;

	for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
		if (!(suitelessValueCount.insert(pair<string, int>(string(*i).erase(0, 1), 1)).second))
			suitelessValueCount[string(*i).erase(0, 1)] += 1;

	for (map<string, unsigned int>::iterator i = suitelessValueCount.begin(); i != suitelessValueCount.end(); i++)
		if (i->second > 2)	// Since only sets of 3 and 4 are possible due to (Spade, Diamond, Heart, Clover), no fail safe is needed.
		{
			if (debug) cout << debugPrefix << i->first << " can be a set of " << i->second << " cards.\n";

			if (possibleSets.find(i->first) == possibleSets.end())
				possibleSets.insert(pair<string, vector<string> >(i->first, vector<string>()));

			if (debug) cout << "\t" << debugPrefix << "size comparision of found vs recorded: " << i->second << " : " << possibleSets.find(i->first)->second.size() << "\n";

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
			if (possibleSets.find(i->first) != possibleSets.end())
			{	// If the set is no longer possible, ensure it is not within the possible set list.
				possibleSets.erase(i->first);
				if (debug) cout << debugPrefix << i->first << " was deleted from map<string, vector<string>> possibleSets.\n";
			}
		}

	// Now that all possible sets are accounted for, attempt to find all possible rows.
	map<string, vector<string>> seperatedSuites;
	vector<vector<string>> possibleRows;

	for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
	{	// Converts the string (char in a sense) to an int value for better sorting. Just easier for me to conceptualize.
		if (seperatedSuites.find(to_string((*i)[0])) == seperatedSuites.end())
			seperatedSuites.insert(pair<string, vector<string>>(to_string((*i)[0]), vector<string>()));

		seperatedSuites.find(to_string((*i)[0]))->second.push_back(string(*i).erase(0, 1));
	}

	for (map<string, vector<string>>::iterator i = seperatedSuites.begin(); i != seperatedSuites.end(); i++)
	{
		string rowCardBegun;
		unsigned int rowStreakCount = 0;
		vector<string> rowPushed;

		for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
			if ((next(j) != i->second.end()) && (*next(j) == to_string(stoi(*j) + 1)))
			{
				if (rowStreakCount == 0)
					rowCardBegun = *j;
				rowStreakCount++;
			} else {
				if (rowStreakCount >= 2)
				{
					for (unsigned int k = 0; k <= rowStreakCount; k++)
					{	// The following char() conversion is important, as the string is converted to int for ease of foor loop purposes. ASCII simply handles better.
						rowPushed.push_back(char(stoi(i->first)) + *find(i->second.begin(), i->second.end(), to_string(stoi(rowCardBegun) + k)));
						if (debug) cout << debugPrefix << to_string(stoi(rowCardBegun) + k) << " added to vector<string> rowPushed.\n";
					}

					possibleRows.push_back(rowPushed);
				}

				rowStreakCount = 0;
			}
	}

	// Determine which sets are invalid due to rows.
	vector<string> invalidSets;
	for (map<string, vector<string>>::iterator i = possibleSets.begin(); i != possibleSets.end(); i++)
		for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
			for (vector<vector<string>>::iterator k = possibleRows.begin(); k != possibleRows.end(); k++)
				for (vector<string>::iterator l = k->begin(); l != k->end(); l++)
					if (*j == *l)
						invalidSets.push_back(i->first);

	for (vector<string>::iterator i = invalidSets.begin(); i != invalidSets.end(); i++)
		possibleSets.erase(possibleSets.find(*i));

	// Finaly, set essential class variables to temporary variables decided upon by this function.
	for (map<string, vector<string>>::iterator i = possibleSets.begin(); i != possibleSets.end(); i++)
		sets.insert(possibleSets.begin(), possibleSets.end());
	rows = possibleRows;

	// Debug output of final conclusions.
	if (debug)
	{
		cout << debugPrefix << "essential sets:\n";
		for (map<string, vector<string>>::iterator i = sets.begin(); i != sets.end(); i++)
		{
			cout << "\t" << debugPrefix << i->first << ": ";
			for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
				cout << *j << ", ";
			cout << "\n";
		}

		cout << debugPrefix << "essential rows:\n";
		for (vector<vector<string>>::iterator i = rows.begin(); i != rows.end(); i++)
		{
			cout << "\t" << debugPrefix;
			for (vector<string>::iterator j = i->begin(); j != i->end(); j++)
				cout << *j << ", ";
			cout << "\n";
		}
	}
}

unsigned int NonPlayerControlled::CountSetCards()
{
	unsigned int count = 0;

	for (map<string, vector<string>>::iterator i = sets.begin(); i != sets.end(); i++)
		for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
			count++;

	if (debug) cout << debugPrefix << "count for sets is " << count << "\n";

	return count;
}

unsigned int NonPlayerControlled::CountRowCards()
{
	unsigned int count = 0;

	for (vector<vector<string>>::iterator i = rows.begin(); i != rows.end(); i++)
		for (vector<string>::iterator j = i->begin(); j != i->end(); j++)
			count++;

	if (debug) cout << debugPrefix << "count for rows is " << count << "\n";

	return count;
}

bool NonPlayerControlled::Play(const string discardCard)
{
	// First determine what is currently in hand with no predictions.
	DetermineMelds();

	unsigned int currentSetsCount = CountSetCards();
	unsigned int currentRowsCount = CountRowCards();

	// Determine if the discard card will be used to make an immediate increase in meld.
	hand.push_back(discardCard);
	if (debug) cout << debugPrefix << discardCard << " was added to vector<string> hand.\n";

	DetermineMelds();

	if (currentSetsCount == CountSetCards() && currentRowsCount == CountRowCards()) {
		if (debug) cout << debugPrefix << "There was no change in melds found, discard card deemed to risky.\n";
			
		hand.erase(remove(hand.begin(), hand.end(), discardCard), hand.end());
		// TODO: Add function to pull card from unknown deck.
		DetermineMelds();
	} else {
		if (debug) cout << debugPrefix << "There was a change in melds found, discard card deemed worthy.\n";
		// TODO: Add function to pull the discard card.
	}

	// Now, discard one card that is not the pulled card.

	// Check if bot has melded all cards.
	if (CountSetCards() + CountRowCards() == 10)
		return true;
	return false;
}

int main()
{
	NonPlayerControlled bot("Kanna", vector<string>(10)={"s6","c6","h11","h9","d8","d9","d7","d1","h10","h12"}, true);

	bot.Play("h13");

	cout << "\n"; system("pause");
	return 0;
}
