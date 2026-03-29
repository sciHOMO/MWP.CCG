#pragma once

#include "CoreMinimal.h"
#include "MirrorRealmProtocol/Gameplay/GameStruct.h"
#include "MirrorRealmProtocol/Interface/Interact.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "CardModel.generated.h"

struct FOrderUpdateEvent;
enum class ELocation : uint8;
enum class ETeamType : uint8;
class UVisualManager;

UENUM(BlueprintType)
enum class ECardState : uint8
{
	//卡牌基本状态枚举。
	
	PlaceHolder	UMETA(DisplayName = "PlaceHolder"),
	Anim				UMETA(DisplayName = "Anim"),
	LinearLep		UMETA(DisplayName = "LinearLep"),
	QuickLep			UMETA(DisplayName = "QuickLep"),
	Static				UMETA(DisplayName = "Static")
};

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API ACardModel : public AActor, public IInteract
{
	GENERATED_BODY()

	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;

public:	
	ACardModel();

	//组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* CardCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* PawnCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* CardMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PawnMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* CardOutline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* PawnOutline;
	
	//管理器
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVisualManager* VisualManager = nullptr;
	
	//卡牌参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCardInfo CardInfo = FCardInfo();
	
	//位置参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETeamType Team = ETeamType::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECardState CardState = ECardState::PlaceHolder;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalInHand;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int  IndexInHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform CacheTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform TargetTransform = FTransform::Identity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AdditiveLocation = FVector::ZeroVector;

	UFUNCTION()
	FVector CalculateAdditiveLocation();
	
	UFUNCTION(BlueprintCallable)
	void SetHandLocation();

	UFUNCTION(BlueprintCallable)
	void SetEntryTransform(const FIntPoint Index);

	//事件表现
	UFUNCTION()
	void ReceiveEvent(const FOrderUpdateEvent& Event);
	
	UFUNCTION()
	void HandleEvent();
	
	UFUNCTION()
	void Continue();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EventIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FOrderUpdateEvent> Events;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsProcessing = false;
	
	UFUNCTION()
	void LocalCardMove(const FOrderUpdateEvent& Event);

	UFUNCTION()
	void LocalCardAttack(const FOrderUpdateEvent& Event);

	UFUNCTION()
	void LocalCardTakeDamage(const FOrderUpdateEvent& Event);
	
	UFUNCTION()
	void LocalCardActivate(const FOrderUpdateEvent& Event);
	
	UFUNCTION()
	void LocalCardUpdate(const FOrderUpdateEvent& Event);

	//卡牌动画
	UFUNCTION(BlueprintNativeEvent)
	void PlayDrawAnim(const FOrderUpdateEvent& Event);

	UFUNCTION(BlueprintCallable)
	void DrawAnimEnd();

	UFUNCTION(BlueprintNativeEvent)
	void PlayEntryAnim(const FOrderUpdateEvent& Event);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayEntryAnim_C(const FOrderUpdateEvent& Event);

	UFUNCTION(BlueprintCallable)
	void EntryAnimEnd();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayLeaveAnim(const FOrderUpdateEvent& Event);

	UFUNCTION(BlueprintCallable)
	void LeaveAnimEnd();
	
	UFUNCTION(BlueprintNativeEvent)
	void PlayCastAnim(const FOrderUpdateEvent& Event);
	
	UFUNCTION(BlueprintCallable)
	void CastAnimEnd();
	
	UFUNCTION(BlueprintNativeEvent)
	void PlayAttackAnim(const FOrderUpdateEvent& Event);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AttackAnimEnd();

	UFUNCTION(BlueprintNativeEvent)
	void PlayDamageAnim(const FOrderUpdateEvent& Event);
	
	UFUNCTION(BlueprintCallable)
	void DamageAnimEnd();
	
	UFUNCTION(BlueprintNativeEvent)
	void PlayActivateAnim(const FOrderUpdateEvent& Event);

	UFUNCTION(BlueprintCallable)
	void ActivateAnimEnd();
	
	//卡牌呈现状态

	UFUNCTION()
	void ReceiveCardInfo(const FCardInfo& CI);

	UFUNCTION(BlueprintNativeEvent)
	void UpdateCardInfo(const FCardInfo& CI);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOverlap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSelected = false;
	
	virtual void StartOverlap_Implementation() override;
	virtual void StopOverlap_Implementation() override;
	virtual void StartBeSelected_Implementation() override;
	virtual void StopBeSelected_Implementation() override;
	
};
