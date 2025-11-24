# Development Log

## Date: 2025-07-01

- Decision: Introduce Enhanced Input System  
- Reasons:  
  1. Legacy Input system is somewhat outdated  
  2. Easier to maintain with more resources available  
  3. Easier for AIController to take over  
- Issues Encountered:  
  1. Frequent crashes.  
     Solution: After multiple debugging sessions, found it was caused by dereferencing a nullptr.  
  2. IA, IMC, and binding function signatures were confusing.  
     Solution: Redid everything to become familiar again.  
- Learning Notes:  
  1. Always check for null before dereferencing pointers.  
  2. IMC handles management, IA abstracts methods, and implementations consistently use `FInputActionValue& inValue` to get the required type (bool, float, vector).  

## Date: 2025-09-12

- Decision: Design enemy spawning and wave system  
- Reasons:  
  1. Wanted to test practical spawning system implementation  
  2. Make the game more diverse  
- Issues Encountered:  
  1. Could not locate the floor; spawning caused collisions and UE automatically aborted.  
     Solution: Use sweep to scan ground height.  
  2. Even after testing multiple heights with UE_LOG, UE still aborted spawning.  
     Solution: Hardcoded test heights revealed the offset was roughly equal to the AActor’s height.  
  3. Could not dynamically obtain the height of the AActor to spawn.  
     Temporary solution: Spawn a dummy to get height then delete it (inefficient and problematic).  
     Current solution: Use a map to pre-store tested heights (HACK).  
  4. High coupling.  
     Current solution: Use multicast delegates and factory pattern to retrieve pointers.  
- Learning Notes:  
  1. First architecture design failed (it worked but was unmaintainable).  

## Date: 2025-09-16

- Decision: Abstract logic into AIController  
- Reasons:  
  1. Increase flexibility  
  2. Allow players to potentially control NPCs  
  3. Future expansion with decision trees or state machines  
- Issues Encountered:  
  1. Original NPC turret rotation logic directly fetched player coordinates and aimed vectors. Abstracting into input required a float.  
     Solution: Use built-in ATAN2 to calculate offset.  
  2. Turret rotation overshooting.  
     Solution: Set a threshold.  

## Date: 2025-09-23

- Decision: Use Multicast Delegate / Factory Pattern for spawning  
- Reasons:  
  1. The 09-12 version had too many bidirectional dependencies, turned into spaghetti code.  
  2. Decoupling (SpawnManager doesn’t need to know GameMode).  
  3. Extensibility (future listeners like UI, audio).  
  4. Align with UE5 conventions.  
- Learning Notes:  
  1. Read SOLID principles.  

## Date: 2025-10-03

- Decision: Move movement logic from TankPawn to MovementComponent  
- Reasons:  
  1. Original vector-based movement couldn’t handle slopes  
  2. Pawn carried too much logic  
  3. Easier to implement falling  
  4. Future support for Pitch Align Slope  
- Issues Encountered:  
  1. Camera behaved oddly on slopes because SpringArm was attached to a MeshComponent instead of root. Fixed by attaching to root and handling yaw in Tick.  
  2. Tank floated after climbing slopes due to missing fall logic.  
  3. TODO: Pitch not adjusting on slopes.  
- Learning Notes:  
  1. Avoid writing too much logic in a single class.  
  2. Chaos Vehicle provides a better base class for vehicles.  
  3. Most components should be attached to root to avoid over-dependency.  

## Date: 2025-10-27

- Decision: Rewrite comments  
- Reasons:  
  1. Skipped writing comments, later couldn’t understand my own code.  
  2. Architecture became complex, wasted time re-reading spaghetti code.  
- Log:  
  Started writing comments and updating logs/README.  
- Learning Notes:  
  1. Projects should be understandable to future self and others.  

## Date: 2025-11-01

- Decision: Add GAS  
- Log:  
  1. Added ASC, ABS, and GA_LaserFire  
  2. Applied ASC virtual function overrides in Tank  
  3. Bound GA_LaserFire to ASC  
  4. Implemented laser logic in GA_LaserFire  
  5. Pushed to new branch  
- Issues Encountered:  
  1. Laser weapon using LineTracing hit self.  
     Solution: Used ProjectileSpawnPoint SceneComponent from tutorial as origin, added getter.  
  2. Currently using custom HealthComponent.  
     Solution: Will migrate to GAS later, postponed for now.  
  3. Abilities hardcoded in Pawn.  
     Solution: Build weapon slot and pickup system.  
