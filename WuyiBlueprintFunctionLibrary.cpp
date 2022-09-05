// Fill out your copyright notice in the Description page of Project Settings.


#include "WuyiBlueprintFunctionLibrary.h"

TArray<FString>  UWuyiBlueprintFunctionLibrary::ReadTxtFromPath(FString TxtPath)
{
	TArray<FString> OutArray;
	FString TxtStream;//文本流
	// std::vector<std::vector<FString>> vec{};
	// 没用上
	int64 line_number = 0;
	TxtPath = FPaths::ProjectContentDir() + TxtPath;
	//查找文本是否存在
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*TxtPath))
	{
		FString str = TxtPath + "文件不存在！";
		UE_LOG(LogTemp, Warning, TEXT("%s,文件不存在！"), *str);
		return OutArray;
	}

	FFileHelper::LoadFileToStringArray(OutArray, *TxtPath);

	// 存一维数组OutArray中
	return OutArray;
}


TArray<FVector>  UWuyiBlueprintFunctionLibrary::ChangeFStringToVec(TArray<FString> TxtArr, int flag)
{
	TArray<FVector> ans;
	for (FString Txt : TxtArr) {
		FVector vec{};  // 位置数组
		std::string MyString(TCHAR_TO_UTF8(*Txt));
		std::istringstream isS(MyString);
		std::string tmp;
		int index = 0;
		while (std::getline(isS, tmp, ',')) { // ，分隔
			//FString Ftmp(tmp.c_str());
			double d1 = atof(tmp.c_str());
			if (flag == 0) {
				vec[index] = d1;
			}
			else if (flag == 1){
				vec[index % 3] = d1;
			}
			++index;
			
		}
		ans.Add(vec);
		UE_LOG(LogTemp, Warning, TEXT("The vector value is: %s"), *vec.ToString());
	}
	return ans;
}
/*
TArray<FVector>  UWuyiBlueprintFunctionLibrary::ChangeFStringToVecVelocity(TArray<FString> TxtArr)
{
	TArray<FVector> ans;
	for (FString Txt : TxtArr) {
		FVector vec{};  // 位置数组
		std::string MyString(TCHAR_TO_UTF8(*Txt));
		std::istringstream isS(MyString);
		std::string tmp;
		int index = 0;
		while (std::getline(isS, tmp, ',')) { // ，分隔
			//FString Ftmp(tmp.c_str());
			double d1 = atof(tmp.c_str());
			vec[index % 3] = d1;
			++index;
			//if (index == 2) break;
			
		}
		ans.Add(vec);
		UE_LOG(LogTemp, Warning, TEXT("The velocity is: %s"), *vec.ToString());
	}
	return ans;
}
*/
UTexture2D* UWuyiBlueprintFunctionLibrary::CreateTextureFromArray(TArray<FVector> values)
{
	//纹理的参数：
	int XORY = sqrt(values.Num())+1;//不然texValues太小了存不下报错
	const int sizeX = XORY;
	const int sizeY = XORY;

	//将数组内容写入Texture2D
	// Number of values might be lower than number of pixels because of slack (closest two integer multiplication)
	int32 tex2DNumPixels = sizeX * sizeY;
	int32 numOfValues = values.Num();

	TArray<float> texValues; // temp array because of need to add alpha
	texValues.Init(0.0f, tex2DNumPixels * 4);

	int k = 0;
	for (int32 idx = 0; idx < numOfValues; ++idx)
	{
		texValues[k] = values[idx].X;
		texValues[k + 1] = values[idx].Y;
		texValues[k + 2] = values[idx].Z;
		texValues[k + 3] = 1.0f;
		k += 4;
	}

	const int32 texelCount = sizeX * sizeY;
	const int32 valueTexelCount = texValues.Num() / 4;
	//check(valueTexelCount <= texelCount);

	UTexture2D* Tex = UTexture2D::CreateTransient(sizeX, sizeX, PF_A32B32G32R32F);//创建瞬态纹理 无法保存
#if WITH_EDITORONLY_DATA
	Tex->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	Tex->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	//初始化数据：
	//得到数据的指针
	FTexture2DMipMap* mip = &Tex->PlatformData->Mips[0];
	// Lock the texture so it can be modified
	float* data = (float*)mip->BulkData.Lock(LOCK_READ_WRITE);
	// Copy Data
	FMemory::Memcpy(data, texValues.GetData(), sizeof(float) * texValues.Num());
	mip->BulkData.Unlock();
	// Update Resoruce!
	Tex->UpdateResource();

	return  Tex;
}

void UWuyiBlueprintFunctionLibrary::CreateTextureFromArrayNoreturn(TArray<FVector> values, const FString name)
{
	//资源的路径与名字：
	const FString path = "/Game/" + name;
	//const FString name = "testTex";
	//纹理的参数：
	int XORY = sqrt(values.Num())+1;
	const int sizeX = XORY;
	const int sizeY = XORY;
	const ETextureSourceFormat format = ETextureSourceFormat::TSF_BGRA8;  //?
	//将数组内容写入Texture2D

	// Number of values might be lower than number of pixels because of slack (closest two integer multiplication)
	int32 tex2DNumPixels = sizeX * sizeY;
	int32 numOfValues = values.Num();

	TArray<float> texValues; // temp array because of need to add alpha
	texValues.Init(0.0f, tex2DNumPixels * 4);

	int k = 0;
	for (int32 idx = 0; idx < numOfValues; ++idx)
	{
		texValues[k] = values[idx].X;
		texValues[k + 1] = values[idx].Y;
		texValues[k + 2] = values[idx].Z;
		texValues[k + 3] = 1.0f;
		k += 4;
	}

	const int32 texelCount = sizeX * sizeY;
	const int32 valueTexelCount = texValues.Num() / 4;
	//check(valueTexelCount <= texelCount);
	
	//包的完整路径：
	const FString PackageName = path;
	//创建包：
	UPackage* pacakge = CreatePackage(NULL, *PackageName);
	pacakge->FullyLoad();

	//创建纹理对象：
	UTexture2D* Tex = NewObject<UTexture2D>(pacakge, FName(*name), RF_Public | RF_Standalone | RF_MarkAsRootSet);

	Tex->AddToRoot(); //防止纹理的垃圾收集
	//初始化数据：
	Tex->Source.Init(sizeX, sizeX, 1, 1, format);
	//得到数据的指针
	//FTexture2DMipMap* mip = &Tex->PlatformData->Mips[0];
	//float* data = (float*)mip->BulkData.Lock(LOCK_READ_WRITE);

	uint32* BufferAddress = (uint32*)Tex->Source.LockMip(0);
	
	//数据应有的尺寸（应该和 MyData 一致）
	const int32 BufferSize = Tex->Source.CalcMipSize(0);
	FMemory::Memcpy(BufferAddress, texValues.GetData(), BufferSize);

	//数据拷贝结束
	Tex->Source.UnlockMip(0);
	//给包标脏
	Tex->MarkPackageDirty();
	//结束修改
	Tex->PostEditChange();
}