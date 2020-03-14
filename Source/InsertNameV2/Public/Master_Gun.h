// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Master_Pickup.h"
#include "Components/TimelineComponent.h"
#include "Master_Gun.generated.h"

class UPaperSpriteComponent;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class INSERTNAMEV2_API AMaster_Gun : public AMaster_Pickup
{
	GENERATED_BODY()


public:

  AMaster_Gun();

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Compoents")
  UBoxComponent* BarrelCollision;
  /* The socket to attach the gun to */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  FName SocketName;
  /* The actual transform to place the gun relative to the player */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  FTransform RealativeTransform;
  /* Whether or the the gun is actually equipped */
  UPROPERTY(BlueprintReadWrite, Category = "Gun Settings")
  bool bIsGunEquipped;
  /* Scale of the gun in world */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  FVector GunScale;
  /* How much to offset the gun position in world */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  FVector GunOffset;
  /* The scale of the laser beam in world */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  FVector TrailScale;
  /* The multiplier used to multiply the length of the Multi line ray trace equation is Trace Start * TraceMultipler */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  float TraceMultipler;
  /* Color of the gun's laser beam */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  FLinearColor TrailColor;
  /* Amount of time it takes for the the laser beam to Despawn */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  float TrailDespawn;
  /* The glow intensity of the gun laser beam */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  float TrailGlowAmount;
  /* The multiplier used to determine how much the gun will knock an enemy back equation is Trace Impact Point * KnockbackRange */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  float KnockBackMultipler;
  /* The multiplier used to multiply the length of the Multi line ray trace equation is Trace Start * KnockbackRangeMultipler */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  float KnockbackRangeMultipler;
  /* Whether or the should faded out of the scene */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  bool bFadeOut;
  /* The rate of fire the gun is shooting at */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  float FireRate;
  /* How much damage the gun will deal to the given enemy */
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gun Settings")
  float Damage;
  /* Will fire a multi line ray cast to look for enemies to damage */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gun Functions")
  void FireMultiLineTrace();
  virtual void FireMultiLineTrace_Implementation();
  /* Will fire a multi line ray cast to look for enemies to knockback */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gun Functions")
  void FireMultiLineKnockBack();
  virtual void FireMultiLineKnockBack_Implementation();
  /* Will stop the gun from firing it's raycast and apply the gun cooldown */
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gun Functions")
  void StopGunFire();
  virtual void StopGunFire_Implementation();
  /* Reads the value of bOnCooldown */
  UFUNCTION(BlueprintPure, Category = "Gun Functions")
  const bool GetGunOnCooldown();
  /* Gets the default damage */
  UFUNCTION(BlueprintPure, Category = "Gun Functions")
  const float GetDefaultDamage();
  /* Reads the value of bCanFireTrace */
  UFUNCTION(BlueprintPure, Category = "Gun Functions")
  const bool GetCanFireTrace();
  /* Reads the value of TraceStart */
  UFUNCTION(BlueprintPure, Category = "Gun Functions")
  const FVector GetTraceStart();
  /* Reads the value of TraceEnd */
  UFUNCTION(BlueprintPure, Category = "Gun Functions")
  const FVector GetTraceEnd();

  /* Upon player interacting with this object call player equip gun function */
  virtual void OnInteract_Implementation() override;

  // Called every frame
  virtual void Tick(float DeltaSeconds) override;

  UFUNCTION()
  void MouseTimelineProgress(float Value);

  virtual void AttackKeyPressed();

  virtual void AttackKeyReleased();

  UFUNCTION(BlueprintCallable, Category = "Gun Functions")
  void UpdateGunInput();

  bool bOnCooldown;

  virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

  UFUNCTION()
  void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  FTimeline MouseTimeline;

  UPROPERTY()
  UCurveFloat* CurveFloat;

private:

  bool bCanFireTrace;

  FVector TraceStart;
  
  FVector TraceEnd;

  /* Rotate gun towards the mouse */
  void RotateGunToMouse();

  float DefaultDamage;

  TArray<AActor*> HitActors;

  TArray<AActor*> KnockBackActors;

  void DamageHitActors();

  void ApplyKnockBack();
};
