#include "MirrorRealmProtocol/Gameplay/TheInstance.h"
#include "MirrorRealmProtocol/Gameplay/LocalDecks.h"
#include "Kismet/GameplayStatics.h"

void UTheInstance::Init()
{
	Super::Init();
	GeneratePlayerProfile();
}

void UTheInstance::GeneratePlayerProfile()
{
	//创建或加载卡组存档
	if (UGameplayStatics::LoadGameFromSlot(TEXT("LocalDecks"), 0)) return;
	ULocalDecks* NewLocalDeck = Cast<ULocalDecks>(UGameplayStatics::CreateSaveGameObject(ULocalDecks::StaticClass()));
	UGameplayStatics::SaveGameToSlot(NewLocalDeck, TEXT("LocalDecks"), 0);
}
