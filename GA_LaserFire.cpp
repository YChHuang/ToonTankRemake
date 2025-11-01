// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_LaserFire.h"
#include "BasePawn.h"
#include "Kismet/GameplayStatics.h"


UGA_LaserFire::UGA_LaserFire()
{
    // 設定實例化策略
    // InstancedPerActor = 每個使用這個技能的 Actor 都有自己的實例
    // 理由:如果技能有狀態(如計數器、計時器),不同 Actor 的狀態不會互相干擾
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_LaserFire::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    // 印出 Log 確認技能被觸發
    UE_LOG(LogTemp, Warning, TEXT("=== LaserFire Ability Activated! ==="));

    // 取得施放者資訊
    AActor* Avatar = ActorInfo->AvatarActor.Get();
    if (Avatar)
    {
        UE_LOG(LogTemp, Warning, TEXT("Fired by: %s"), *Avatar->GetName());


        FVector Start = Avatar->GetActorLocation() + (Avatar->GetActorForwardVector() * 200.f);
        FVector End = Avatar->GetActorForwardVector();

        /*Get turret ForwardVector*/
        
        ABasePawn* BasePawn = Cast<ABasePawn>(Avatar);
        
        if (BasePawn)
        {
            USceneComponent* ProjectileSpawnPoint = BasePawn->GetProjectileSpawnPoint();
            if (ProjectileSpawnPoint)
            {
                FVector Forward = ProjectileSpawnPoint->GetForwardVector();
                Start = ProjectileSpawnPoint->GetComponentLocation() + Forward * 200.f;
                End = Start + (Forward * 2000.f);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Can not get the forwardVector from turret"));
            
        }
        /*Get turret ForwardVector*/

        FHitResult Hit;
        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
        {

            UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *Hit.GetActor()->GetName());
            DrawDebugLine(
                GetWorld(),
                Start,
                End,
                FColor::Red,
                false,         // bPersistentLines
                2.0f,          // LifeTime
                0,             // DepthPriority
                2.0f           // Thickness
            );

            UClass* DamageTypeClass = UDamageType::StaticClass();
            UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, Avatar->GetInstigatorController(), Avatar, DamageTypeClass);
        }
    }

    // 結束技能
    // 理由:如果不呼叫 EndAbility,GAS 會認為技能還在執行中
    //      會導致技能無法再次觸發、記憶體洩漏等問題
    // 參數說明:
    // - Handle: 這個技能實例的識別
    // - ActorInfo: 施放者資訊
    // - ActivationInfo: 啟動資訊
    // - bReplicateEndAbility (true): 同步結束狀態到客戶端
    // - bWasCancelled (false): 技能是正常結束,不是被取消
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}