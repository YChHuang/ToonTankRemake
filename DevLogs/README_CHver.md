# ToonTank威力加強版
> 基於 UE5 的戰車戰鬥系統，從 Tutorial(Learn C++ from scratch while making 4 fun games in Unreal Engine) 在跟完教學之後並進行多次架構重構與功能擴充

## 🎮 專案展示
![Demo](../ProjectScreenShots/GameplayShot.png)

*圖 1：封面*

![Demo2](../ProjectScreenShots/SlideAndAlignOnSlope.gif)

*圖 2：Tank在斜坡上滑行的畫面*

![Demo3](../ProjectScreenShots/SpawnSys_AiCon_MovementComp.gif)

*圖 3：敵人生成、AiController與MovememtComponent*

## 📋 專案簡介
這是一個學習導向的專案，主要是為了學習UE5的各種功能，從 Tutorial 完成後(原先只有npc邏輯和Gamemode)，逐步加入以下功能(透過詢問AI或查詢官方文件)：
- 輸入系統重構（Enhanced Input System）
- 重構耦合邏輯，分離出controller和Pawn 並且 AI 與玩家共用控制邏輯(注入數值)
- 敵人生成與波次系統(重構GameMode勝利條件與計數)

**開發時間**：2025/07 - 持續開發中  
**UE 版本**：5.5

## 📖 開發日誌
詳細的技術決策、問題排查與學習筆記請見 [DEVLOG.md](DevLog_CHver.md)

## 🛠 技術實作

### 1. Enhanced Input System 整合
- 從舊版 Input 重構為 Enhanced Input
- 重構與處理 AI Controller 與 Player Controller 的輸入接口
- AI Controller是根據砲塔類敵人的基類實作，故可交由Tank/Tower共用

### 2. 生成敵人系統
- 使用 Multicast Delegate + 工廠模式，簡化依賴關係
- 動態掃描地面高度避免碰撞

### 3. 物理移動系統
- 自訂 Movement Component 處理爬坡與AI導航邏輯，玩家與AI可共用
- 使用UNavMovementComponent，並且模擬UPawnMovementComponent的輸入邏輯
- 利用向量運算，實現 Pitch Align Slope（車身隨坡度旋轉）
- 解決 Spring Arm 相對/世界座標轉換問題

### 4. Gameplay Ability System (GAS)
- 整合 ASC、AttributeSet、Gameplay Ability
- 實作雷射武器（Line Trace），TODO:將與原本的projectile結合，可以切換
- Cost 系統設定（彈藥消耗）

## 🛠️ 技術棧
- **引擎**：Unreal Engine 5.5
- **語言**：C++ + Blueprint
- **核心系統**：
  - Enhanced Input System
  - Gameplay Ability System(僅照著官方文件部署成功)
  - Custom Movement Component
  - AI Controller (已部屬) + Behavior Tree (規劃學習中)

## 🎯 學習成果
-  理解 UE5 的 Delegate 機制與解耦合設計
-  掌握 GAS 基本流程與整合
-  實踐重構與架構改進（從單一責任到模組化）
-  養成寫開發日誌的習慣
-  向量運算的應用

## 🚧 已知問題與改進方向
- Health Component 尚未遷移到 GAS AttributeSet
- 武器系統計畫改成切換系統
- AI 決策邏輯待擴充（Behavior Tree）
- ~~爬坡時SpringArm會亂轉~~
- ~~Tank會怪異轉向世界X軸正向~~
- AI尋路偶爾會出現螃蟹走路
- Spawn偶而會疊羅漢



**控制方式**：
- MOUSE&KEYBOARD
- WASD：移動
- 滑鼠：瞄準/轉動砲塔
- 左鍵：射擊
---

## 💡 開發心得
這是我從零開始的 UE5 學習專案，在完成官方教學後持續擴充功能。
過程中經歷多次重構，從緊耦合的程式碼逐步改為模組化設計，
深刻體會到架構設計對專案維護的重要性。

**最大收穫**：學會透過 Delegate、Interface 等機制實現解耦合設計。

**持續更新中** | 歡迎交流與建議
