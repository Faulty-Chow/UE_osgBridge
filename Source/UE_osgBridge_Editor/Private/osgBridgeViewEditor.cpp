#include "osgBridgeViewEditor.h"
#include "osgBridgeEngineSubsystem.h"

#include "SceneView.h"
#include"EditorViewportClient.h"
#include "Kismet/KismetSystemLibrary.h"

#include <algorithm>

void osgBridgeViewEditor::Tick(float DeltaTime)
{
	std::unique_lock<std::mutex> lock(_rwMutex);
	_bValid = false;
	_lastTickTimeStamp = FDateTime::Now();
	_lastTickFrameNumber = UKismetSystemLibrary::GetFrameCount();
	FEditorViewportClient* ActiveViewport = nullptr;
	int ViewportNum = GEditor->GetAllViewportClients().Num();
	for (int32 ViewIndex = 0; ViewIndex < ViewportNum; ++ViewIndex)
	{
		ActiveViewport = GEditor->GetAllViewportClients()[ViewIndex];
		if (ActiveViewport == GEditor->GetActiveViewport()->GetClient())
			break;
		else
			ActiveViewport = nullptr;
	}

	if (ActiveViewport)
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamilyContext::ConstructionValues(
			ActiveViewport->Viewport, ActiveViewport->GetScene(), ActiveViewport->EngineShowFlags)
			.SetRealtimeUpdate(ActiveViewport->IsRealtime()));
		FSceneView* View = ActiveViewport->CalcSceneView(&ViewFamily);

		_viewSize = View->UnconstrainedViewRect;
		_viewOrigin = View->ViewMatrices.GetViewOrigin();
		_projectionMatrix = View->ViewMatrices.GetProjectionMatrix();
		_viewProjectionMatrix = View->ViewMatrices.GetViewProjectionMatrix();

		_bValid = true;
	}
}