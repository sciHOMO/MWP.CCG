#include "MirrorRealmProtocol/Gameplay/VisualManager.h"
#include "MirrorRealmProtocol/Gameplay/CardModel.h"
#include "MirrorRealmProtocol/Gameplay/MainServer.h"
#include "MirrorRealmProtocol/Gameplay/GameStruct.h"
#include "MirrorRealmProtocol/Gameplay/BoardGrid.h"
#include "MirrorRealmProtocol/Gameplay/GameController.h"
#include "MirrorRealmProtocol/Interface/ToUMG.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"

class UWidget;

void UVisualManager::BeginPlay()
{
	//加载模型库
	CardModelLib = LoadObject<UDataTable>(nullptr,TEXT("/Game/Misc/Library/CardModelLib.CardModelLib"));

	//棋盘格同步
	GridMap.Empty();
	TArray<AActor*> FoundGrid;
	UGameplayStatics::GetAllActorsOfClass(Controller -> GetWorld(), ABoardGrid::StaticClass(), FoundGrid);
	for (AActor* Actor : FoundGrid)
	{
		GridMap.FindOrAdd(Cast<ABoardGrid>(Actor) -> Index, Cast<ABoardGrid>(Actor));
	}
}

void UVisualManager::ReceiveEvent(const FOrderUpdateEvent& NewEvent)
{
	Events.AddUnique(NewEvent);
	const FOrderUpdateEvent* FoundEvent = Events.FindByPredicate([this](const FOrderUpdateEvent& Item) {return Item.GlobalEventID == LocalEventID;});	//记住这个Lambda搜索模式
	if (!bIsProcessing && FoundEvent)
	{
		HandleEvent();
	}
}

void UVisualManager::HandleEvent()
{
	const FOrderUpdateEvent* FoundEvent = Events.FindByPredicate([this](const FOrderUpdateEvent& Item) {return Item.GlobalEventID == LocalEventID;});
	if (!bIsProcessing && FoundEvent)
	{
		bIsProcessing = true;
		switch (FoundEvent -> EventType)
		{
		case EEventType::Turn :
			{
				TurnChange(*FoundEvent);
				break;
			}
		case EEventType::Win :
			{
				ShowWinner(*FoundEvent);
				break;
			}
		case EEventType::Move :
			{
				LocalCardMove(*FoundEvent);
				break;
			}
		case EEventType::Attack :
			{
				LocalCardAttack(*FoundEvent);				
				break;
			}
		case EEventType::Damage :
			{
				LocalCardDamage(*FoundEvent);
				break;
			}
		case EEventType::PlayerDamage :
			{
				LocalPlayerDamage(*FoundEvent);				
				break;
			}
		case EEventType::Activate :
			{
				LocalCardActivate(*FoundEvent);	
				break;
			}
		case EEventType::Update :
			{
				LocalCardUpdate(*FoundEvent);	
				break;
			}	
		default : break;
		}
	}
}

void UVisualManager::Continue()
{
	bIsProcessing = false;
	Events.RemoveAll([this](const FOrderUpdateEvent& Item) {return Item.GlobalEventID == LocalEventID;});	//记住这个Lambda删除模式
	LocalEventID++;
	const FOrderUpdateEvent* FoundEvent = Events.FindByPredicate([this](const FOrderUpdateEvent& Item) {return Item.GlobalEventID == LocalEventID;});
	if (!bIsProcessing && FoundEvent)
	{
		HandleEvent();
	}
}

void UVisualManager::Test(const FOrderUpdateEvent& Event)
{
	if (GEngine)
	{

		const FString EventTypeString = UEnum::GetValueAsString(Event.EventType);
		const FString Message = FString::Printf(TEXT("Event Type: %s, GlobalID: %d"),
			*EventTypeString, Event.GlobalEventID);
		
		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, Message);
	}
}

void UVisualManager::TurnChange(const FOrderUpdateEvent& Event)
{
	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld(), AllWidgets, UToUMG::StaticClass(), true);
	IToUMG::Execute_TurnChange(AllWidgets[0], Event.IntInfo[0]);
	Continue();
}

