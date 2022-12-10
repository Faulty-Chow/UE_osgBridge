#include "osgBridgeView.h"

float osgBridgeView::GetSphereBoundsPixelSizeInView(const FBoxSphereBounds& sphereBounds, bool bABS /*= true*/)
{
	std::unique_lock<std::mutex> lock(_rwMutex);
#define M _projectionMatrix.M
	float far, near, left, right, top, bottom;
	near = M[3][2] / (M[2][2] - 1.f);
	far = M[3][2] / (1.f + M[2][2]);
	left = near * (M[2][0] - 1.f) / M[0][0];
	right = near * (1.f + M[2][0]) / M[0][0];
	top = near * (1.f + M[2][1]) / M[1][1];
	bottom = near * (M[2][1] - 1.f) / M[1][1];
#undef M
	float fovy = (atanf(top / near) - atanf(bottom / near)) / PI * 180.f;
	float aspectRatio = (right - left) / (top - bottom);

	float distance = FVector::Dist(sphereBounds.Origin, _viewOrigin);

	float angularSize = 2.f * atanf(sphereBounds.SphereRadius / distance) / PI * 180.f;
	float dpp = std::max(fovy, (float)1.0e-17) / (float)_viewSize.Height();
	float pixelSize = angularSize / dpp;
	if (bABS)
		return fabs(pixelSize);
	else
		return pixelSize;
}

bool osgBridgeView::IsSphereBoundsInView(const FBoxSphereBounds& sphereBounds)
{
	std::unique_lock<std::mutex> lock(_rwMutex);
#define M _viewProjectionMatrix.M
#define X sphereBounds.Origin.X
#define Y sphereBounds.Origin.Y
#define Z sphereBounds.Origin.Z
#define D (sphereBounds.SphereRadius * 2)
	float left = (M[0][3] + M[0][0]) * X + (M[1][0] + M[1][3]) * Y + (M[2][0] + M[2][3]) * Z + (M[3][0] + M[3][3]);
	float right = (M[0][3] - M[0][0]) * X + (M[1][3] - M[1][0]) * Y + (M[2][3] - M[2][0]) * Z + (M[3][3] - M[3][0]);
	float top = (M[0][3] - M[0][1]) * X + (M[1][3] - M[1][1]) * Y + (M[2][3] - M[2][1]) * Z + (M[3][3] - M[3][1]);
	float bottom = (M[0][3] + M[0][1]) * X + (M[1][3] + M[1][1]) * Y + (M[2][3] + M[2][1]) * Z + (M[3][3] + M[3][1]);
	float near = (M[0][3] + M[0][2]) * X + (M[1][3] + M[1][2]) * Y + (M[2][3] + M[2][2]) * Z + (M[3][3] + M[3][2]);
	float far = (M[0][3] - M[0][2]) * X + (M[1][3] - M[1][2]) * Y + (M[2][3] - M[2][2]) * Z + (M[3][3] - M[3][2]);
	return left > -D && right > -D && top > -D && bottom > -D && near > -D && far > -D;
#undef M
#undef X
#undef Y
#undef Z
#undef R
}