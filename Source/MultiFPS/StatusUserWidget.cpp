// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "StatusUserWidget.h"
//
// #include "Blueprint/WidgetTree.h"
// #include "Components/CanvasPanel.h"
// #include "Components/CanvasPanelSlot.h"
// #include "Components/ProgressBar.h"
//
// class UCanvasPanel;
// class UCanvasPanelSlot;
//
// // UStatusUserWidget::NativeConstruct()
// // {
// // 	Super::NativeConstruct();
// //
// // 	if (UCanvasPanel* RootCanvas = Cast<UCanvasPanel>(GetRootWidget()))
// // 	{
// // 		// Создаем ProgressBar для здоровья
// // 		HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass());
// // 		if (HealthBar)
// // 		{
// // 			HealthBar->SetPercent(1.0f); // Устанавливаем начальное значение в 100%
// // 			HealthBar->SetFillColorAndOpacity(FLinearColor::Red); // Цвет заполнения
// // 			// Добавляем ProgressBar в Canvas
// // 			RootCanvas->AddChildToCanvas(HealthBar);
// //
// // 			// Задаем расположение и размер ProgressBar
// // 			UCanvasPanelSlot* HealthBarSlot = Cast<UCanvasPanelSlot>(HealthBar->Slot);
// // 			if (HealthBarSlot)
// // 			{
// // 				HealthBarSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
// // 				HealthBarSlot->SetOffsets(FMargin(-150.0f, -22.0f, 300.0f, 44.0f));
// // 			}
// // 		}
// //
// // 		// Создаем ProgressBar для брони
// // 		ChargeBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass());
// // 		if (ChargeBar)
// // 		{
// // 			ChargeBar->SetPercent(1.0f);
// // 			ChargeBar->SetFillColorAndOpacity(FLinearColor::Blue);
// // 			RootCanvas->AddChildToCanvas(ChargeBar);
// //
// // 			// Задаем расположение и размер ProgressBar
// // 			UCanvasPanelSlot* ChargeBarSlot = Cast<UCanvasPanelSlot>(ChargeBar->Slot);
// // 			if (ChargeBarSlot)
// // 			{
// // 				ChargeBarSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
// // 				ChargeBarSlot->SetOffsets(FMargin(-150.0f, 28.0f, 300.0f, 44.0f));
// // 			}
// // 		}
// // 	}
// // 	
// // }
//
// void UStatusUserWidget::UpdateHealthBar(float Health) const
// {
// 	HealthBar->SetPercent(Health);
// }
//
// void UStatusUserWidget::UpdateChargeBar(float Charge) const
// {
// 	ChargeBar->SetPercent(Charge);
// }
