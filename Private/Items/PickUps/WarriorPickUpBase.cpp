// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickUps/WarriorPickUpBase.h"

// Sets default values
AWarriorPickUpBase::AWarriorPickUpBase()
{
	PrimaryActorTick.bCanEverTick = false;

	PickUpCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollisionSphere"));
	SetRootComponent(PickUpCollisionSphere);
	PickUpCollisionSphere->InitSphereRadius(50.f);
	PickUpCollisionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWarriorPickUpBase::OnPickUpCollisionSphereBeginOverlap);

}

void AWarriorPickUpBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	
}



