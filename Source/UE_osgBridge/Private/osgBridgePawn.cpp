#include "osgBridgePawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "osgBridgeEngineSubsystem.h"

osgBridgePawn::osgBridgePawn(const osgBridgePawn& other)
{
	_bValid = other._bValid;
	_lastTickTimeStamp = other._lastTickTimeStamp;
	_lastTickFrameNumber = other._lastTickFrameNumber;
	_viewOrigin = other._viewOrigin;
	_projectionMatrix = other._projectionMatrix;
	_viewProjectionMatrix = other._viewProjectionMatrix;
	_viewSize = other._viewSize;
}

void osgBridgePawn::Tick(float DeltaTime)
{
	std::unique_lock<std::mutex> lock(_rwMutex);
	_bValid = false;
	_lastTickTimeStamp = FDateTime::Now();
	_lastTickFrameNumber = UKismetSystemLibrary::GetFrameCount();
	UWorld* world = UosgBridgeEngineSubsystem::GetOsgBridgeEngineSubsystem()->GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			ULocalPlayer* localPlayer = Cast<ULocalPlayer>(playerController->Player);
			if (localPlayer)
			{
				FSceneViewProjectionData projectionData;
				FVector out_Location;
				FRotator out_Rotation;
				playerController->GetPlayerViewPoint(out_Location, out_Rotation);
				FMatrix ViewRotationMatrix = FInverseRotationMatrix(out_Rotation) * FMatrix(
					FPlane(0, 0, 1, 0),
					FPlane(1, 0, 0, 0),
					FPlane(0, 1, 0, 0),
					FPlane(0, 0, 0, 1));
				if (!ViewRotationMatrix.GetOrigin().IsNearlyZero(0.0f))
				{
					out_Location += ViewRotationMatrix.InverseTransformPosition(FVector::ZeroVector);
					ViewRotationMatrix = ViewRotationMatrix.RemoveTranslation();
				}
				projectionData.ViewOrigin = out_Location;
				projectionData.ViewRotationMatrix = ViewRotationMatrix;

				int32 X = FMath::TruncToInt(localPlayer->Origin.X * localPlayer->ViewportClient->Viewport->GetSizeXY().X);
				int32 Y = FMath::TruncToInt(localPlayer->Origin.Y * localPlayer->ViewportClient->Viewport->GetSizeXY().Y);
				uint32 SizeX = FMath::TruncToInt(localPlayer->Size.X * localPlayer->ViewportClient->Viewport->GetSizeXY().X);
				uint32 SizeY = FMath::TruncToInt(localPlayer->Size.Y * localPlayer->ViewportClient->Viewport->GetSizeXY().Y);

				FIntRect Rectangle = FIntRect(X, Y, X + SizeX, Y + SizeY);
				projectionData.SetViewRectangle(Rectangle);

				FMinimalViewInfo OutViewInfo;
				if (playerController->PlayerCameraManager)
				{
					OutViewInfo = playerController->PlayerCameraManager->CameraCache.POV;
					OutViewInfo.FOV = playerController->PlayerCameraManager->GetFOVAngle();
					playerController->GetPlayerViewPoint(OutViewInfo.Location, OutViewInfo.Rotation);
				}
				else
				{
					playerController->GetPlayerViewPoint(OutViewInfo.Location, OutViewInfo.Rotation);
				}
				FMinimalViewInfo::CalculateProjectionMatrixGivenView(OutViewInfo, localPlayer->AspectRatioAxisConstraint, localPlayer->ViewportClient->Viewport, projectionData);

				_viewOrigin = projectionData.ViewOrigin;
				_projectionMatrix = projectionData.ProjectionMatrix;
				_viewProjectionMatrix = projectionData.ComputeViewProjectionMatrix();
				_viewSize = projectionData.GetViewRect();
				_bValid = true;
			}
			else
				_bValid = false;
		}
		else
			_bValid = false;
	}
	else
		_bValid = false;
}