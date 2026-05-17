// Copyright (c) 2025, Missing Planet LLC. All rights reserved.


#include "EditorAssetPicker.h"

#include "ThumbnailRendering/ThumbnailManager.h"

TSharedRef<SWidget> UEditorAssetPicker::RebuildWidget()
{
	UClass* FilterClass = AllowedClass ? AllowedClass.Get() : UObject::StaticClass();

	ThumbnailPool = UThumbnailManager::Get().GetSharedThumbnailPool();
	AssetThumbnail = MakeShared<FAssetThumbnail>(SelectedAsset, 64, 64, ThumbnailPool);

	SAssignNew(EntryBox, SObjectPropertyEntryBox)
		.AllowedClass(FilterClass)
		.OnObjectChanged(FOnSetObject::CreateUObject(this, &UEditorAssetPicker::OnAssetChanged));

	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.MinWidth(64)
		.Padding(10.f)
		[
			AssetThumbnail->MakeThumbnailWidget()
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			EntryBox.ToSharedRef()
		];
}

void UEditorAssetPicker::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	EntryBox.Reset();
	AssetThumbnail.Reset();
}


void UEditorAssetPicker::OnAssetChanged(const FAssetData& InAssetData)
{
	SelectedAsset = InAssetData;
	OnAssetSelected.Broadcast(SelectedAsset);
	if (AssetThumbnail.IsValid())
	{
		AssetThumbnail->SetAsset(SelectedAsset);
		ThumbnailPool->Tick(0.0f); // Force an immediate pool tick so it renders right away
	}
}

void UEditorAssetPicker::ClearSelection()
{
	SelectedAsset = FAssetData();
	TakeWidget();
}

UObject* UEditorAssetPickerLibrary::GetSelectedAssetOfClass(UEditorAssetPicker* Picker, TSubclassOf<UObject> Class,
	UObject*& OutAsset)
{
	OutAsset = nullptr;
	if (!Picker || !Class) return nullptr;

	UObject* Selected = Picker->GetSelectedAsset();
	if (Selected && Selected->IsA(Class))
	{
		OutAsset = Selected;
	}

	return OutAsset;
}
