// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h"

UWarriorHeroAnimInstance* UWarriorHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
	return Cast<UWarriorHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
