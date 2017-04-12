#include "npc.h"

// Failsafe constructor. Failsafe for inproper programmer usage. Only string, vector<string, bool creation allowed.
NonPlayerControlled::NonPlayerControlled()
{
	cerr << "Non player character's cannot be created without three given parameters of string, vector<string> [size 10], and bool values.\n";
	system("pause");
	exit(1);
}

// Standard constructor. Check given inputs to make sure a deck hand of 10 was given. If no name is given, default to Larry.
NonPlayerControlled::NonPlayerControlled(const string n, const vector<string>& h, const bool d) : debug(d)
{
	try
	{
		if (n.empty())
			throw n;
		name = n;
	}
	catch (string e)
	{
		if (debug) cerr << "Name parameter was empty, program will default to name of \"Larry\".\n";
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

// Work in progress. Do not translate.
void NonPlayerControlled::OutputHand()
{
	if (debug)
	{
		cout << debugPrefix << name << "'s current deck: ";
		for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
			cout << *i << " ";

		cout << "\n";
	}
}

// The function will first find temporary sets and rows, determine which of which to use for it's turn, then adjust class values with selection.
void NonPlayerControlled::DetermineMelds()
{
	sort(hand.begin(), hand.end());

	// Find all possible sets that could be formed.
	map<string, unsigned int> suitelessValueCount;
	map<string, vector<string>> possibleSets;

	for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
		if (!(suitelessValueCount.insert(pair<string, int>(string(*i).erase(0, 1), 1)).second))
			suitelessValueCount[string(*i).erase(0, 1)] += 1;

	for (map<string, unsigned int>::iterator i = suitelessValueCount.begin(); i != suitelessValueCount.end(); i++)
		// Since only sets of 3 and 4 are possible due to (Spade, Diamond, Heart, Clover), no fail safe is needed.
		if (i->second > 2)
		{
			if (possibleSets.find(i->first) == possibleSets.end())
				possibleSets.insert(pair<string, vector<string> >(i->first, vector<string>()));

			if (debug) cout << debugPrefix << "map<string, vector<string>> possibleSets(" << i->first << ") size of current vs recorded: " << i->second << " : " << possibleSets.find(i->first)->second.size() << "\n";

			if (i->second != possibleSets.find(i->first)->second.size())
			{
				possibleSets.find(i->first)->second.clear();

				if (debug) cout << "\t" << debugPrefix;
				for (vector<string>::iterator j = hand.begin(); j != hand.end(); j++)
					if (string(*j).erase(0, 1) == i->first)
					{
						possibleSets.find(i->first)->second.push_back(*j);
						if (debug) cout << "(" << *j << ") ";
					}
				if (debug) cout << "added to set [" << i->first << "] in map<string, vector<string>> possibleSets.\n";
			}
		}
		else
		{
			// If the set is no longer possible, ensure it is not within the possible set list.
			if (possibleSets.find(i->first) != possibleSets.end())
			{
				possibleSets.erase(i->first);
				if (debug) cout << debugPrefix << i->first << " was deleted from map<string, vector<string>> possibleSets.\n";
			}
		}

		// Now that all possible sets are accounted for, find all possible rows that can be formed.
		unordered_map<string, vector<string>> seperatedSuites;
		vector<vector<string>> possibleRows;

		for (vector<string>::iterator i = hand.begin(); i != hand.end(); i++)
		{
			if (seperatedSuites.find(to_string((*i)[0])) == seperatedSuites.end())
				seperatedSuites.insert(pair<string, vector<string>>(to_string((*i)[0]), vector<string>()));

			seperatedSuites.find(to_string((*i)[0]))->second.push_back(string(*i).erase(0, 1));
		}

		// Sort the suites in number based ascending value instead of character based.
		for (unsigned int k = 0; k < seperatedSuites.size(); k++)
			for (unordered_map<string, vector<string>>::iterator i = seperatedSuites.begin(); i != seperatedSuites.end(); i++)
				for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
					if ((next(j) != i->second.end()) && (stoi(*j) > stoi(*next(j))))
					{
						string temp = *j;
						*j = *next(j);
						*next(j) = temp;
					}

		// For each suite, find the lowest value card and see if the next increment is found. If this passes twice, we must have a row of at least 3.
		for (unordered_map<string, vector<string>>::iterator i = seperatedSuites.begin(); i != seperatedSuites.end(); i++)
		{
			string rowCardBegun;
			unsigned int successivePassed = 0;
			vector<string> rowPushed;

			for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
			{
				if ((next(j) != i->second.end()) && (*next(j) == to_string(stoi(*j) + 1)))
				{
					if (successivePassed == 0)
						rowCardBegun = *j;

					successivePassed++;
				}
				else 
				{
					if (successivePassed >= 2)
					{
						// The following char() conversion is important, as the string is converted to int for ease of foor loop purposes. ASCII simply handles better.
						if (debug) cout << debugPrefix;

						for (unsigned int k = 0; k <= successivePassed; k++)
						{
							rowPushed.push_back(char(stoi(i->first)) + *find(i->second.begin(), i->second.end(), to_string(stoi(rowCardBegun) + k)));
							if (debug) cout << "(" << char(stoi(i->first)) + to_string(stoi(rowCardBegun) + k) << ") ";
						}

						if (debug) cout << "push_back to vector<vector<string>> possibleRows.\n";

						possibleRows.push_back(rowPushed);
					}

					successivePassed = 0;
				}
			}
		}

		vector<string> invalidSets;
		vector<string> invalidRows;

		// Determine which sets have a card that also exists within a row. Then, if found, determine if the set removes more deadwood.
		for (map<string, vector<string>>::iterator i = possibleSets.begin(); i != possibleSets.end(); i++)
			for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
				for (vector<vector<string>>::iterator k = possibleRows.begin(); k != possibleRows.end(); k++)
					for (vector<string>::iterator l = k->begin(); l != k->end(); l++)
						if (*j == *l)
						{
							if (debug) cout << "\n";

							if (distance(i->second.begin(), i->second.end()) <= distance(k->begin(), k->end()))
							{
								invalidSets.push_back(i->first);
								if (debug) cout << debugPrefix << "Set (map<string, vector<string>>)[" << (i->first) << "] invalid, "
									<< "(vector<vector<string>>)[" << to_string(distance(k->begin(), find(k->begin(), k->end(), *l))) << "] is " 
									<< distance(k->begin(), k->end()) - distance(i->second.begin(), i->second.end()) << " element(s) larger.\n";
							}
							else
							{
								invalidRows.push_back( to_string(distance(k->begin(), find(k->begin(), k->end(), *l))) );
								if (debug) cout << debugPrefix << "Row (vector<vector<string>>)[" << to_string(distance(k->begin(), find(k->begin(), k->end(), *l))) << "] invalid, "
									<< "(map<string, vector<string>>)[" << (i->first) << "] is " << distance(i->second.begin(), i->second.end()) - distance(k->begin(), k->end())
									<< " element(s) larger.\n";
							}
						}

		for (vector<string>::iterator i = invalidSets.begin(); i != invalidSets.end(); i++)
			possibleSets.erase(possibleSets.find(*i));

		for (vector<string>::iterator i = invalidRows.begin(); i != invalidRows.end(); i++)
			possibleRows.erase(possibleRows.begin() + stoi(*i));

		// Finaly, set essential class variables to temporary variables decided upon by this function.
		for (map<string, vector<string>>::iterator i = possibleSets.begin(); i != possibleSets.end(); i++)
			sets.insert(possibleSets.begin(), possibleSets.end());
		rows = possibleRows;
}

// Overwrite the bot's deadwood varaible with any card not in a meld.
void NonPlayerControlled::DetermineDeadwood()
{
	deadwood.erase(deadwood.begin(), deadwood.end());
	if (debug) cout << debugPrefix << "Deadwood cards are: ";

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
			if (debug) cout << "(" << *i << ") ";
		}
	}

	sort(deadwood.begin(), deadwood.end());
	if (debug) cout << "\n";
}

