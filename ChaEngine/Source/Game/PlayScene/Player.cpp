
#include "Player.h"
#include "Stake.h"
#include "PlayerInput.h"
#include "Terrain.h"
#include "UI/MessageArea.h"
#include "UI/HorizontalGauge.h"

Player::Player(
	const GameObject _objPlayerInput,
	const GameObject _objTerrain,
	const GameObject _objMessageArea) :
	ObjectBase<PlayerData>{ _objPlayerInput, _objTerrain, _objMessageArea }
	//JsonLoader{ "Config/PlayerConfig.json" }
{
}

void Player::Start()
{
	// ジャンプの速度を求める
	jumpSpeedPerSec = std::sqrtf(2.0f * config.gravityPerSec * config.jumpTargetHeight);

	float scale{ 3.0f };

	transform.scale = { scale, scale, scale };
	obb.scale = { 1.0f / scale, 1.0f / scale, 1.0f / scale };
	obb.offset.y = 0.5f;

	state.previousHeight = GetTerrainHeight();

	objHpGauge = GetScene().Instantiate<HorizontalGauge>();
	HorizontalGaugeData& horizontalGaugeData = Get<HorizontalGauge>(objHpGauge).Data();

	horizontalGaugeData.transform.position = { 20, 830 };
	horizontalGaugeData.transform.scale = { 690, 50 };

	horizontalGaugeData.backgroundColor = Color::FromWebCode(0xE8F9FD);
	horizontalGaugeData.frameColor = Color::FromWebCode(0x59CE8F);
	horizontalGaugeData.innerColor = Color::FromWebCode(0xFF1E00);

	horizontalGaugeData.frameWidthPx = 2.0f;

	horizontalGaugeData.maxValue = 100.0f;
	horizontalGaugeData.currentValue = 75.0f;
}

