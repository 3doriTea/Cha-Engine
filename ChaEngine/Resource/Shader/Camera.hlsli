cbuffer Camera : register(b1)
{
	row_major matrix Camera_ViewProjection;
	float4 Camera_Position;
};