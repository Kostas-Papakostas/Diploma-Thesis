// Fill out your copyright notice in the Description page of Project Settings.

#include "Ribbon.h"
#include "CoreMinimal.h"
#include "GBrush_Cpp.h"


// Sets default values
ARibbon::ARibbon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;


	spherePtr = CreateDefaultSubobject<USphereComponent>(TEXT("sphere"));
	spherePtr->InitSphereRadius(2.f);
	spherePtr->SetupAttachment(RootComponent);
	//spherePtr->RegisterComponent();


	ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("/Game/Assets/RibbonToPaint.RibbonToPaint"));

	if (PS.Succeeded()) {
		ribbon = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ribbon"));
		//	ribbon->RegisterComponent();
		ribbon->SetTemplate(PS.Object);
		ribbon->InitializeSystem();
		//	ribbon->RegisterComponent();
		ribbon->SetupAttachment(spherePtr);
	}
}

// Called when the game starts or when spawned
void ARibbon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARibbon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