void UVisualManager::ShowWinner(const FOrderUpdateEvent& Event)
{
	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld(), AllWidgets, UToUMG::StaticClass(), true);
	IToUMG::Execute_ShowWinner(AllWidgets[0], Event.IntInfo[0]);
	Continue();
}

void UVisualManager::LocalCardMove(const FOrderUpdateEvent& Event)
{
	if (Event.LocationInfo.Num() == 2 && Event.LocationInfo[0] == ELocation::Deck && Event.LocationInfo[1] == ELocation::Hand)
	{
		//加载模型数据库
		if (!CardModelLib) return;

		//基于CardInstID查找对应卡牌
		const FCardInfo* FoundCard = Controller -> Server -> GetCardInstCopybyID(Event.IntInfo[0]);
		if (!FoundCard) {return;}

		//找到模型
		const FCardModels* CardModel = CardModelLib -> FindRow<FCardModels>(FName(*FString::FromInt(FoundCard -> CardID)), TEXT("Not Found"));
		if (!CardModel || CardModel -> CardModelClass.IsNull()) {return;}
		UClass* CardModelClass =  CardModel -> CardModelClass.LoadSynchronous();
		if (!CardModelClass) {return;}

		//确定生成位置
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(FVector::ZeroVector);
		SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
		SpawnTransform.SetScale3D(FVector::OneVector);

		//生成卡牌
		ACardModel* NewCard = Controller -> GetWorld() -> SpawnActor<ACardModel>(CardModelClass, SpawnTransform);
		AllCards.Emplace(NewCard);
		NewCard -> VisualManager = this;
		NewCard -> ReceiveCardInfo(Event.SCardInfo[0]);
		NewCard -> Team = Event.IntInfo[1] == Controller -> PlayerState -> GetPlayerId() ? ETeamType::Owner : ETeamType::Opponent;
		NewCard -> Location = ELocation::Hand;
		NewCard -> Position = FIntPoint::NoneValue;
		DecideHandLocation(NewCard);
		NewCard -> PlayDrawAnim(Event);
		return;
	}
	if (Event.LocationInfo.Num() == 2 && Event.LocationInfo[0] == ELocation::Hand && Event.LocationInfo[1] == ELocation::Board)
	{
		for (ACardModel* Card : AllCards)
		{
			if (Card && Card -> CardInfo.CardInstID == Event.IntInfo[0])
			{
				Card -> ReceiveCardInfo(Event.SCardInfo[0]);
				Card -> Location = ELocation::Board;
				Card -> Position = Event.PositionInfo[1];
				ModifyHandLocation(Card);
				Card -> PlayEntryAnim(Event);
				return;
			}
		}
		Continue();
		return;
	}
	if (Event.LocationInfo.Num() == 2 && Event.LocationInfo[0] == ELocation::Hand && Event.LocationInfo[1] == ELocation::Grave)
	{
		for (ACardModel* Card : AllCards)
		{
			if (Card && Card -> CardInfo.CardInstID == Event.IntInfo[0])
			{
				Card -> ReceiveCardInfo(Event.SCardInfo[0]);
				Card -> Location = ELocation::Grave;
				Card -> Position = FIntPoint::NoneValue;
				ModifyHandLocation(Card);
				Card -> PlayCastAnim(Event);
				return;
			}
		}
		Continue();
		return;
	}
	if (Event.LocationInfo.Num() == 2 && Event.LocationInfo[0] == ELocation::Board && Event.LocationInfo[1] == ELocation::Grave)
	{
		for (ACardModel* Card : AllCards)
		{
			if (Card && Card -> CardInfo.CardInstID == Event.IntInfo[0])
			{
				Card -> ReceiveCardInfo(Event.SCardInfo[0]);
				Card -> Location = ELocation::Grave;
				Card -> Position = FIntPoint::NoneValue;
				Card -> TryKill();
				Continue();
				return;
			}
		}
		Continue();
		return;
	}
	if (Event.LocationInfo.Num() == 2 && Event.LocationInfo[0] == ELocation::Grave && Event.LocationInfo[1] == ELocation::Banish)
	{
		return;
	}
}

