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
