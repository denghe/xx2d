Texture2D t : register(t0);
SamplerState s : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

float4 main( PS_INPUT i ) : SV_Target
{
	return t.Sample(s, i.texcoord);
}
