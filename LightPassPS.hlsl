#include "DeferredRender.hlsli"
#include "Lighting.hlsli"

Texture2D gAlbedoTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gSpecularGlossTexture : register(t2);
Texture2D gDepth: register(t3);

sampler gLinearSample;

float4 main(vs_out pIn) : SV_TARGET
{


float z = gDepth[pIn.position.xy];
float4 vProjectedPos = float4(pIn.position.xy, z, 1.0f);
// Transform by the inverse screen view projection matrix to world space
float4 vPositionWS = mul(vProjectedPos, gInvPV);
// Divide by w to get the view-space position
vPositionWS= vPositionWS / vPositionWS.w;
float3 albedo = gAlbedoTexture[pIn.position.xy].xyz;
float3 normal = normalize(gNormalTexture[pIn.position.xy].xyz);
float4 specGloss = gSpecularGlossTexture[pIn.position.xy].xyzw;

float3 col=GGXBRDF(normalize(gLightPos.xyz- vPositionWS.xyz), gLightPos, albedo ,normal,
	normalize(gCamPos - vPositionWS), specGloss.xyz, specGloss.w);
float d = length(gLightPos.xyz - vPositionWS.xyz);
col = col*(1.0f / (1.0f + 0.01f*d + 0.001f*d));
return float4(col,1.0f);


}