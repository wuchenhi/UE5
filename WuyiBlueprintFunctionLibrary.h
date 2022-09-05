// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <sstream>
#include <string>
#include "UTVRPointCloudHelper.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WuyiBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FTKITCHEN_API UWuyiBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	// 读取txt文件存为TArray，每行存为FString
	UFUNCTION(BlueprintCallable, Category = "HandleTXT")
		static TArray<FString> ReadTxtFromPath(FString TxtPath); 
	// 利用istream 将每行信息以 , 分割提取为FVector 存入TArray ，flag == 0 读取位置信息  flag == 1 读取速度信息
	UFUNCTION(BlueprintCallable, Category = "HandleTXT")
		static TArray<FVector> ChangeFStringToVec(TArray<FString> TxtArr, int flag);
	// 将数组存为 UTexture2D 纹理返回， 创建瞬态纹理版本，不保存
	UFUNCTION(BlueprintCallable, Category = "HandleTXT")
		static UTexture2D* CreateTextureFromArray(TArray<FVector> values);
    // 将数组存为 UTexture2D 纹理 并保存到本地名为name的纹理
	UFUNCTION(BlueprintCallable, Category = "HandleTXT")
		static void CreateTextureFromArrayNoreturn(TArray<FVector> values, const FString name);
};
