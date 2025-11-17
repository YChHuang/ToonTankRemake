# 開發日誌

## 日期: 2025-07-01

- 決策: 引入EnhancedInputSystem
- 原因:
  1. 舊版本Input略為過時
  2. 更容易維護與資源更多
  3. 更容易讓AIcontroller接手
- 遇到的問題：
  1. 時常崩潰。
     解決方法：經過多次排查後，發現是對nullptr解引用造成的
  2. IA, IMC, 綁定函式簽名混亂
     解決方法：重做一遍熟悉。
- 學習筆記：
  1. 解指標前最好檢查null
  2. IMC管理，IA負責方法抽象，實作方法統一由FInputActionValue& inValue get要的型別(bool, float, vector)

## 日期: 2025-09-12

- 決策: 設計生成與波次系統
- 原因:
  1. 想測試實際操作生成系統
  2. 使遊戲可以更多樣
- 遇到的問題：
  1. 無法定位到地板，生成時出現碰撞UE自動中斷生成
     解決方法：用sweap掃描地面高度
  2. 使用UE_LOG測試多次高度後，仍被UE主動中斷生成
     解決方法：先用hardcode測試實際能生成的高度後，發現大概是差了AActor的高度
  3. 無法動態取得欲生成的AActor高度
     暫時解法：生成dummy取得高度後刪除dummy，問題很多且效能很差
     目前解法：使用map預先存好測試的高度(HACK)
  4. 耦合度很高：
     目前解法：用多播委託與工廠模式取指標。
- 學習筆記：
  1. 初次設計架構翻車(雖然會動，但根本無法維護)

## 日期: 2025-09-16

- 決策: 將邏輯抽象到AIController
- 原因:
  1. 增加更多彈性
  2. 使玩家有機會控制NPC
  3. 未來可以增加決策樹或狀態機，更容易擴充
- 遇到的問題:
  1. 原本NPC砲塔轉向的邏輯是隨時取得玩家座標，並直接向量座標，抽象成接收input就變成要取得一個float，
     解決方式是：用內建的ATAN2計算偏移量
  2. 砲塔轉向會overshooting，解決方式是：設定Threshold

## 日期: 2025-09-23

- 決策: 使用 Multicast Delegate/工廠模式生成
- 原因:
  1. 09-12的版本過多雙向依賴，捲成義大利麵了，自己放個假也看不懂了
  2. 解耦合 (SpawnManager 不需要知道 GameMode)
  3. 擴充性 (未來可能有 UI、音效等監聽者)
  4. 符合 UE5 慣例
- 學習筆記：
  1. 閱讀SOLID原則

## 日期: 2025-10-03

- 決策: 在TankPawn上改動移動邏輯，交給MovementComponent
- 原因:
  1. 原本的向量移動不能上下坡
  2. 原本的Pawn承擔太多邏輯
  3. 加入墜落更容易寫
  4. 未來可加入Pitch Align Slope
- 遇到的問題:
  1. 攝影機會在上坡時變得很怪異，原因是SpringArm綁在某個MeshComponent，應綁在root，
     並將攝影機移動邏輯寫在Tick上，只接收input的yaw。
  2. 爬上坡後會飄起來，原因是沒有任何可以墜落的邏輯。
  3. TODO-上坡時不會改變pitch
- 學習筆記：
  1. 盡量不要在一個class內寫太多邏輯
  2. 載具類有更好的基類chaos vehicle
  3. 大多數的component應該綁在root上，比免過度依賴

## 日期: 2025-10-27

- 決策: 重寫註解
- 原因:
  1. 偷懶沒寫註解，導致自己看不懂自己寫了什麼
  2. 架構稍微複雜，每次都浪費時間重看過去的自己拉的義大利麵
- 日誌：
  開始寫註解與補寫日誌、README
- 學習筆記：
  1. 專案要讓未來的自己與別人看得懂

## 日期: 2025-11-01

- 決策: 增加GAS
- 日誌：
- 1. 增加ASC, ABS 和一個GA_LaserFire
  2. 在Tank中套用ASC虛擬函數複寫
  3. 在ASC綁定GA_LaserFire
  4. 在GA_LaserFire中寫好射線邏輯
  5. 推上新分枝
- 遇到的問題：
  1. 實作了用LineTracing的雷射武器，結果會打到自己
     解決方法：想起當時在實作Tutorial時，有一個SceneComponent是PorjectileSpawnPoint，用他當起點，寫一個getter解決。
  2. 目前有自己的heathComponent
     解決方法：未來再改成GAS的，目前擱置
  3. 將技能寫死在Pawn上
     解決方法：再構建一個武器插槽與拾取系統