void UVisualManager::LocalCardAttack(const FOrderUpdateEvent& Event)
{
	for (ACardModel* Card : AllCards)
	{
		if (Card && Card-> CardInfo.CardInstID == Event.IntInfo[0])
		{
			Card -> ReceiveCardInfo(Event.SCardInfo[0]);
			Card -> PlayAttackAnim(Event);
			return;
		}
	}
	Continue();
}

void UVisualManager::LocalCardDamage(const FOrderUpdateEvent& Event)
{
	for (ACardModel* Card : AllCards)
	{
		if (Card && Card -> CardInfo.CardInstID == Event.IntInfo[0])
		{
			Card -> ReceiveCardInfo(Event.SCardInfo[0]);
			Card -> PlayDamageAnim(Event);
			return;
		}
	}
	Continue();
}

void UVisualManager::LocalPlayerDamage(const FOrderUpdateEvent& Event)
{
	Continue();
}

void UVisualManager::LocalCardActivate(const FOrderUpdateEvent& Event)
{
	for (ACardModel* Card : AllCards)
	{
		if (Card && Card -> CardInfo.CardInstID == Event.IntInfo[0])
		{
			if (Controller -> Server -> GetCardInstCopybyID( Event.IntInfo[0]) -> CardType == ECardType::Servant)
			{
				Card -> ReceiveCardInfo(Event.SCardInfo[0]);
				Card -> PlayActivateAnim(Event);
				return;
			}
		}
	}
		Continue();
}

void UVisualManager::LocalCardUpdate(const FOrderUpdateEvent& Event)
{
	for (ACardModel* Card : AllCards)
	{
		if (Card && Card -> CardInfo.CardInstID == Event.IntInfo[0])
		{
			Card -> ReceiveCardInfo(Event.SCardInfo[0]);
			Continue();
			return;
		}
	}
	Continue();
}

void UVisualManager::DecideHandLocation(ACardModel* Model)
{
	int Total_P0 = 0;
	int Total_P1 = 0;
	
	//遍历现有卡牌
	for (ACardModel* Card : AllCards)
	{
		if (Card && Card -> CardInfo.PlayerID == 0 && Card -> Location == ELocation::Hand)
		{
			Total_P0 ++;
		}
		if (Card && Card -> CardInfo.PlayerID == 1 && Card -> Location == ELocation::Hand)
		{
			Total_P1 ++;
		}
	}
	
	//设置该卡牌的Index
	Model -> TotalInHand = Model -> CardInfo.PlayerID == 0 ? Total_P0 : Total_P1;
	Model -> IndexInHand = Model -> CardInfo.PlayerID == 0 ? Total_P0 - 1 : Total_P1 - 1;
	Model -> SetHandLocation();
	
	//设置其他卡牌的Index
	for (ACardModel* Card : AllCards)
	{
		if (Card -> CardInfo.PlayerID == 0 && Card -> Location == ELocation::Hand)
		{
			Card -> TotalInHand = Total_P0;
			Card -> SetHandLocation();
		}
		if (Card -> CardInfo.PlayerID == 1 && Card -> Location == ELocation::Hand)
		{
			Card -> TotalInHand = Total_P1;
			Card -> SetHandLocation();
		}
	}
}

void UVisualManager::ModifyHandLocation(ACardModel* Model)
{
	for (ACardModel* Card : AllCards)
	{
		if (Card&& Card -> CardInfo.PlayerID == Model -> CardInfo.PlayerID && Card -> Location == ELocation::Hand)
		{
			Card -> TotalInHand--;
			if (Card -> IndexInHand > Model -> IndexInHand) Card -> IndexInHand--;
			Card -> SetHandLocation();
		}
	}
}

	
