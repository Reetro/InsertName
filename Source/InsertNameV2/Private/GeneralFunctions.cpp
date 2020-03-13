// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralFunctions.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "SideScrollerGamemode.h"
#include "Master_Pickup.h"
#include "Master_Enemy.h"
#include "InsertNameV2.h"
#include "WardenCameraManager.h"
#include "LeechInnerEggBase.h"
#include "MasterDamageEffect.h"
#include "Camera/PlayerCameraManager.h"
#include "PaperWarden.h"
#include "PaperCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PaperZDCharacter.h"
#include <iostream>
#include <random>
#include <time.h>

using namespace std;

bool UGeneralFunctions::IsNumberNegative(float number)
{
  if (number >= 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

float UGeneralFunctions::TickDamage(float Tick, float GunDamage, float BuffAmount)
{
  float temp1 = Tick * BuffAmount + 1;
  float temp2 = temp1 * GunDamage;
  return temp2;
}

FVector UGeneralFunctions::CalcReflectionVector(FVector VectorToReflect, FVector Normal)
{
  float DotProduct = FVector::DotProduct(VectorToReflect, Normal);
  FVector NewVector = 1 * -2 * DotProduct * Normal + VectorToReflect;
  return NewVector;
}

FVector2D GetGameViewportSize()
{
  FVector2D Result = FVector2D(1, 1);

  if (GEngine && GEngine->GameViewport)
  {
    GEngine->GameViewport->GetViewportSize(Result);
  }

  return Result;
}

float x = 0;
float resX = 0;
bool UGeneralFunctions::MouseLeftOrRight(float mouseX, float mouseY)
{
  resX = GetGameViewportSize().X;
  resX = resX / 2;
  if (mouseX < resX)
  {
    return false;
  }
  else
  {
    return true;
  }
}

int32 UGeneralFunctions::RandomNumber(int32 max, int32 min)
{
  if (min > max) {
    int32 temp = max;
    max = min;
    min = temp;
  }

  static bool first = true;
  if (first)
  {
    srand(time(NULL)); //seeding for the first time only!
    first = false;
  }
  return min + rand() % ((max + 1) - min);
}

int32 UGeneralFunctions::GetIDFromGamemode(UObject* WorldContextObject, AActor* ActorToAssign)
{
  ASideScrollerGamemode* LocalGameMode = Cast<ASideScrollerGamemode>(WorldContextObject->GetWorld()->GetAuthGameMode());

  if (LocalGameMode)
  {
    return LocalGameMode->GenID();
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Warning, TEXT("Unable to assing ID cast to ASideScrollerGamemode failed assign ID to : %s"), *ActorToAssign->GetName())
    return 0;
  }
}

void UGeneralFunctions::RemoveIDFromGamemode(UObject* WorldContextObject, int32 ID, AActor* ActorToRemove)
{
  ASideScrollerGamemode* LocalGameMode = Cast<ASideScrollerGamemode>(WorldContextObject->GetWorld()->GetAuthGameMode());

  if (LocalGameMode)
  {
    LocalGameMode->RemoveID(ID);
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Warning, TEXT("Unable to assing ID cast to ASideScrollerGamemode failed to remove ID from : %s"), *ActorToRemove->GetName())
  }
}

void UGeneralFunctions::DisplayItemObtainMessage(UObject* WorldContextObject, AMaster_Pickup* Pickup, int32 Amount)
{
  ASideScrollerGamemode* LocalGameMode = Cast<ASideScrollerGamemode>(WorldContextObject->GetWorld()->GetAuthGameMode());

  if (LocalGameMode)
  {
    LocalGameMode->DisplayItemObtainedMessage(Pickup, Amount);
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Warning, TEXT("Unable to create message cast to ASideScrollerGamemode failed to display item : %s"), *Pickup->GetName())
  }
}

void UGeneralFunctions::CreateOnScreenMessage(UObject* WorldContextObject, const FString& Message)
{
  ASideScrollerGamemode* LocalGameMode = Cast<ASideScrollerGamemode>(WorldContextObject->GetWorld()->GetAuthGameMode());

  if (LocalGameMode)
  {
    FText localizedText = FText::FromString(Message);

    LocalGameMode->DisplayTextMessage(localizedText);
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Warning, TEXT("Unable to create on screen message cast ASideScrollerGamemode failed"))
  }
}

void UGeneralFunctions::RemoveActorsY(UObject* WorldContextObject, AActor* ActorRef)
{
  if (WorldContextObject && ActorRef)
  {
    float ActorsX = ActorRef->GetActorLocation().X;
    float ActorsZ = ActorRef->GetActorLocation().Z;
    FVector FilterdLocation = FVector(ActorsX, 0, ActorsZ);
    ActorRef->SetActorLocation(FilterdLocation);
  }
}

AMaster_Pickup* UGeneralFunctions::IsPickupAtLocation(UObject* WorldContextObject, FVector Location)
{
  TArray<AActor*> FoundActors;

  UGameplayStatics::GetAllActorsOfClass(WorldContextObject->GetWorld(), AMaster_Pickup::StaticClass(), FoundActors);

  AMaster_Pickup* LocalPickup = nullptr;

  for (AActor* CurrentActor : FoundActors)
  {
    if (CurrentActor)
    {
      if (CurrentActor->GetActorLocation() == Location)
      {
        AMaster_Pickup* PickupToSet = Cast<AMaster_Pickup>(CurrentActor);
        LocalPickup = PickupToSet;
        break;
      }
      else
      {
        LocalPickup = nullptr;
        continue;
      }
    }
  }
  return LocalPickup;
}

