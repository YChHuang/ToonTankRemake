// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" 
#include "GameplayEffectExtension.h" 
#include "ABS_Tank.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)



/**
 * 
 */
UCLASS()
class TOONTANKS_API UABS_Tank : public UAttributeSet
{
	GENERATED_BODY()


public:
    UABS_Tank();
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Health;

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Ammo;

    ATTRIBUTE_ACCESSORS(UABS_Tank, Health)
    ATTRIBUTE_ACCESSORS(UABS_Tank, Ammo)

	
};
