#include "npc.h"
using namespace std;

int main()
{
	NonPlayerControlled bot("Kanna", vector<string>(10)={"d2","c2","s2","d7","d8","d9","h9","h10","h11","h12"}, true);
	bot.Play("d13");

	cout << "\n"; system("pause");
	return 0;
}