- Learning Notes:  
  1. AbilitySystemComponent is the entry point for GAS.  
  2. AttributeSet stores GAS parameters (Health, Ammo).  
  3. Attach ASC and ABS directly to Actor to use.  
  4. Register GAS in Build.cs.  
  5. GameplayAbility implementation:  
     - Handle identifies activation instance  
     - ActorInfo contains caster info  
     - `AActor* Avatar = ActorInfo->AvatarActor.Get();` retrieves caster pointer  
     - Call `EndAbility(Handle, ActorInfo, ActivationInfo, true, false);` to end ability  
  6. Create BP subclasses for GameplayAbility for easier pointer injection.  
  7. Create BP inheriting from GameEffect to specify AttributeSet.Attribute.  
  8. In BP GA, set cost tag pointing to step 7 GE to configure resource consumption.  

## Date: 2025-11-07

- Decision: Rewrite RotateSpringArm  
- Reasons:  
  1. Behavior inconsistent, sometimes rotated incorrectly  
  2. Misused world vs relative coordinates  
     ```cpp
     float CurrentYaw = SpringArmComp->GetRelativeRotation().Yaw;
     float TargetYaw = TurretMesh->GetComponentRotation().Yaw;
     ```  
- Log:  
  Changed to use GetComponentRotation().  
  New issue: FMath::FInterpTo caused large rotations at limits.  
  Solution: Use FMath::FindDeltaAngleDegrees.  

## Date: 2025-11-09

- Decision: Implement RotationAlignSlope logic  
- Log:  
  1. Logic theoretically worked, but pitch never updated.  
  2. Found SlideAlongSurface cleared hit results.  
  3. Binding tree and turret rotation produced odd effects.  
  4. Fixed by setting SetUsingAbsoluteLocation/Rotation to false.  
  5. Changed rotation to AddRelativeRotation.  
  6. Still many unknown bugs.  

## Date: 2025-11-12

- Decision: Refine RotationAlignSlope logic in MovementComponent and fix errors  
- Log:  
  1. Modified turn logic: moved chassis mesh instead of entire Actor, turret always aimed at player center.  
  2. Improved tank handling but broke slope logic.  
  3. Could only operate on slopes facing world X-axis; single-wheel cases forced rotation to world X-axis.  
  4. Found Actor rotator always stayed at (0,0,0).  
  5. Dropped the fancy turn logic, slope logic improved.  
  6. Found repeated variable declarations in TickComponent; added BeginPlay override in MovementComponent.  
  7. ~~Temporarily switched rotation logic to quaternions~~  
  8. ~~TODO: replicate earlier turn logic without breaking slope handling~~  

## Date: 2025-11-16

- Decision: Encapsulate MovementComponent  
- Log:  
  1. Slope climbing feature completed, encapsulated logic for readability.  
  2. Merged branch into main.  

- Decision: Add pathfinding logic to tank  
- Log:  
  1. AIController built for turrets reused for tanks thanks to polymorphic design, minimal rewrites needed. Cast provided entry point for pathfinding logic.  
  2. First attempt: straight-line pathfinding to player, too odd.  
  3. Next: MoveToActor with NavMeshBoundsVolume deployed. UPawnMovementComponent caused teleport-like movement.  
  4. To use UE’s A*, had to switch to UNavMovementComponent, breaking UPawnMovementComponent inheritance.  
  5. Rewrote MovementComponent to inherit from UNavMovementComponent, simulated PawnMovementComponent interface:  
     ```cpp
     UFUNCTION(BlueprintCallable, Category = "Pawn|Components|PawnMovement")
     void AddInputVector(FVector WorldVector, bool bForce = false);

     UFUNCTION(BlueprintCallable, Category = "Pawn|Components|PawnMovement")
     FVector ConsumeInputVector();

     UFUNCTION(BlueprintCallable, Category = "Pawn|Components|PawnMovement")
     FVector GetPendingInputVector() const { return PendingInputVector; }
     ```  
     Also restored PawnOwner, simulated dependencies including external controller input injection.  
  6. After testing, tank could pathfind and chase player, but exhibited crab-w
