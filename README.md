# UE5porject
ue5
C++（熟悉語法基礎、STL 容器與演算法、記憶體管理與 OOP；具 UE4/UE5 專案開發經驗）
C++：
- 熟悉語法基礎、STL 容器與演算法（vector, map, set, priority_queue 等）
- 理解記憶體管理（stack/heap、new/delete、智能指標）、RAII 與物件生命週期
- 掌握 OOP 與多型（virtual/override）、函式多載、基本模板應用
- 能獨立以 C++ 在 UE5 中實作遊戲功能（波次生成系統、遊戲結束判定、事件與計時器系統）
- 熟悉 UE 容器（TArray, TMap）、反射系統（UCLASS, UPROPERTY, UFUNCTION）與藍圖互動
- 具跨領域背景，能結合 3D/美術知識進行資產掛接與流程優化


# UE5 Wave System Demo

## 專案簡介
一個使用 C++ 與 Unreal Engine 5 實作的波次生成系統，包含敵人生成、波次切換、遊戲結束判定等功能。

## 功能特色
- 波次生成與計時控制（使用 UE Timer）
- 敵人數量追蹤與事件廣播（Delegate）
- 與美術資產整合，支援藍圖與 C++ 互動
- 可擴充的波次設定（TArray 配置）

## 特色更新
- 增加Enhanced Input System
- 增加Spawn System
- 將原版許多功能抽象給AiController，避面過度耦合


## 開發痛點

**Enhanced Input System 改造炮塔旋轉邏輯**

**背景**  
原版炮塔瞄準系統在 `Tick` 中使用滑鼠游標掃描場景，取得目標座標後傳入 `RotateTurret()` 進行平滑旋轉。此設計依賴滑鼠輸入與 `HitResult` 計算，不支援搖桿操作，且對 AIController 擴展不友好。

**痛點**  
- 輸入來源耦合：函式簽名直接依賴世界座標，無法抽象成 Enhanced Input 的 `InputAction` 向量。  
- 操作模式限制：搖桿輸入為軸值，無法直接轉換成目標座標。  
- AI 擴展困難：AI 無游標概念。
- 當可以用controller輸入float後，另一個問題是，原本Enemy的邏輯是直接用玩家的座標直接套用RotateTurret()使得可以指向玩家，但此時邏輯被改成輸入float了
- 成功將所有邏輯寫入AIController，但RotateTurret()是輸入float，但一直overshooting導致砲塔一直轉圈
- 在AIController會需要取得砲塔的指標，當時沒有getter的概念，以為可以直接取得

**改動**  
- 將 `RotateTurret(FVector LookAtTarget)` 改為 `RotateTurret(const FInputActionValue& Value)`。  
- 移除目標座標計算，改為根據玩家輸入值計算 `DeltaYaw`，並引入 `DeltaSeconds` 控制旋轉速度，確保不同幀率下旋轉一致。
- 先把邏輯抽象給AIController，再用FindDeltaAngleDegrees，底層是用Atan2取得弧度並再轉成float
- 和線性插直一樣給Clamp即可，並動態調整兩端

**成果**  
此改動使炮塔旋轉邏輯同時支援滑鼠與搖桿輸入，並為 AIController 提供更通用的控制接口，降低後續擴展成本。



**Spawn & wave System**

**背景**  
原版無生成系統，依賴的是Editor上預先設置好的ActorPawn來處理遊戲規則邏輯，我想開發一個能依照波次生成，且可以選擇要生成的派生類，並與當前的GameMode規則配合，當前勝利邏輯為開場統計EnemyCount，並在EnemyDied時減少計數，當計數為0->GameOver。

**痛點**  

- 生成的Enemy若直接加入在EnemyCount上，很高的機會在敵人尚未完全生成，就觸發GaemOver
- 生成敵人會跑進地板，觸發生成失敗，利用Sweap掃地板生成，但又遇到角色本身高度的問題
- 統計生成時，因為可能失敗，計數出現困難
- 一切能運作後，因為過多的藍圖依賴注入，此外有許多的循環依賴，後續想要增加更多的spawner/EnemyClass 實在無法繼續擴充
- 原本的Enemey邏輯寫死在Tower這個class上，但我想讓玩家操作所有pawn分不清enemy
- 此時由於泛化邏輯，玩家的死亡邏輯與enemy衝突
- 寫出spaghetti


**改動**  
- 在加入RemainingWave後，暫時解決GameOver邏輯
- 在首次嘗試時，先生成一個DummyVersion，取抓它的CapsuleHeight，再把它刪掉，雖可行，但為後續埋下許多地雷且效能不佳，後續解方：硬編一個能BluePrint編輯的哈希表儲存<UClass, 高度>
- 在生成成功後，會回傳該class的指標，透過它可以判斷是否生成成功
- 重構邏輯，將依賴注入改成多播委託，並使用工廠生成spawnerManager，除了取得指標外，順便訂閱delegate，而spawnManager會從當前存在Editor的spawner尋找必要的資訊和生成範圍(這些可以透過BP設定，因此我保留在Editor)等
- 將Tower邏輯降維成基類，改用controller和Tag判斷是否為enemy，並在生成Actor時就加入標籤，以便gamemode統計
- 利用前面的tag判斷玩家，並且寫一個玩家專屬的方法，從原本基類的方法，override成NPCver和PlayerVer
- AI幫忙封裝






## 架構圖
![Wave System Architecture](docs/wave_system_architecture.png)

## 技術細節
- 語言：C++
- 引擎：Unreal Engine 5
- 使用 UE API：`GetWorldTimerManager`, `UCLASS`, `UPROPERTY`, `TArray`, `Delegate`
- 設計模式：事件驅動、模組化設計

## 專案截圖
![Gameplay Screenshot](docs/gameplay.png)

## 如何運行
1. Clone 專案
2. 在 UE5 中開啟 `.uproject`
3. 編譯並運行
