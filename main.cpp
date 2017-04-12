#include "npc.h"
using namespace std;

int main()
{
	cout << "What name would you grant your opponent?\n";
	string name = "";
	getline(cin, name);

	NonPlayerControlled bot(name, vector<string>(10)={"d2","c2","s2","h1","d3","d4","h10","h11","h12","s1"}, true);
	bot.Play("h13");

	cout << "\nBot's deck contains:\n";
		bot.OutputHand();

	cout << "\n"; system("pause");
	return 0;
}
