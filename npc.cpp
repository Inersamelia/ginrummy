#include "npc.h"
using namespace std;

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
	sort(hand.begin(), hand.end());
	if (debug) cout << "\n";

	// The function will first find temporary sets and rows, determine which of which to use for it's turn, then adjust class values with selection.
	// Find all possible sets that could be matched into.
	map<string, unsigned int> suitelessValueCount;
	map<string, vector<string>> possibleSets;

	for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
		if (!(suitelessValueCount.insert(pair<string, int>(string(*i).erase(0, 1), 1)).second))
			suitelessValueCount[string(*i).erase(0, 1)] += 1;

	for (map<string, unsigned int>::iterator i = suitelessValueCount.begin(); i != suitelessValueCount.end(); i++)
		// Since only sets of 3 and 4 are possible due to (Spade, Diamond, Heart, Clover), no fail safe is needed.
		if (i->second > 2)
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
		}
		else {
			if (possibleSets.find(i->first) != possibleSets.end())
			{	// If the set is no longer possible, ensure it is not within the possible set list.
				possibleSets.erase(i->first);
				if (debug) cout << debugPrefix << i->first << " was deleted from map<string, vector<string>> possibleSets.\n";
			}
		}

		// Now that all possible sets are accounted for, attempt to find all possible rows.
		unordered_map<string, vector<string>> seperatedSuites;
		vector<vector<string>> possibleRows;

		for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
		{
			if (seperatedSuites.find(to_string((*i)[0])) == seperatedSuites.end())
				seperatedSuites.insert(pair<string, vector<string>>(to_string((*i)[0]), vector<string>()));

			seperatedSuites.find(to_string((*i)[0]))->second.push_back(string(*i).erase(0, 1));
		}

		// Sort the suites in int ascending value instead of character based.
		for (unsigned int k = 0; k < seperatedSuites.size(); k++)
			for (unordered_map<string, vector<string>>::iterator i = seperatedSuites.begin(); i != seperatedSuites.end(); i++)
				for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
					if ((next(j) != i->second.end()) && (stoi(*j) > stoi(*next(j))))
					{
						string temp = *j;
						*j = *next(j);
						*next(j) = temp;
					}

		for (unordered_map<string, vector<string>>::iterator i = seperatedSuites.begin(); i != seperatedSuites.end(); i++)
		{
			string rowCardBegun;
			unsigned int rowStreakCount = 0;
			vector<string> rowPushed;

			if (debug) cout << debugPrefix << "Checking the " << i->first << " suite for rows.\n";

			for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
			{
				if (debug) cout << "\t" << debugPrefix << "Checking " << *j << ", expecting " << to_string(stoi(*j) + 1) << "\n";

				if ((next(j) != i->second.end()) && (*next(j) == to_string(stoi(*j) + 1)))
				{
					if (debug) cout << "\t\t" << debugPrefix << "Found " << *next(j) << ".\n";

					if (rowStreakCount == 0)
					{
						rowCardBegun = *j;
						if (debug) cout << "\t\t" << debugPrefix << "string rowCardBegun set to " << rowCardBegun << ".\n";
					}

					rowStreakCount++;
				}
				else {
					if (rowStreakCount >= 2)
					{
						if (debug) cout << debugPrefix << "unsigned int rowStreakCount was at least 2, creating a new row.\n";
						// The following char() conversion is important, as the string is converted to int for ease of foor loop purposes. ASCII simply handles better.
						for (unsigned int k = 0; k <= rowStreakCount; k++)
						{
							rowPushed.push_back(char(stoi(i->first)) + *find(i->second.begin(), i->second.end(), to_string(stoi(rowCardBegun) + k)));
							if (debug) cout << "\t" << debugPrefix << to_string(stoi(rowCardBegun) + k) << " added to vector<string> rowPushed.\n";
						}

						if (debug) cout << "\n";
						possibleRows.push_back(rowPushed);
					}

					rowStreakCount = 0;
				}
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

			cout << "\n";
		}
}

