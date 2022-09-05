// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoorWithLerp.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AOpenDoorWithLerp::AOpenDoorWithLerp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    Open = false;
    // 将UBoxComponent 设置为 RootComponent 。然后，将重叠函数连接到 UBoxComponent
    MyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
    MyBoxComponent->InitBoxExtent(FVector(50, 50, 50));
    RootComponent = MyBoxComponent;

    Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Mesh"));
    // 相对父组件的位置  这个导致没开始游戏的时候门位置是歪的
    //Door->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    Door->SetRelativeLocation(FVector(0.0f, 50.0f, -50.0f));
    Door->SetupAttachment(RootComponent);

    MyBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AOpenDoorWithLerp::OnOverlapBegin);
    MyBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AOpenDoorWithLerp::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AOpenDoorWithLerp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOpenDoorWithLerp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    // 用函数才能访问 直接访问这个成员是 私有的
    DoorRotation = Door->GetRelativeRotation();
    DoorLocation = Door->GetRelativeLocation();
    // 平滑移动
    if (Open)
    {
        Door->SetRelativeRotation(FMath::Lerp(FQuat(DoorRotation), FQuat(FRotator(0.0f, RotateValue, 0.0f)), 0.01f));
        // 45是门的半径
        Door->SetRelativeLocation(FMath::Lerp(FVector(DoorLocation), FVector(FVector(45, 45, 0)), 0.01));
    }
    else
    {
        Door->SetRelativeRotation(FMath::Lerp(FQuat(DoorRotation), FQuat(FRotator(0.0f, 0.0f, 0.0f)), 0.01f));
        Door->SetRelativeLocation(FMath::Lerp(FVector(DoorLocation), FVector(FVector(0, 0, 0)), 0.01));
    }

}

// 创建重叠函数
// OnOverlapBegin 将首先对空值进行条件检查，以确定函数是否应该继续。
// 该函数根据玩家和 actor 的位置和旋转来检查他们所面对的方向。
// 玩家(Pawn) 是传递给函数的 OtherActor 参数
void AOpenDoorWithLerp::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
    {
        FVector PawnLocation = OtherActor->GetActorLocation();
        // Actor 的位置减去Pawn 的位置，得到一个方向 FVector
        FVector Direction = GetActorLocation() - PawnLocation;
        // 考虑父组件的旋转
        Direction = UKismetMathLibrary::LessLess_VectorRotator(Direction, GetActorRotation());
        // 如果玩家在门前，则 RotateValue 将等于 -90.0f
        if (Direction.X > 0.0f)
        {
            RotateValue = -90.0f;
        }
        else // 如果不是，则 RotateValue 将等于 -90.0f 
        {
            RotateValue = -90.0f;
        }

        Open = true;
    }
}

void AOpenDoorWithLerp::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
    {
        Open = false;
    }
}

