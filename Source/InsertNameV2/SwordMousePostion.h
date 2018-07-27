// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SwordMousePostion.generated.h"

/**
 * 
 */
UCLASS()
class INSERTNAMEV2_API USwordMousePostion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
private:

	UFUNCTION(BlueprintPure, meta = (DisplayName = "SwordMousePosition", CompactNodeTitle = "SwordMousePosition", Keywords = "Sword"), Category = Game)
		static FString SwordMousePosition();

	
	
};