AMaster_Pickup* UGeneralFunctions::DoesPickupExistInWorld(UObject* WorldContextObject, AMaster_Pickup* Pickup)
{
  TArray<AActor*> FoundActors;

  UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AMaster_Pickup::StaticClass(), FoundActors);

  AMaster_Pickup* LocalPickup = nullptr;

  for (AActor* CurrentActor : FoundActors)
  {
    if (CurrentActor)
    {
      AMaster_Pickup* CurrentPickup = Cast<AMaster_Pickup>(CurrentActor);

      if (CurrentPickup)
      {
        if (CurrentPickup->GetUniqueID() == Pickup->GetUniqueID())
        {
          LocalPickup = CurrentPickup;
          break;
        }
      }
    }
  }
  return LocalPickup;
}

FVector UGeneralFunctions::GetUnitVector(FVector From, FVector To)
{
  return (To - From).GetSafeNormal();
}

bool UGeneralFunctions::AreEneimesInLevel(UObject* WorldContextObject, int32 EnemyTolerance)
{
  TArray<AActor*> FoundActors;

  UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AMaster_Enemy::StaticClass(), FoundActors);

  int32 EnemyCounter = 0;

  for (AActor* Actor : FoundActors)
  {
    if (Actor)
    {
      AMaster_Enemy* Enemy = Cast<AMaster_Enemy>(Actor);

      if (Enemy)
      {
        if (!Enemy->GetIsDead())
        {
          EnemyCounter++;
        }
      }
    }
  }

  if (EnemyCounter > EnemyTolerance)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void UGeneralFunctions::CheckPlayerCooldowns(UObject* WorldContextObject)
{
  APaperWarden* Player = UGeneralFunctions::GetPlayer(WorldContextObject);

  if (Player)
  {
    Player->CheckSpellCooldowns();
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Error, TEXT("CheckPlayerCooldowns failed Player was not valid"))
  }
}

AWardenCameraManager* UGeneralFunctions::GetWardenCameraManager(UObject* WorldContextObject)
{
  APlayerCameraManager* CameraManager = WorldContextObject->GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

  if (CameraManager)
  {
    AWardenCameraManager* WardenCameraManager = Cast<AWardenCameraManager>(CameraManager);

    if (WardenCameraManager)
    {
      return WardenCameraManager;
    }
    else
    {
      UE_LOG(LogGeneralFunctions, Error, TEXT("Failed to GetWardenCameraManager cast to WardenCameraManager failed"))
      return nullptr;
    }
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Error, TEXT("Failed to GetWardenCameraManager was unable to get CameraManager"))
    return nullptr;
  }
}

APaperWarden* UGeneralFunctions::GetPlayer(UObject* WorldContextObject)
{
  ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);

  if (Character)
  {
    APaperWarden* Player = Cast<APaperWarden>(Character);
    
    if (Player)
    {
      return Player;
    }
    else
    {
      UE_LOG(LogGeneralFunctions, Error, TEXT("Cast to Character failed was unable to get player"))
      return nullptr;
    }
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Error, TEXT("Was unable to get player from UGameplayStatics::GetPlayerCharacter"))
    return nullptr;
  }
}

FRotator UGeneralFunctions::GetMouseRotation(UObject* WorldContextObject)
{
  APaperWarden* PlayerRef = GetPlayer(WorldContextObject);

  if (PlayerRef)
  {
    PlayerRef->UpdateMouseRotation();

    return PlayerRef->GetCurrentMouseRotation();
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Error, TEXT("Unable to GetMouseRotation to get unable to get player"))
    return FRotator(0);
  }
}

bool UGeneralFunctions::DamageHitActor(AActor* HitActor, float Damage, AActor* Instigator, bool bShowDamageText)
{
  if (HitActor)
  {
    AMaster_Enemy* HitEnemy = Cast<AMaster_Enemy>(HitActor);
    ALeechInnerEggBase* HitEgg = Cast<ALeechInnerEggBase>(HitActor);
    AMasterDamageEffect* HitEffect = Cast<AMasterDamageEffect>(HitActor);

    if (HitEnemy)
    {
      HitEnemy->DamageEnemy(Damage, bShowDamageText, Instigator);
      return true;
    }
    else if (HitEgg)
    {
      HitEgg->SpawnActor();
      return true;
    }
    else if (HitEffect)
    {
      HitEffect->DamageActor(Damage, bShowDamageText, Instigator);
      return false;
    }
    else
    {
      return false;
    }
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Error, TEXT("Unable to DamageHitActor HitActor was not valid"))
    return false;
  }
}

void UGeneralFunctions::LaunchCharacterAwayFromActor(APaperCharacter* CharacterToLaunch, AActor* ActorToLaunchAwayfrom, float LaunchVelocityMultipler)
{
  if (CharacterToLaunch)
  {
    if (ActorToLaunchAwayfrom)
    {
      FVector CharacterLocation = CharacterToLaunch->GetActorLocation();
      FVector ActorLocation = ActorToLaunchAwayfrom->GetActorLocation();

      FVector LaunchVelocity = GetUnitVector(ActorLocation, CharacterLocation) * LaunchVelocityMultipler;

      CharacterToLaunch->LaunchCharacter(LaunchVelocity, false, false);
    }
    else
    {
      UE_LOG(LogGeneralFunctions, Error, TEXT("Unable to LaunchCharacterAwayFromActor ActorToLaunchAwayfrom was not valid"))
    }
  }
  else
  {
    UE_LOG(LogGeneralFunctions, Error, TEXT("Unable to LaunchCharacterAwayFromActor CharacterToLaunch was not valid"))
  }
}

