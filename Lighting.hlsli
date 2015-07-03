float G_Smith(float roughness, float NoV, float NoL)
{
	float  k = (roughness + 1) * (roughness + 1) / 8;
	return  (NoV / (NoV * (1 - k) + k)) *  (NoL / (NoL * (1 - k) + k));
}

float3 GGXBRDF(float3 lightDir,float3 lightPos,float3 albedo,float3 normal,float3 viewDir,float3 specular,float gloss)
{
	const  float pi = 3.14159;
	float3 h = normalize(viewDir + lightDir);


	float NdotL = max(0, dot(normal, lightDir));
	float NdotH = max(0, dot(normal, h));
	float LdotH = max(0, dot(lightDir, h));
	float VdotH = max(0, dot(viewDir, h));
	float NdotV = max(0, dot(normal, viewDir));
	float roughness = gloss;

	//D
	float alpha = roughness *  roughness;
	float alphaSqr = alpha*alpha;
	float denom = ((NdotH * NdotH) * (alphaSqr - 1.0f) + 1.0f);
	float D = alphaSqr / (pi * denom* denom);
	float FV;

	//fersnel & V
	float F_b = pow((1 - VdotH), 5);
	float F_a = 1;
	float k = alpha / 2;
	float	vis = G_Smith(roughness, NdotV, NdotL);
	float2 FV_helper = float2(F_a*vis, F_b*vis);
	float3 col = specular*FV_helper.x + (1 - specular)*FV_helper.y;

	col = (NdotL*D*col + NdotL*albedo);
	
	return float4(col,1);
}