// Count how many cards are within a set.
const unsigned int NonPlayerControlled::CountSetCards()
{
	unsigned int count = 0;

	for (map<string, vector<string>>::iterator i = sets.begin(); i != sets.end(); i++)
		for (vector<string>::iterator j = i->second.begin(); j != i->second.end(); j++)
			count++;

	return count;
}

// Count how many cards are within a row.
const unsigned int NonPlayerControlled::CountRowCards()
{
	unsigned int count = 0;

	for (vector<vector<string>>::iterator i = rows.begin(); i != rows.end(); i++)
		for (vector<string>::iterator j = i->begin(); j != i->end(); j++)
			count++;

	return count;
}

// The primary function to call from other files/classes. This will complete one turn for the npc and return either 0, 1, or 2. See the header file for explanation.
const unsigned int NonPlayerControlled::Play(const string discardCard)
{
	currentTurn++;
	if (debug) cout << debugPrefix << name << "'s turn, #" << currentTurn << "\n";
	if (debug) { cout << "\t"; for (unsigned int i = 0; i < 50; i++)cout << "-="; cout << "\n"; }

	// First, card drawing! If discard can be used to decrease deadwood instantly, use it. Otherwise pull random. \(*・ω・)ﾉ
	DetermineMelds();
	unsigned int currentSetsCount = CountSetCards();
	unsigned int currentRowsCount = CountRowCards();
	string addedCard = "";

	// Determine if the discard card will be used to make an immediate increase in meld.
	hand.push_back(discardCard);
	if (debug) { cout << "\t"; for (unsigned int i = 0; i < 50; i++)cout << "-="; cout << "\n"; }
	if (debug) cout << debugPrefix << discardCard << " was added to vector<string> hand.\n";

	DetermineMelds();

	if (debug) { cout << "\t";for (unsigned int i = 0; i < 50; i++)cout << "-="; cout << "\n"; }

	if (currentSetsCount == CountSetCards() && currentRowsCount == CountRowCards())
	{
		if (debug) cout << debugPrefix << "There was no change in melds found, discard card deemed to risky.\n";

		hand.erase(remove(hand.begin(), hand.end(), discardCard), hand.end());
		if (debug) cout << debugPrefix << discardCard << " was removed from vector<string> hand.\n";

		addedCard = "Unknown Stack Card";
		// TODO: Take the card from the unknown deck.
		// TODO: Pushback new card to hand.
		if (debug) cout << debugPrefix << addedCard << " was added to vector<string> hand.\n";

		DetermineMelds(); // Recalculate melds as previous iteration is with discard card in hand.
	}
	else
	{
		if (debug) cout << debugPrefix << "There was a change in melds found, discard card deemed worthy.\n";
		addedCard = discardCard;
		// TODO: Take the card from the deck.
	}
	
	DetermineDeadwood();

	// Check if with this newly drawn card, we can declare victory or not.
	if (deadwood.size() == 0)
		return 2;

	// Second, determine what card to add to discard pile.
	string cardToDiscard = "";
	string highestCardFound = "";
	int highestSuitelessValue = 0;

	if (debug) { cout << "\t"; for (unsigned int i = 0; i < 50; i++)cout << "-="; cout << "\n"; }
	for (vector<string>::iterator i = deadwood.begin(); i != deadwood.end(); i++)
		if (highestSuitelessValue < stoi(string(*i).erase(0, 1)) && (*i != addedCard))
		{
			highestSuitelessValue = stoi(string(*i).erase(0, 1));
			highestCardFound = *i;

			if (debug) cout << debugPrefix << "string highestCardFound set to " << *i << ".\n";
			if (debug) cout << debugPrefix << "int highestSuitelessValue set to " << stoi(string(*i).erase(0, 1)) << ".\n";
		}
	if (debug) { cout << "\t"; for (unsigned int i = 0; i < 50; i++)cout << "-="; cout << "\n"; }

	// As long as we are not trying to discard a 2 as our highest deadwood, first action. Otherwise, check if an ace can be discarded first.
	cardToDiscard = highestCardFound;

	if (highestSuitelessValue == 2)
		for (vector<string>::iterator i = deadwood.begin(); i != deadwood.end(); i++)
			if (stoi(string(*i).erase(0, 1)) == 1)
				cardToDiscard = *i;

	hand.erase(remove(hand.begin(), hand.end(), cardToDiscard), hand.end());
	if (debug) cout << debugPrefix << cardToDiscard << " was discarded.\n";

	// Finally, check if after discarding a card that is not the drawn card, if we have no deadwood left for a standard win.
	DetermineDeadwood();
	if (deadwood.size() == 0)
		return 1;

	return 0;
}

// All work and no play leaves students up at 2 AM testing codes. ٩(ˊ〇ˋ*)و
