// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperWarden.h"
#include "Master_Pickup.h"
#include "PaperSpriteComponent.h"
#include "Engine.h"
#include "GeneralFunctions.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"

APaperWarden::APaperWarden()
{
  PrimaryActorTick.bCanEverTick = true;

  BarkInnerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Bark Inner Collision"));
  BarkInnerCollision->SetupAttachment(RootComponent);
  BarkOuterCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Bark Outer Collision"));
  BarkOuterCollision->SetupAttachment(RootComponent);

  AmountofInventorySlots = 8;
}

void APaperWarden::BeginPlay()
{
  Super::BeginPlay();

  BarkInnerCollision->OnComponentBeginOverlap.AddDynamic(this, &APaperWarden::OnOverlapBegin);
  BarkOuterCollision->OnComponentBeginOverlap.AddDynamic(this, &APaperWarden::BeginOverlap);

  InventoryItems.Empty();
  InventoryItems.Init(0, AmountofInventorySlots);
}

int32 APaperWarden::AddToKillCount(int32 AmountToadd)
{
  return KillCount += AmountToadd;
}

const int32 APaperWarden::GetCurrentKillCount()
{
  return KillCount;
}

void APaperWarden::LoadKillCount(int32 KillCountToLoad)
{
  KillCount = KillCountToLoad;
}

void APaperWarden::PickedUpItem_Implementation(class UTexture2D* PickUpTexture)
{
  UE_LOG(LogTemp, Warning, TEXT("Player Pickup event has no Implementation"))
}

void APaperWarden::HealPlayer_Implementation(float HealAmount)
{
  UE_LOG(LogTemp, Warning, TEXT("Player Heal event has no Implementation"))
}

void APaperWarden::KillPlayer_Implementation()
{
  UE_LOG(LogTemp, Warning, TEXT("Kill Player event has no Implementation"))
}

void APaperWarden::UpgradeHP_Implementation(float UpgradeAmount, float NewMaxHP)
{
  UE_LOG(LogTemp, Warning, TEXT("Upgrade HP event has no Implementation"))
}

void APaperWarden::Damage_Implementation(float DamageAmount, bool bShowCombatText, AActor* DamageInstigator)
{
  UE_LOG(LogTemp, Warning, TEXT("Damage event has no Implementation"))
}

void APaperWarden::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (OtherActor)
  {
    OnBarkInnerOverlap(OtherActor);
  }
}

void APaperWarden::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (OtherActor)
  {
    OnBarkOuterOverlap(OtherActor);
  }
}

