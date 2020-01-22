// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Master_Enemy.generated.h"

class UBlueprint;
class UBehaviorTree;
class AMaster_AIController;

UCLASS()
class INSERTNAMEV2_API AMaster_Enemy : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:

  AMaster_Enemy();
  
  UPROPERTY(EditAnywhere, Category = "HP Values")
  float CurrentHP;

  UPROPERTY(EditAnywhere, Category = "HP Values")
  float MaxHP;

  UPROPERTY(EditAnywhere, Category = "HP Values")
  float ActorDespawnDelay;

  UPROPERTY(EditAnywhere, Category = "HP Values")
  float HPBarHideDelay;

  UPROPERTY(EditAnywhere, Category = "HP Values")
  bool bCanBeStunned;

  UPROPERTY(EditAnywhere, Category = "AI", meta = (EditCondition = "bUseBT"))
  UBehaviorTree* BehaviorTreeToUse;

  UPROPERTY(EditAnywhere, Category = "AI")
  bool bUseBT;

  UPROPERTY(EditAnywhere, Category = "AI")
  bool bAddToKillCount;

  UPROPERTY(EditAnywhere, Category = "AI")
  TSubclassOf <AMaster_AIController> ControllerToUse;

  UPROPERTY(BlueprintReadOnly, Category = "AI")
  bool bAggroed;

  UPROPERTY(EditAnywhere, Category = "Damage")
  float DamageToPlayer;

  UFUNCTION(BlueprintPure, Category = "Getter Functions")
  const FVector GetHomeLocation();

  UFUNCTION(BlueprintPure, Category = "Getter Functions")
  const int32 GetID();

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
  void DamageEnemy(float Damage);
  void DamageEnemy_Implementation(float Damage);

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
  void OnDeath();
  void OnDeath_Implementation();

protected:

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
  
private:
  int32 AssignID();

  void RemoveIDFromGamemode();

  UBlueprint* WarlustEffect;

  int32 ID;

  bool bIsDead;

  bool bTakenDamage;

  FVector HomeLocation;

  FLinearColor DefaultColor;

  bool bIsStunned;

  float StunDuration;

  bool bIsJumping;

  float DefaultSpeed;

  float DefaultGravityScale;

  float DefaultMaxAcceleration;

  bool bWasKnockedBack;

  bool bOverlapedPlayer;

  bool bHitPlayer;

  bool bIsPlayerOnEnemy;
};
