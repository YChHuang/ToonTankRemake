// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_LaserFire.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API UGA_LaserFire : public UGameplayAbility
{
	GENERATED_BODY()

public:
    UGA_LaserFire();

    // 技能被觸發時,GAS 會自動呼叫這個函數
    // 這是 virtual function,我們覆寫它來定義「技能做什麼」
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "GAS")
    TSubclassOf<UGameplayEffect> ConsumeAmmoEffect;



private:
    UPROPERTY(EditAnywhere, Category = "GAS")
    float Damage = 50.f;
};
