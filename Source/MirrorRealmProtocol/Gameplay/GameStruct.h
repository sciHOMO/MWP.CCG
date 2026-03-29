#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameStruct.generated.h"

//****************************枚举类*****************************

class ACardModel;

UENUM(BlueprintType)
enum class ECardType : uint8
{
	//卡牌基本类型枚举。
	
	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Servant			UMETA(DisplayName = "Servant"),
	Spell				UMETA(DisplayName = "Spell")
};

UENUM(BlueprintType)
enum class ECardAttribute : uint8
{
	//卡牌属性类型枚举。

	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Divine				UMETA(DisplayName = "Divine"),
	Evil					UMETA(DisplayName = "Evil"), 
	Flame				UMETA(DisplayName = "Flame"),
	Aqua				UMETA(DisplayName = "Aqua"),
	Nature				UMETA(DisplayName = "Nature"),
	Creation			UMETA(DisplayName = "Craft")
};

UENUM(BlueprintType)
enum class ECardRace : uint8
{
	//卡牌种族类型枚举。

	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Human			UMETA(DisplayName = "Human"), 
	Second			UMETA(DisplayName = "Second"),
	Beast				UMETA(DisplayName = "Beast"),
	Plant				UMETA(DisplayName = "Plant"),
	Dragon			UMETA(DisplayName = "Dragon")								
};

UENUM(BlueprintType)
enum class ECardEffectType : uint8
{
	//卡牌效果类型枚举
	
	PlaceHolder		UMETA(DisplayName = "PlaceHolder"),
	Spell					UMETA(DisplayName = "Spell"),
	Activate					UMETA(DisplayName = "Activate"),
	BattleCry				UMETA(DisplayName = "BattleCry"),
	DeathRattle			UMETA(DisplayName = "DeathRattle"),
	AuraType				UMETA(DisplayName = "AuraType"),
	ListenEntry			UMETA(DisplayName = "ListenEntry"),
	ListenLeave			UMETA(DisplayName = "ListenLeave"),
	ListenSpell			UMETA(DisplayName = "ListenSpell"),
	ListenStartTurn	UMETA(DisplayName = "ListenStartTurn"),
	ListenEndTurn		UMETA(DisplayName = "ListenEndTurn")
};

UENUM(BlueprintType)
enum class ECardSpec : uint8
{
	//卡牌字段类型枚举。

	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Taunt				UMETA(DisplayName = "Taunt"), 
	Shield				UMETA(DisplayName = "Shield"),
	Talisman			UMETA(DisplayName = "Talisman"),
	Vanished			UMETA(DisplayName = "Vanished"),
	Ranged			UMETA(DisplayName = "Ranged"),
	Toxic				UMETA(DisplayName = "Toxic")
};

UENUM(BlueprintType)
enum class ETargetType : uint8
{
	//对象类型枚举。
	
	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Player				UMETA(DisplayName = "Player"),
	Line					UMETA(DisplayName = "Line"),
	Position			UMETA(DisplayName = "Position"),
	Card					UMETA(DisplayName = "Card")
};

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	//队伍类型枚举。
	
	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Owner				UMETA(DisplayName = "Owner"),
	Opponent		UMETA(DisplayName = "Opponent")
};

UENUM(BlueprintType)
enum class ELocation : uint8
{
	//位置枚举。
	
	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Deck				UMETA(DisplayName = "Deck"),
	Hand				UMETA(DisplayName = "Hand"),
	Board				UMETA(DisplayName = "Board"),
	Grave				UMETA(DisplayName = "Grave"),
	Banish				UMETA(DisplayName = "Banish")
};

UENUM(BlueprintType)
enum class EEventType : uint8
{
	//事件枚举。
	
	PlaceHolder		UMETA(DisplayName = "PlaceHolder"),
	Turn						UMETA(DisplayName = "Turn"),
	Win						UMETA(DisplayName = "Win"),
	Move					UMETA(DisplayName = "Move"),
	Attack					UMETA(DisplayName = "Attack"),
	Damage				UMETA(DisplayName = "Damage"),
	PlayerDamage		UMETA(DisplayName = "PlayerDamage"),
	Activate				UMETA(DisplayName = "Activate"),
	Influence				UMETA(DisplayName = "Influence"),
	Update				UMETA(DisplayName = "Update")
};

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	//事件枚举。
	
	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Awaiting		UMETA(DisplayName = "Awaiting"),
	Start				UMETA(DisplayName = "Start"),
	Continue		UMETA(DisplayName = "Continue"),
	End					UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
	//卡牌效果类型枚举
	
	PlaceHolder		UMETA(DisplayName = "PlaceHolder"),
	PreStart				UMETA(DisplayName = "PreStart"),
	Start					UMETA(DisplayName = "Start"),
	Continue			UMETA(DisplayName = "Continue"),
	PreEnd				UMETA(DisplayName = "PreEnd"),
	End						UMETA(DisplayName = "End"),
};

//****************************结构体类*****************************