void Player::Update()
{
	const float DELTA_TIME = Get<DeltaTime>();
	const PlayerInputData& input = Get<PlayerInput>(objPlayerInput).Data();

	// ワールド上での移動方向
	Vec3 move
	{
		DirectX::XMVector3TransformCoord(
			input.move,
			transform.normal)
	};
	
	// 地上と空中で移動速度を変える
	move *= (
		state.onGrounded
		? config.moveSpeedPerSec
		: config.hoverSpeedPerSec) * DELTA_TIME;

	// 移動の適用
	transform.position += move;

	// ちょっとづつ回転していく
	transform.rotation = DirectX::XMQuaternionSlerp(
		transform.rotation,
		state.targetRotation,
		DELTA_TIME * config.rotateSpeedPerSec);

	// 地形の高さ
	const float GROUND_HEIGHT = GetTerrainHeight();

	/*if (IsBlockedTerrain())
	{
		transform.position -= move;
	}
	else
	{
		state.previousHeight = GetTerrainHeight();
	}*/

	state.onGrounded = SnapTerrain();  // 地形の高さにスナップ

	if (state.onGrounded)  // 地面に立っているとき
	{
		// ジャンプの加速
		if (input.jumpOn)
		{
			state.velocityY = jumpSpeedPerSec;
			state.hoverPositionY = GROUND_HEIGHT + config.jumpTargetHeight;
			state.toStartHoveringTimeLeft = config.toStartHoveringTimeSec;
			state.jumpCount = 1;  // 最初のジャンプ
		}
		else
		{
			state.jumpCount = 0;  // ジャンプ回数リセットW
		}
	}
	else  // 空中にいるとき
	{
		if (input.jumpOn)  // ジャンプが押された瞬間
		{
			// ジャンプ回数上限まだ来てない
			if (state.jumpCount < config.jumpCountMax)
			{
				state.velocityY = jumpSpeedPerSec;
				state.hoverPositionY = transform.position.y + config.jumpTargetHeight;
				state.toStartHoveringTimeLeft = config.toStartHoveringTimeSec;
			}
			else  // ジャンプ回数の上限到達
			{
				state.hoverPositionY = transform.position.y;
				state.toStartHoveringTimeLeft = config.toStartHoveringTimeSec;
			}

			state.jumpCount++;  // ジャンプ回数増やす
		}
		else if (input.jumping)  // ジャンプが押されているとき
		{
			// ホバリング開始までの時間を引く
			state.toStartHoveringTimeLeft -= DELTA_TIME;

			// ホバリング開始
			if (state.toStartHoveringTimeLeft <= 0.0f)
			{
				// ホバリング高さとの差分
				float heightDiff = state.hoverPositionY - transform.position.y;
				if (heightDiff > 0.0f)  // 低すぎるならジェット始動
				{
					state.velocityY -= state.velocityY * config.hoverDragPerSec * DELTA_TIME;
					state.velocityY += config.jetPower * heightDiff * config.stability * DELTA_TIME;
				}
			}
		}
		else
		{
			state.toStartHoveringTimeLeft = config.toStartHoveringTimeSec;
		}
	}


	// 落下していく
	state.velocityY -= config.gravityPerSec * DELTA_TIME;

	// 移動量y
	float deltaY = state.velocityY * DELTA_TIME;

	// 建築パーツに当たっているか
	bool touchBuildParts{ false };
	Vec3 stairPush{};  // 階段からの押し出し
	for (auto& [gameObject, push] : obb.hitGameObjects)
	{
		Name* pName = TryGet<Name>(gameObject);
		bool isStair = (pName && pName->content == "BuildBlockStair");

		if (isStair)
		{
			/* 階段の処理 */
			stairPush = push;
			state.velocity.x = 0.0f;
			state.velocity.z = 0.0f;
			state.velocity.y = 0.1f;
			deltaY = 0.001f;
			touchBuildParts = true;
		}

		// 押し出し方向が上なら地面と判定
		if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(push.Normalize(), Vec3::Up())) < config.upPushThresholdCos)
		{
			continue;  // 押された方向が上向きベクトルではないなら無視
		}
		if (deltaY < -push.y || isStair)
		{
			touchBuildParts = true;

			// もし乗っている対象に剛体コンポーネントがついているなら、速度をこっちにも適用
			RigidBody* pRigidBody = TryGet<RigidBody>(gameObject);
			if (pRigidBody)
			{
				state.velocity = pRigidBody->velocity;
			}
		}
	}

	transform.position = transform.position + state.velocity * DELTA_TIME;
	state.velocity *= std::pow(config.friction, DELTA_TIME);

	// 地面に着地処理
	if (transform.position.y + deltaY <= GROUND_HEIGHT || touchBuildParts)
	{
		state.velocityY = 0.0f;
		if (!touchBuildParts)
		{
			transform.position.y = GROUND_HEIGHT;
			state.velocity = Vec3::Zero();
		}
		else
		{
			transform.position.y += stairPush.y;
		}
		if (!state.onGrounded)
		{
			// 着地地した瞬間
			state.onGrounded = true;
		}
	}
	else
	{
		transform.position.y += deltaY;
		state.onGrounded = false;
	}

	// 平行光線のターゲット座標をプレイヤーの座標にする
	Get<LightManager>().SetPosition(transform.WorldPosition());

	transform.UpdateFamily();

	if (Get<InputSystem>().GetKeyDown(KeyCode::R))
	{
		MessageArea& messageArea = Get<MessageArea>(objMessageArea);
		messageArea.ShowText("テストだよー", "プレイヤー");
	}

	if (move.Length() > 0.0f)
	{
		animStatus = AnimStatus_Run;
	}
	else
	{
		animStatus = AnimStatus_Idle;
	}
}

void Player::Draw()
{
	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	model.Get().UpdateAnim(Get<DeltaTime>(), animStatus);

	const auto& subParts{ model.Get().subParts };
	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

bool Player::IsBlockedTerrain()
{
	if (GetTerrainHeight() - state.previousHeight > config.stepHeightMax)
	{
		return true;  // 登れない
	}

	return false;  // 登れる
}

bool Player::SnapTerrain()
{
	auto [x, y, z] { transform.position.v };

	// プレイヤーのy軸における地形の高さ
	const float HEIGHT = GetTerrainHeight();

	// もし地形より高さが低いなら地形の高さへ移動する
	if (y < HEIGHT)
	{
		transform.position.y = HEIGHT;
		return true;  // スナップした
	}
	return false;  // スナップしなかった
}

const float Player::GetTerrainHeight()
{
	auto [x, y, z] { transform.position.v };

	// プレイヤーのy軸における地形の高さ
	const float HEIGHT = Get<Terrain>(objTerrain).GetHeight(x, z);

	return HEIGHT;
}
