// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperWarden.h"
#include "Master_Pickup.h"
#include "PaperSpriteComponent.h"
#include "Engine.h"
#include "Master_Spell.h"
#include "GeneralFunctions.h"
#include "Components/InputComponent.h"
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
  ActionBarSlotsPerRow = 10;
}

void APaperWarden::BeginPlay()
{
  Super::BeginPlay();

  BarkInnerCollision->OnComponentBeginOverlap.AddDynamic(this, &APaperWarden::OnOverlapBegin);
  BarkOuterCollision->OnComponentBeginOverlap.AddDynamic(this, &APaperWarden::BeginOverlap);

  InventoryItems.Empty();
  InventoryItems.Init(0, AmountofInventorySlots);

  ActionBarItems.Empty();
  ActionBarItems.Init(0, ActionBarSlotsPerRow);

  AssignTestSpells();
}

void APaperWarden::AssignTestSpells()
{
  for (TSubclassOf<AMaster_Spell> SpellClass : TestSpells)
  {
    if (SpellClass)
    {
      auto SpellToAssign = GetWorld()->SpawnActor<AMaster_Spell>(SpellClass, FVector(0), FRotator(0));

      if (SpellToAssign)
      {
        AssignSpellToActionBar(SpellToAssign);
      }
    }
  }
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

bool APaperWarden::AddItem(AMaster_Pickup* ItemToAdd, int32 Amount, bool bDisplayItemObtained)
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

            if (bDisplayItemObtained)
            {
              UGeneralFunctions::DisplayItemObtainMessage(this, StackToAddTo, Amount);
            }

            UpdateInventory();
            return true;
          }
          else
          {
            if (Amount > ItemToAdd->MaxItemAmount)
            {
              int32 Index = FindEmptySlotInInventory();

              if (bFoundSlot)
              {
                int32 Overflow = Amount - ItemToAdd->MaxItemAmount;
                InventoryItems[Index] = ItemToAdd;
                ItemToAdd->bAddedToStack = false;

                if (bDisplayItemObtained)
                {
                  UGeneralFunctions::DisplayItemObtainMessage(this, ItemToAdd, Amount);
                }

                UpdateInventory();
                AddItem(ItemToAdd, Overflow, bDisplayItemObtained);
                return true;
              }
              else
              {
                return false;
              }
            }
            else
            {
              int32 Index = FindEmptySlotInInventory();

              if (bFoundSlot)
              {
                InventoryItems[Index] = ItemToAdd;
                ItemToAdd->bAddedToStack = false;

                if (bDisplayItemObtained)
                {
                  UGeneralFunctions::DisplayItemObtainMessage(this, ItemToAdd, Amount);
                }

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
          int32 Index = FindEmptySlotInInventory();

          if (bFoundSlot)
          {
            InventoryItems[Index] = ItemToAdd;
            ItemToAdd->bAddedToStack = false;

            if (bDisplayItemObtained)
            {
              UGeneralFunctions::DisplayItemObtainMessage(this, ItemToAdd, Amount);
            }

            UpdateInventory();

            if (Amount > 1)
            {
              Amount--;
              AddItem(ItemToAdd, Amount, bDisplayItemObtained);
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

bool APaperWarden::RemoveItemFromInventory(AMaster_Pickup* ItemToRemove, int32 Amount, bool bDestroyPickup)
{
  if (ItemToRemove)
  {
    AMaster_Pickup* LocalItem = FindItemByName(ItemToRemove, InventoryItems);

    if (LocalItem)
    {
      LocalItem->AmountAtIndex -= Amount;

      if (LocalItem->AmountAtIndex <= 0)
      {
        int32 Index = InventoryItems.Find(LocalItem);
        if (InventoryItems.IsValidIndex(Index))
        {
          InventoryItems[Index] = nullptr;
          
          if (bDestroyPickup)
          {
            ItemToRemove->DestroyPickup();
          }

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

bool APaperWarden::SplitItemStack(AMaster_Pickup* ItemToSplit, int32 Amount)
{
  if (ItemToSplit)
  {
    if (ItemToSplit->ItemInfo.bCanBeStacked && ItemToSplit->AmountAtIndex > Amount)
    {
      int32 EmptySlot = FindEmptySlotInInventory();
      AMaster_Pickup* LocalItem = FindItemByName(ItemToSplit, InventoryItems);

      if (bFoundSlot)
      {
        FTransform SpawnLocation = FTransform(FRotator(1), FVector(1), FVector(1));

        UClass* ItemClass = LocalItem->GetClass();

        AMaster_Pickup* SplitActor = GetWorld()->SpawnActor<AMaster_Pickup>(ItemClass, SpawnLocation);

        if (SplitActor)
        {
          int32 NewAmount = LocalItem->AmountAtIndex - Amount;
          LocalItem->AmountAtIndex = NewAmount;

          SplitActor->AmountAtIndex = Amount;
          SplitActor->ShowPickup(false);

          InventoryItems[EmptySlot] = SplitActor;
          UpdateInventory();
          return true;
        }
        else
        {
          UE_LOG(LogTemp, Error, TEXT("Couldn't split item SplitActor was not vaild"))
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Couldn't split item ItemToSplit was not vaild"))
    return false;
  }
}

bool APaperWarden::DropItemsOnActionBar(AMaster_Pickup* Pickup, int32 Index)
{
  if (!IsActionBarFull())
  {
    AMaster_Pickup* LocalPickup = FindItemByName(Pickup, InventoryItems);

    if (LocalPickup)
    {
      ActionBarItems[Index] = LocalPickup;
      UpdateActionBar();
      int32 InventoryIndex = InventoryItems.Find(LocalPickup);
      if (InventoryItems.IsValidIndex(InventoryIndex))
      {
        InventoryItems[InventoryIndex] = nullptr;
        UpdateInventory();
        return true;
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("Was unable to find Item in inventory to move"))
        return false;
      }
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't remove item from Inventory when moving item. Local Item was not vaild."))
      return false;
    }
  }
  else
  {
    return false;
  }
}

void APaperWarden::UpdateInventory()
{
  OnUpdateInventory.Broadcast(InventoryItems);
}

void APaperWarden::UpdateActionBar()
{
  OnUpdateActionBar.Broadcast(ActionBarItems);
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

bool APaperWarden::IsActionBarFull()
{
  return false;
}

AMaster_Pickup* APaperWarden::FindItemByName(AMaster_Pickup* ItemToFind, const TArray<AMaster_Pickup*> ArrayToUse)
{
  AMaster_Pickup* LocalItem = nullptr;

  for (AMaster_Pickup* Item : ArrayToUse)
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
        auto CurrentPickup = FindItemByName(Pickup, InventoryItems);

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

int32 APaperWarden::FindEmptySlotInInventory()
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
}

int32 APaperWarden::FindEmptySlotOnActionBar()
{
  int32 LocalIndex = 0;

  for (int32 Index = 0; Index < ActionBarItems.Num(); Index++)
  {
    AMaster_Pickup* CurrentIndex = Cast<AMaster_Pickup>(ActionBarItems[Index]);

    if (!CurrentIndex)
    {
      LocalIndex = Index;
      bFoundSlotOnActionbar = true;
      break;
    }
    else
    {
      LocalIndex = 0;
      bFoundSlotOnActionbar = false;
      continue;
    }
  }

  return LocalIndex;
}

bool APaperWarden::IsItemAtIndex(int32 Index, const TArray<AMaster_Pickup*> ArrayToUse)
{
  if (ArrayToUse.IsValidIndex(Index))
  {
    if (ArrayToUse[Index] != nullptr)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Could not find item at given Index. Index was not valid"))
    return false;
  }
}

void APaperWarden::SwapItemsInInventory(AMaster_Pickup* ItemOne, AMaster_Pickup* ItemTwo)
{
  if (ItemOne)
  {
    if (ItemTwo)
    {
      int32 Index1 = InventoryItems.Find(ItemOne);
      
      if (InventoryItems.IsValidIndex(Index1))
      {
        int32 Index2 = InventoryItems.Find(ItemTwo);

        if (InventoryItems.IsValidIndex(Index2))
        {

          if (InventoryItems[Index1]->GetClass() == InventoryItems[Index2]->GetClass() && InventoryItems[Index2]->AmountAtIndex < InventoryItems[Index1]->MaxItemAmount)
          {
            InventoryItems[Index1]->AmountAtIndex += InventoryItems[Index2]->AmountAtIndex;
            InventoryItems[Index2]->DestroyPickup();
            InventoryItems[Index2] = nullptr;
            UpdateInventory();
          }
          else
          {
            InventoryItems.Swap(Index1, Index2);
            UpdateInventory();
          }
        }
        else
        {
          UE_LOG(LogTemp, Error, TEXT("Couldn't swap items ItemTwo was not found in Inventory"))
        }
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("Couldn't swap items ItemOne was not found in Inventory"))
      }
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

bool APaperWarden::UpdateItemIndexInInventory(AMaster_Pickup* ItemToMove, int32 NewIndex)
{
  if (ItemToMove)
  {
    int32 OldIndex = InventoryItems.Find(ItemToMove);

    if (InventoryItems.IsValidIndex(OldIndex))
    {
      InventoryItems[NewIndex] = ItemToMove;
      InventoryItems[OldIndex] = nullptr;
      UpdateInventory();
      return true;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Failed to find item ItemIndex was not found"))
      return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Failed to find item IndexToMove was not vaild"))
    return false;
  }
}

bool APaperWarden::UpdateItemIndexOnActionbar(AMaster_Pickup* ItemToMove, int32 NewIndex)
{
  if (ItemToMove)
  {
    int32 OldIndex = ActionBarItems.Find(ItemToMove);

    if (ActionBarItems.IsValidIndex(OldIndex))
    {
      ActionBarItems[NewIndex] = ItemToMove;
      ActionBarItems[OldIndex] = nullptr;
      UpdateActionBar();
      return true;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Failed to find item ItemIndex was not found"))
        return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Failed to find item IndexToMove was not vaild"))
    return false;
  }
}

void APaperWarden::SwapItemsOnActionbar(AMaster_Pickup* ItemOne, AMaster_Pickup* ItemTwo)
{
  if (ItemOne)
  {
    if (ItemTwo)
    {
      int32 Index1 = ActionBarItems.Find(ItemOne);

      if (ActionBarItems.IsValidIndex(Index1))
      {
        int32 Index2 = ActionBarItems.Find(ItemTwo);

        if (ActionBarItems.IsValidIndex(Index2))
        {

          if (ActionBarItems[Index1]->GetClass() == ActionBarItems[Index2]->GetClass() && ActionBarItems[Index2]->AmountAtIndex < ActionBarItems[Index1]->MaxItemAmount)
          {
            ActionBarItems[Index1]->AmountAtIndex += ActionBarItems[Index2]->AmountAtIndex;
            ActionBarItems[Index2]->DestroyPickup();
            ActionBarItems[Index2] = nullptr;
            UpdateActionBar();
          }
          else
          {
            ActionBarItems.Swap(Index1, Index2);
            UpdateActionBar();
          }
        }
        else
        {
          UE_LOG(LogTemp, Error, TEXT("Couldn't swap items ItemTwo was not found on Actionbar"))
        }
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("Couldn't swap items ItemOne was not found in Actionbar"))
      }
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

void APaperWarden::UseItemOnActionbar(int32 ItemIndex)
{
  AMaster_Pickup* LocalIndex = ActionBarItems[ItemIndex];

  if (LocalIndex)
  {
    LocalIndex->UseItem();
  }
}

bool APaperWarden::RemoveItemFromActionbar(AMaster_Pickup* ItemToRemove, int32 Amount)
{
  if (ItemToRemove)
  {
    AMaster_Pickup* LocalItem = FindItemByName(ItemToRemove, ActionBarItems);

    if (LocalItem)
    {
      if (!LocalItem->ItemInfo.bIsSpell)
      {

        LocalItem->AmountAtIndex -= Amount;

        if (LocalItem->AmountAtIndex <= 0)
        {
          int32 Index = ActionBarItems.Find(LocalItem);
          if (ActionBarItems.IsValidIndex(Index))
          {
            ActionBarItems[Index] = nullptr;
            ItemToRemove->DestroyPickup();
            UpdateActionBar();
            return true;
          }
          else
          {
            UE_LOG(LogTemp, Error, TEXT("failed to find item on Actionbar not a vaild index"))
              return false;
          }
        }
        else
        {
          UpdateActionBar();
          return true;
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't remove item from Actionbar. LocalItem was not found."))
      return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Couldn't remove item from Actionbar. ItemToRemove was not vaild"))
    return false;
  }
}

bool APaperWarden::RemoveItemFromPlayer(AMaster_Pickup* ItemToRemove, int32 Amount)
{
  if (ItemToRemove)
  {
    int32 InventoryIndex = InventoryItems.Find(ItemToRemove);
    int32 ActionbarIndex = ActionBarItems.Find(ItemToRemove);

    if (InventoryItems.IsValidIndex(InventoryIndex))
    {
      RemoveItemFromInventory(InventoryItems[InventoryIndex], Amount, true);
      return true;
    }
    else if (ActionBarItems.IsValidIndex(ActionbarIndex))
    {
      RemoveItemFromActionbar(ActionBarItems[ActionbarIndex], Amount);
      return true;
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("Couldn't remove item from Player. ItemToRemove was not found on player"))
      return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Couldn't remove item from Player. ItemToRemove was not vaild"))
    return false;
  }
}

AMaster_Pickup* APaperWarden::FindIteminWorld(int32 ID)
{
  TArray<AActor*> FoundActors;

  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMaster_Pickup::StaticClass(), FoundActors);

  AMaster_Pickup* LocalPickup = nullptr;

  for (AActor* CurrentActor : FoundActors)
  {
    AMaster_Pickup* CurrentPickup = Cast<AMaster_Pickup>(CurrentActor);

    if (CurrentPickup)
    {
      if (CurrentPickup->GetID() == ID)
      {
        LocalPickup = CurrentPickup;
        break;
      }
      else
      {
        LocalPickup = nullptr;
        continue;
      }
    }
    else
    {
      LocalPickup = nullptr;
      continue;
    }
  }
  return LocalPickup;
}

bool APaperWarden::MoveActionbarItemsToInventory(AMaster_Pickup* ItemToMove)
{
  if (ItemToMove)
  {
    if (!ItemToMove->ItemInfo.bIsSpell)
    {
      AMaster_Pickup* LocalItem = FindItemByName(ItemToMove, ActionBarItems);

      if (LocalItem)
      {
        AddItem(LocalItem, LocalItem->AmountAtIndex, false);
        int32 ActionBarIndex = ActionBarItems.Find(LocalItem);
        if (ActionBarItems.IsValidIndex(ActionBarIndex))
        {
          ActionBarItems[ActionBarIndex] = nullptr;
          UpdateActionBar();
          return true;
        }
        else
        {
          UE_LOG(LogTemp, Error, TEXT("Failed to remove item from Actionbar ActionBarIndex was not valid"))
          return false;
        }
      }
      else
      {
        UE_LOG(LogTemp, Error, TEXT("Failed to move item to Inventory could not find item on Actionbar"))
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Failed to move item to Inventory ItemToMove was not valid"))
    return false;
  }
}

bool APaperWarden::SetArrayIndex(int32 Index, TArray<AMaster_Pickup*> ArrayToUse, AMaster_Pickup* ItemToSet)
{
  if (ItemToSet)
  {
    ArrayToUse[Index] = ItemToSet;
    return true;
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Failed to set Index ItemToSet was not valid"))
    return false;
  }
}

bool APaperWarden::DupCheck(AMaster_Pickup* ItemOne, AMaster_Pickup* ItemTwo)
{
  if (ItemOne)
  {
    if (ItemTwo)
    {
      if (ItemOne->GetID() == ItemTwo->GetID())
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      UE_LOG(LogTemp, Error, TEXT("DupCheck Failed ItemTwo was not valid"))
      return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("DupCheck Failed ItemOne was not valid"))
    return false;
  }
}

int32 APaperWarden::FindItemIndexByID(int32 ID, const TArray<AMaster_Pickup*> ArrayToUse)
{
  int32 OutIndex = 0;

  for (int32 Index = 0; Index < ArrayToUse.Num(); Index++)
  {
    int32 CurrentID = ArrayToUse[Index]->GetID();

    if (CurrentID == ID)
    {
      OutIndex = Index;
      break;
    }
    else
    {
      OutIndex = 0;
      continue;
    }
  }

  return OutIndex;
}

bool APaperWarden::AssignSpellToActionBar(AMaster_Spell* SpellToAdd)
{
  if (SpellToAdd)
  {
    int32 ActionIndex = FindEmptySlotOnActionBar();

    if (bFoundSlotOnActionbar)
    {
      ActionBarItems[ActionIndex] = SpellToAdd;
      UpdateActionBar();
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Couldn't add spell SpellToAdd was not valid"))
    return false;
  }
}

bool APaperWarden::AssignSpellsToActionBar(TArray<AMaster_Spell*> SpellsToAdd)
{
  bool LocalBool = false;

  for (AMaster_Spell* Spell : SpellsToAdd)
  {
    if (Spell)
    {
      if (AssignSpellToActionBar(Spell))
      {
        LocalBool = true;
      }
      else
      {
        LocalBool = false;
        break;
      }
    }
    else
    {
      LocalBool = false;
      break;
    }
  }
  if (!LocalBool)
  {
    UE_LOG(LogTemp, Error, TEXT("Couldn't add spell. Spell was not valid"))
    return false;
  }
  else
  {
    return true;
  }
}

const TArray<AMaster_Spell*> APaperWarden::GetPlayerSpells()
{
  return PlayerSpells;
}

const TArray<AMaster_Pickup*> APaperWarden::GetPlayerInventory()
{
  return InventoryItems;
}

const TArray<AMaster_Pickup*> APaperWarden::GetActionBarItems()
{
  return ActionBarItems;
}