void NonPlayerControlled::DetermineDeadwood()
{
	deadwood.erase(deadwood.begin(), deadwood.end());
	if (debug) cout << debugPrefix << "deadwood cards are: ";

	for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
	{
		bool inMeld = false;

		for (map<string, vector<string>>::iterator j = sets.begin(); j != sets.end(); j++)
			for (vector<string>::iterator k = j->second.begin(); k != j->second.end(); k++)
				if (*k == *i)
					inMeld = true;

		for (vector<vector<string>>::iterator j = rows.begin(); j != rows.end(); j++)
			for (vector<string>::iterator k = j->begin(); k != j->end(); k++)
				if (*k == *i)
					inMeld = true;

		if (inMeld == false)
		{
			deadwood.push_back(*i);
			if (debug) cout << *i << ", ";
		}
	}

	sort(deadwood.begin(), deadwood.end());
	if (debug) cout << "\n";
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
	DetermineMelds();
	unsigned int currentSetsCount = CountSetCards();
	unsigned int currentRowsCount = CountRowCards();
	string cardToAdd = "";

	// Determine if the discard card will be used to make an immediate increase in meld.
	hand.push_back(discardCard);
	if (debug) cout << debugPrefix << discardCard << " was added to vector<string> hand.\n";

	DetermineMelds();

	hand.erase(remove(hand.begin(), hand.end(), discardCard), hand.end());
	if (debug) cout << debugPrefix << discardCard << " was removed from vector<string> hand.\n";

	if (currentSetsCount == CountSetCards() && currentRowsCount == CountRowCards()) {
		if (debug) cout << debugPrefix << "There was no change in melds found, discard card deemed to risky.\n";

		// TODO: Get card to add from deck.
		cardToAdd = "Unknown Stack Card";
	}
	else {
		if (debug) cout << debugPrefix << "There was a change in melds found, discard card deemed worthy.\n";

		// TODO: Get card from discard deck.
		cardToAdd = "Discard Stack Card";
	}

	DetermineMelds();
	DetermineDeadwood();

	string cardToDiscard = "";
	string highestSuiteFound = "";
	int highestSuitelessValue = 0;

	for (vector<string>::iterator i = deadwood.begin(); i != deadwood.end(); i++)
		if (highestSuitelessValue < stoi(string(*i).erase(0, 1)) && (*i != cardToAdd))
		{
			highestSuitelessValue = stoi(string(*i).erase(0, 1));
			highestSuiteFound = string(*i).erase(1, (*i).size() - 1);

			if (debug) cout << debugPrefix << "string highestSuiteFound set to " << string(*i).erase(1, (*i).size() - 2) << ".\n";
			if (debug) cout << debugPrefix << "unsigned int highestSuitelessValue set to " << stoi(string(*i).erase(0, 1)) << ".\n";
		}

	// Determine what card to discard. If deadwood is found, use the highest card unless a 2 is found, then use an ace if possible. 
	// Otherwise use the largest card within sets. If no set is found, use largest row.
	if (highestSuiteFound != "")
		if (highestSuitelessValue > 1)
			cardToDiscard = highestSuiteFound + to_string(highestSuitelessValue);
		else
			cardToDiscard = deadwood.front();
	else
		if (CountSetCards() > 0)
		{
			unsigned int largestSetFound = 0;

			for (map<string, vector<string>>::iterator i = sets.begin(); i != sets.end(); i++)
				if (i->second.size() > largestSetFound)
				{
					largestSetFound = i->second.size();
					cardToDiscard = i->second.back();
				}
		}
		else {
			unsigned int largestRowFound = 0;

			for (vector<vector<string>>::iterator i = rows.begin(); i != rows.end(); i++)
				if (i->size() > largestRowFound)
				{
					largestRowFound = i->size();
					cardToDiscard = i->back();
				}
		}

		// TODO: Add function to add card to discard pile.
		hand.erase(remove(hand.begin(), hand.end(), cardToDiscard), hand.end());
		if (debug) cout << debugPrefix << cardToDiscard << " was discarded.\n";

		if (CountSetCards() + CountRowCards() == 10)
			return true;
		return false;
}
