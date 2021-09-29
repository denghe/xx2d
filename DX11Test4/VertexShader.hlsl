cbuffer BufProj : register(b0) {
	float4 u_projection;
};

struct VS_INPUT
{
	float2 vert					: TEXCOORD0;
	float4 center_scale_rotate	: TEXCOORD1;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

PS_INPUT main( VS_INPUT i )
{
	PS_INPUT o;
    float2 offset = float2(i.vert.x - 0.5, i.vert.y - 0.5);
    float2 texcoord = i.vert;
    float2 center = i.center_scale_rotate.xy;
    float scale = i.center_scale_rotate.z;
    float rotate = i.center_scale_rotate.w;

    float cos_theta = cos(rotate);
    float sin_theta = sin(rotate);
    float2 v1 = offset * scale;
    float2 v2 = float2(
       dot(v1, float2(cos_theta, sin_theta)),
       dot(v1, float2(-sin_theta, cos_theta))
    );
    float2 v3 = (v2 + center) * u_projection.xy + u_projection.zw;

    o.texcoord = texcoord;
    o.pos = float4(v3.x, v3.y, 0.0, 1.0);
	return o;
}