- 學習筆記：
  1. AbilitySystemComponent作為GAS的入口
  2. AttributeSet存GAS的參數(例如：Health, Ammo)
  3. 直接在Actor內掛上ASC和ABS，就可以用了
  4. 記得在Build.cs註冊GAS
  5. GameplayAbility作為技能實作其中：
     1. Handle負責識別激活實例
     2. ActorInfo則是施放者的資訊
     3. AActor* Avatar = ActorInfo->AvatarActor.Get(); 可以取得施放者的指標
     4. 結束時呼叫EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
  6. GameplayAbility最好建立BP子類，方便注入指標
  7. 建立一個繼承自GameEffect的BP，在裡面指定AttributeSet.Attribute
  8. 在BP的GA內，找到cost標籤，並指向步驟7.GE，這樣就完成消耗設定了

## 日期: 2025-11-07

- 決策: 重寫RotateSpringArm
- 原因:
  1. 時好時壞，有時候會亂轉
  2. 錯用世界座標與相對座標做轉換
     ```
     float CurrentYaw = SpringArmComp->GetRelativeRotation().Yaw
     
     float TargetYaw = TurretMesh->GetComponentRotation().Yaw;
     ```
- 日誌：
  嘗試都修改為GetComponentRotation()
  
  新問題是用FMath::FInterpTo 會在轉到底時出現大旋轉
  
  先用FMath::FindDeltaAngleDegrees解決

## 日期: 2025-11-09

- 決策: 製作RotationAlignSlope邏輯
- 日誌：
  1. 邏輯理論上能運作，但pitch始終沒跟著動，當時沒搞懂為何
  2. 後來發現是SlideAlongSurface會把hit洗掉
  3. 但整個綁定樹和旋轉砲塔實作的效果會很怪
  4. 把SetUsingAbsoluteLocation/Rotation設定成false即可
  5. 旋轉要改成AddRelativeRotation
  6. 仍有許多未知bug
  
## 日期: 2025-11-12

- 決策: 在MovementCoponent內完善RotationAlignSlope邏輯與修正錯誤
- 日誌：
  1. 某次微調改了turn的邏輯，原本是動整個Actor，改成了改動底盤的Mesh，讓砲塔始終指著玩家的畫面中心
  2. 雖然1.讓操控坦克的手感變得很好，但爬坡邏輯壞掉了
  3. 無論如何只能對著世界x軸朝向的斜坡操作，此外出現孤輪時，會強制轉到世界x軸的方向
  4. 查了半天才發現，Actor的rotator始終保持在(0, 0, 0)
  5. 先捨棄掉那個很酷的turn，邏輯就完善了
  6. 排查中還發現，我在tickcomponent內宣告不少初始變數，一直反覆宣告，所以在MovementComponent內也複寫一個beginplay
  7. ~~排查中順便把旋轉邏輯改成四元數~~
  8. ~~TODO : 在不讓邏輯出現怪異問題時，變得像1.~~
  

## 日期: 2025-11-16

- 決策: 封裝MovementComponent
- 日誌：
  1. 爬坡功能已開發完成，將邏輯封裝增加可讀性
  2. 分支合併到main

- 決策: 幫坦克增加尋路邏輯
- 日誌：
  1. 先前幫砲塔做的AI控制器可以直接套給坦克，是先前遵守多型開發的意外成果，幾乎不用重寫邏輯，用cast提供邏輯入口後直接寫尋路邏輯
  2. 先嘗試做了直線找到玩家的邏輯，但實在太詭異
  3. 而後做了MoveToActor，得先部署NavMeshBoundsVolume，框住範圍就好，但UPawnMovementComponent會讓這方法實作變成瞬間移動，非常詭異
  4. 要使用UE的A*，必須得用UNavMovementComponent，導致原本繼承自UPawnMovementComponent的邏輯失效
  5. 重寫MovementComponent，將繼承對象改成UNavMovementComponent，模擬PawnMovementComponent原來的介面與接口(如下)，
     以及必須把PawnOwner找回，模擬後依賴幾乎不辨，包含外部控制器輸入端注入數值
    ```
    	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|PawnMovement")
      void AddInputVector(FVector WorldVector, bool bForce = false);

      UFUNCTION(BlueprintCallable, Category = "Pawn|Components|PawnMovement")
      FVector ConsumeInputVector();

      UFUNCTION(BlueprintCallable, Category = "Pawn|Components|PawnMovement")
      FVector GetPendingInputVector() const { return PendingInputVector; }
    ```
  6. 測試完成後，已經會巡路追殺玩家了，但發現會螃蟹走路
  7. 所以先安排旋轉到玩家方向，不是最佳解，仍有機率看到螃蟹走路，但數量顯著減少
