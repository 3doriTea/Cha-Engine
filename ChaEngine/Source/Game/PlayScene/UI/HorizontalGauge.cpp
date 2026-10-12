#include "HorizontalGauge.h"
#include <Game/Common/Canvas.h>

void HorizontalGauge::Draw()
{
	Canvas& canvas = Get<Canvas>();
	float ratio{ currentValue / maxValue };

	// 枠の描画
	Vec2 frameBegin{ transform.position - Vec2::One() * frameWidthPx };
	Vec2 frameEnd{ (transform.position + transform.scale) + Vec2::One() * frameWidthPx };
	canvas.DrawBox(frameBegin, frameEnd, frameColor, transform.layerOrder);

	// 内側の背景描画
	canvas.DrawBox(transform, backgroundColor);
	
	// 内側のバー描画
	Vec2 innerBegin{ transform.position };
	Vec2 innerEnd{ transform.position + Vec2{ transform.scale.x * ratio, transform.scale.y } };
	canvas.DrawBox(innerBegin, innerEnd, innerColor, transform.layerOrder);
}