bool APaperWarden::AddItem(AMaster_Pickup* ItemToAdd, int32 Amount)
{
  if (!IsInventoryFull())
  {
    if (ItemToAdd)
    {
      if (!ItemToAdd->ItemInfo.bIsSpell)
      {
        if (ItemToAdd->ItemInfo.bCanBeStacked)
        {
          AMaster_Pickup* StackToAddTo = SearchForFreeStack(ItemToAdd);

          if (StackToAddTo)
          {
            StackToAddTo->AddToStack();
            ItemToAdd->bAddedToStack = true;
            UpdateInventory();
            return true;
          }
          else
          {
            if (Amount > ItemToAdd->MaxItemAmount)
            {
              int32 Index = FindEmptySlot();

              if (bFoundSlot)
              {
                int32 Overflow = Amount - ItemToAdd->MaxItemAmount;
                InventoryItems[Index] = ItemToAdd;
                ItemToAdd->bAddedToStack = false;
                UpdateInventory();
                AddItem(ItemToAdd, Overflow);
                return true;
              }
              else
              {
                return false;
              }
            }
            else
            {
              int32 Index = FindEmptySlot();

              if (bFoundSlot)
              {
                InventoryItems[Index] = ItemToAdd;
                ItemToAdd->bAddedToStack = false;
                UpdateInventory();
                return true;
              }
              else
              {
                return false;
              }
            }
          }
        }
        else
        {
          int32 Index = FindEmptySlot();

          if (bFoundSlot)
          {
            InventoryItems[Index] = ItemToAdd;
            ItemToAdd->bAddedToStack = false;
            UpdateInventory();

            if (Amount > 1)
            {
              Amount--;
              AddItem(ItemToAdd, Amount);
              return true;
            }
            else
            {
              return true;
            }
          }
          else
          {
            return false;
          }
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("failed to add item. Item was not valid"))
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool APaperWarden::RemoveItemFromInventory(AMaster_Pickup* ItemToRemove, int32 Amount)
{
  if (ItemToRemove)
  {
    AMaster_Pickup* LocalItem = FindItemByName(ItemToRemove);

    if (LocalItem)
    {
      LocalItem->AmountAtIndex -= Amount;

      if (LocalItem->AmountAtIndex <= 0)
      {
        int32 Index = InventoryItems.Find(LocalItem);
        if (InventoryItems.IsValidIndex(Index))
        {
          InventoryItems[Index] = nullptr;
          UpdateInventory();
          return true;
        }
        else
        {
          UE_LOG(LogTemp, Error, TEXT("failed to find item in Inventory not a vaild index"))
          return false;
        }
      }
      else
      {
        UpdateInventory();
        return true;
      }
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("failed to find item in Inventory"))
      return false;
    }   
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Item to remove was not valid"))
    return false;
  }
}

void APaperWarden::DropItemsOnActionBar(class AMaster_Pickup* Pickup, int32 MaxItems)
{
  // See if hot bar is full
  if (ActionBarItems.Num() == MaxItems)
  {
    return;
  }
  else
  {
    InventoryItems.Remove(Pickup);
    ActionBarItems.Add(Pickup);

    UpdateInventory();
    OnUpdateActionBar.Broadcast(ActionBarItems);
  }
}

void APaperWarden::UpdateInventory()
{
  OnUpdateInventory.Broadcast(InventoryItems);
}

bool APaperWarden::IsInventoryFull()
{
  int32 LocalCounter = 0;

  for (AMaster_Pickup* Pickup : InventoryItems)
  {
    if (Pickup)
    {
      LocalCounter++;
    }
  }

  if (LocalCounter >= AmountofInventorySlots)
  {
    return true;
  }
  else
  {
    return false;
  }
}

AMaster_Pickup* APaperWarden::FindItemByName(AMaster_Pickup* ItemToFind)
{
  AMaster_Pickup* LocalItem = nullptr;

  for (AMaster_Pickup* Item : InventoryItems)
  {
    if (Item)
    {
      FString Name1 = Item->ConvertItemNameToString();
      FString Name2 = ItemToFind->ConvertItemNameToString();

      if (Name1 == Name2)
      {
        LocalItem = Item;
        break;
      }
      else
      {
        LocalItem = nullptr;
        continue;
      }
    }
  }
  return LocalItem;
}

AMaster_Pickup* APaperWarden::SearchForFreeStack(AMaster_Pickup* ItemClass)
{
  if (ItemClass)
  {
    AMaster_Pickup* LocalPickUp = nullptr;

    for (AMaster_Pickup* Pickup : InventoryItems)
    {
      if (Pickup)
      {
        auto CurrentPickup = FindItemByName(Pickup);

        if (CurrentPickup->GetClass() == ItemClass->GetClass())
        {
          if (CurrentPickup->AmountAtIndex < CurrentPickup->MaxItemAmount)
          {
            LocalPickUp = CurrentPickup;
            break;
          }
          else
          {
            LocalPickUp = nullptr;
            continue;
          }
        }
        else
        {
          LocalPickUp = nullptr;
          continue;
        }
      }
      else
      {
        LocalPickUp = nullptr;
        continue;
      }
    }
    return LocalPickUp;
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("SearchForFreeStack failed ItemClass was not vailded"))
    return false;
  }
}

int32 APaperWarden::FindEmptySlot()
{
  int32 LocalIndex = 0;

  for (int32 Index = 0; Index < InventoryItems.Num(); Index++)
  {
    AMaster_Pickup* CurrentIndex = Cast<AMaster_Pickup>(InventoryItems[Index]);

    if (!CurrentIndex)
    {
      LocalIndex = Index;
      bFoundSlot = true;
      break;
    }
    else
    {
      LocalIndex = 0;
      bFoundSlot = false;
      continue;
    }
  }

  return LocalIndex;
  UE_LOG(LogTemp, Log, TEXT("Empty Slot %i"), LocalIndex)
}

void APaperWarden::PrintInventory()
{
  FString sInventory = "";

  for (AMaster_Pickup* actor : InventoryItems)
  {
    sInventory.Append(actor->GetName());
    sInventory.Append(" | ");
  }

  GEngine->AddOnScreenDebugMessage(1, 3, FColor::Blue, *sInventory);
}

void APaperWarden::SwapItems(AMaster_Pickup* ItemOne, AMaster_Pickup* ItemTwo)
{
  if (ItemOne)
  {
    if (ItemTwo)
    {
      int32 Index1 = FindArrayIndex(ItemOne);
      int32 Index2 = FindArrayIndex(ItemTwo);
      InventoryItems.Swap(Index1, Index2);
      UpdateInventory();
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't swap items ItemTwo was not vaild"))
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Couldn't swap items ItemOne was not vaild"))
  }
}

int32 APaperWarden::FindArrayIndex(AMaster_Pickup* ItemToFind)
{
  if (ItemToFind)
  {
    AMaster_Pickup* LocalItem = FindItemByName(ItemToFind);

    if (LocalItem)
    {
      int32 LocalIndex = 0;

      for (int32 Index = 0; Index < InventoryItems.Num(); Index++)
      {
        AMaster_Pickup* CurrentIndex = InventoryItems[Index];

        if (CurrentIndex->ConvertItemNameToString() == LocalItem->ConvertItemNameToString())
        {
          LocalIndex = Index;
          break;
        }
        else
        {
          LocalIndex = 0;
          continue;
        }
      }
      return LocalIndex;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't Find item in inventory"))
      return 0;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Failed to find item ItemToFind was not vaild"))
    return 0;
  }
}

const TArray<AMaster_Pickup*> APaperWarden::GetPlayerInventory()
{
  return InventoryItems;
}

const TArray<AMaster_Pickup*> APaperWarden::GetActionBarItems()
{
  return ActionBarItems;
}

void APaperWarden::RestDropItemCollision()
{
  ItemToRest->PaperSprite->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
  ItemToRest->BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
