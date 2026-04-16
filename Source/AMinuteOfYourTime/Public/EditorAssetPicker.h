// Copyright (c) 2025, Missing Planet LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "PropertyCustomizationHelpers.h"
#include "EditorAssetPicker.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEditorAssetSelected, const FAssetData&, AssetData);

/**
 * 
 */
UCLASS()
class AMINUTEOFYOURTIME_API UEditorAssetPicker : public UWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Asset Picker")
	UObject* GetSelectedAsset() const { return SelectedAsset.GetAsset(); }

	UFUNCTION(BlueprintCallable, Category = "Asset Picker")
	const FAssetData& GetSelectedAssetData() const { return SelectedAsset; }

	UFUNCTION(BlueprintCallable, Category = "Asset Picker")
	void ClearSelection();

	/** C++ typed getter — not exposed to BP, use the function library instead */
	template<typename T>
	T* GetSelectedAssetAs() const { return Cast<T>(SelectedAsset.GetAsset()); }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Picker")
	TSubclassOf<UObject> AllowedClass;

	UPROPERTY(BlueprintAssignable, Category = "Asset Picker")
	FOnEditorAssetSelected OnAssetSelected;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	void OnAssetChanged(const FAssetData& InAssetData);
	
	FAssetData SelectedAsset;
	TSharedPtr<SObjectPropertyEntryBox> EntryBox;
	TSharedPtr<FAssetThumbnailPool> ThumbnailPool;
	TSharedPtr<FAssetThumbnail> AssetThumbnail;
};

UCLASS()
class AMINUTEOFYOURTIME_API UEditorAssetPickerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Asset Picker", meta = (DeterminesOutputType="Class", DynamicOutputParam="OutAsset"))
	static UObject* GetSelectedAssetOfClass(UEditorAssetPicker* Picker, TSubclassOf<UObject> Class, UObject*& OutAsset);
};
