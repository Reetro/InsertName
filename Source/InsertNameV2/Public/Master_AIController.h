// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Master_AIController.generated.h"

/**
 * 
 */
UCLASS()
class INSERTNAMEV2_API AMaster_AIController : public AAIController
{
	GENERATED_BODY()

protected:

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
};
