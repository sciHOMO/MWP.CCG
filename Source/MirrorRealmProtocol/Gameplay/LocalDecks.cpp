#include "MirrorRealmProtocol//Gameplay/LocalDecks.h"

TArray<int> ULocalDecks::FindDeckByIndex(const int Index) const
{
	switch(Index)
	{
	case 1 :
		{
			return Deck_001;
		}
	case 2 :
		{
			return Deck_002;
		}
	case 3 :
		{
			return Deck_003;
		}
	case 4 :
		{
			return Deck_004;
		}
	case 5 :
		{
			return Deck_005;
		}
	case 6 :
		{
			return Deck_006;
		}
	case 7 :
		{
			return Deck_007;
		}
	case 8 :
		{
			return Deck_008;
		}
	default : break;
	}
	return {};
}
