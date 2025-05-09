﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory_ItemWidget.h"
#include "Item/ItemObject.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Materials/MaterialInterface.h"
#include "Styling/SlateBrush.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UInventory_ItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(ItemObject)) return;
	FIntPoint IntPoint = ItemObject->GetDimension();
	Size.X = FMath::TruncToInt(IntPoint.X * TileSize - 1);
	Size.Y = FMath::TruncToInt(IntPoint.Y * TileSize - 1);

	SizeBox_BackGround->SetWidthOverride(Size.X);
	SizeBox_BackGround->SetHeightOverride(Size.Y);

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Image_Item->Slot);
	CanvasSlot->SetSize(Size);

	if (Image_Item)
	{
		FSlateBrush IconBrush = GetIconImage();
		Image_Item->SetBrush(IconBrush);
	}
}

FSlateBrush UInventory_ItemWidget::GetIconImage()
{
	UMaterialInterface* Material = ItemObject->GetIcon();

	FSlateBrush Brush;
	Brush.SetResourceObject(Material);
	Brush.ImageSize = FVector2D(FMath::TruncToInt(Size.X), FMath::TruncToInt(Size.Y));

	return Brush;
}

//void UInventory_ItemWidget::OnRemoved(UItemObject* IObject)
//{
//	OnItemRemoved.Broadcast(IObject);
//}

void UInventory_ItemWidget::CallOnRemoved()
{
	OnItemRemoved.Broadcast(ItemObject);
}


FReply UInventory_ItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventory_ItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Border_BackGround->SetBrushColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.2f));
}

void UInventory_ItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	Border_BackGround->SetBrushColor(FLinearColor(0.0f, 0.f, 0.f, 0.5f));
}

void UInventory_ItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	UDragDropOperation* DragOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	if (!DragOperation) return;

	DragOperation->DefaultDragVisual = this;
	DragOperation->Pivot = EDragPivot::CenterCenter;
	DragOperation->Payload = ItemObject;

	if (ItemObject)
	{
		CallOnRemoved();
	}

	RemoveFromParent();

	OutOperation = DragOperation;

}