USTRUCT(BlueprintType)
struct  MIRRORREALMPROTOCOL_API  FCardEffectTarget
{
	//卡牌效果选取对象限定条件的结构体
	//占位符表示不设限
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetType TargetType = ETargetType::PlaceHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeamType TeamType = ETeamType::PlaceHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELocation CardLocation = ELocation::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardType CardType = ECardType::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardAttribute CardAttribute = ECardAttribute::PlaceHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardRace CardRace = ECardRace::PlaceHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxCP = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinCP = INT_MIN;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxAP = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinAP = INT_MIN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxHP = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinHP = INT_MIN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Count = 0;
};

USTRUCT(BlueprintType)
struct  MIRRORREALMPROTOCOL_API  FCardEffect
{
	//卡牌效果结构体
	GENERATED_BODY()

	bool IsValid() const
	{
		return CardEffectType != ECardEffectType::PlaceHolder && CardEffectFunction != TEXT("");
	}

	bool operator ==(const FCardEffect& CE) const
	{
		return  CardEffectFunction == CE.CardEffectFunction;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//效果类型：战吼？亡语？等等
	ECardEffectType CardEffectType = ECardEffectType::PlaceHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//效果扳机条件
	FCardEffectTarget TriggerSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CardEffectFunction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Counter = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCardEffectTarget> CardEffectTargets;
};

USTRUCT(BlueprintType)
struct  MIRRORREALMPROTOCOL_API  FQueueEffect
{
	//等待队列处理的卡牌效果结构体
	GENERATED_BODY()

	bool IsValid() const
	{
		return CardEffectFunction != TEXT("");
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardInstID = INT_ERROR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCardEffectTarget TriggerSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//扳机来源信息（难道会出现大于来源1的情况吗）
	TArray<int> TriggerSourceInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CardEffectFunction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Counter = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCardEffectTarget> CardEffectTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//对象列表，按需填充
	TArray<int> CardInstTargets;
};

USTRUCT(BlueprintType)
struct  MIRRORREALMPROTOCOL_API FCardInfo
{
	//卡牌基础静态信息
	GENERATED_BODY()
	
	bool IsValid() const
	{
		return CardID != INT_MAX && CardType != ECardType::PlaceHolder;
	}
	
	static bool AreMapsEqual(const TMap<ECardSpec, int32>& A, const TMap<ECardSpec, int32>& B)
	{
		if (A.Num() != B.Num())
		{
			return false;
		}

		for (const auto& Pair : A)
		{
			if (const int32* ValuePtr = B.Find(Pair.Key); !ValuePtr || *ValuePtr != Pair.Value)
			{
				return false;
			}
		}
		return true;
	}
	 

	bool operator ==(const FCardInfo& CI) const
	{
		return
		CardInstID == CI.CardInstID &&
		CardID == CI.CardID &&
		Cost == CI.Cost &&
		RealCost == CI.RealCost &&
		Name == CI.Name &&
		CardType == CI.CardType &&
		CardAttribute == CI.CardAttribute &&
		CardRace == CI.CardRace &&
		CardEffects == CI.CardEffects &&
		AreMapsEqual(CardSpecs, CI.CardSpecs) &&
		AP == CI.AP && RealAP == CI.RealAP &&
		HP == CI.HP && RealHP == CI.RealHP &&
		Action == CI.Action &&
		PlayerID == CI.PlayerID &&
		Location == CI.Location &&
		Position == CI.Position;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//卡牌实例序列号
	int CardInstID = INT_MAX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardID = INT_MAX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cost = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RealCost = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = TEXT("NULL");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardType CardType = ECardType::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardAttribute CardAttribute = ECardAttribute::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardRace CardRace = ECardRace::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCardEffect> CardEffects = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ECardSpec, int> CardSpecs = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AP = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RealAP = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HP = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RealHP = INT_MAX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Action = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//实际拥有者
	int PlayerID = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//实际拥有者
	ELocation Location = ELocation::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//位置信息
	FIntPoint Position = FIntPoint::NoneValue;
};

USTRUCT(BlueprintType)
struct  MIRRORREALMPROTOCOL_API FOrderUpdateEvent
{
	//游戏本地顺序更新事件
	GENERATED_BODY()
	
	bool operator==(const FOrderUpdateEvent& Other) const
	{
		return GlobalEventID == Other.GlobalEventID;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEventType EventType = EEventType::PlaceHolder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GlobalEventID = INT32_MIN;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> IntInfo = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ELocation> LocationInfo = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntPoint> PositionInfo = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCardInfo> SCardInfo = {};
};

//****************************数据库类*****************************

USTRUCT(BlueprintType)
struct  MIRRORREALMPROTOCOL_API FCardPreInfo : public FTableRowBase
{
	//卡牌基础静态信息
	GENERATED_BODY()
	
	bool IsValid() const
	{
		return CardID != INT_MAX && CardType != ECardType::PlaceHolder;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardID = INT_MAX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cost = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name = TEXT("NULL");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardType CardType = ECardType::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardAttribute CardAttribute = ECardAttribute::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardRace CardRace = ECardRace::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCardEffect> CardEffects = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ECardSpec, int> CardSpecs = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AP = INT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HP = INT_MAX;
};

USTRUCT(BlueprintType)
struct  MIRRORREALMPROTOCOL_API FCardModels : public FTableRowBase
{
	//卡牌实体模型与UMG模型数据库
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardID = INT_MAX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CardName = TEXT("NULL");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<ACardModel> CardModelClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> CardUIClass;
};