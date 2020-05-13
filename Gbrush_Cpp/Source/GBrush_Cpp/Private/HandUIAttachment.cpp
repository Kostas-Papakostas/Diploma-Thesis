// Fill out your copyright notice in the Description page of Project Settings.

#include "HandUIAttachment.h"


// Sets default values
AHandUIAttachment::AHandUIAttachment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	ConstructorHelpers::FClassFinder<UUserWidget> PS(TEXT("'/Game/UI/HandUI'"));
	
	UIHand = CreateDefaultSubobject<UWidgetComponent>(TEXT("UI"));

	UIHand->SetupAttachment(Scene);
	if (PS.Succeeded()) {
		UIHand->SetWidgetClass(PS.Class);
	}
}

// Called when the game starts or when spawned
void AHandUIAttachment::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHandUIAttachment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

