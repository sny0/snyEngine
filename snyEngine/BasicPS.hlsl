#include"BasicType.hlsli"
Texture2D<float4> tex:register(t0);//0番スロットに設定されたテクスチャ
SamplerState smp:register(s0);//0番スロットに設定されたサンプラ
/*
Texture2D<float4> tex:register(t0);//0番スロットに設定されたテクスチャ(ベース)
Texture2D<float4> sph:register(t1);//1番スロットに設定されたテクスチャ(乗算)
Texture2D<float4> spa:register(t2);//2番スロットに設定されたテクスチャ(加算)
Texture2D<float4> toon:register(t3);//3番スロットに設定されたテクスチャ(トゥーン)
*/

//SamplerState smp:register(s0);//0番スロットに設定されたサンプラ
//SamplerState smpToon:register(s1);//1番スロットに設定されたサンプラ

//定数バッファ0
cbuffer SceneData : register(b0) {
	matrix world;//ワールド変換行列
	matrix view;
	matrix proj;//ビュープロジェクション行列
	float3 eye;
};
//定数バッファ1
//マテリアル用

cbuffer Material : register(b1) {
	float3 ambientColor; // 環境光
	float3 diffuseColor; // 拡散反射光
	float3 specularColor; // 鏡面反射光
	float dissolve; // 透明度
	float specularity; // 鏡面反射率
};

float4 BasicPS(BasicType input) : SV_TARGET{
	//return float4(input.normal.xyz, 1.0f);
	//return float4(specular.xyz, 1.0f);

	float4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f}; // ライトの光
	float3 lightVec = normalize(float3(1.0f, -1.0f, 1.0f)); // ライト（ディレクションライト）の方向ベクトル

	float diffuseBrightness = saturate(dot(-lightVec, input.normal)); // 拡散反射の強さ

	float3 reflectLightVec = normalize(reflect(lightVec, input.normal.xyz)); // ライトがテクセルの法線で反射したときのベクトル
	float specularBrightness = pow(saturate(dot(reflectLightVec, -input.ray)), 3) * specularity/1000; // 鏡面反射の強さ

	float4 textureColor = tex.Sample(smp, input.uv); // テクスチャの色

	float4 finalColor = saturate(lightColor * textureColor * diffuseBrightness * float4(diffuseColor.xyz, 1.0f)); // 拡散反射の適応
	finalColor += saturate(lightColor * textureColor * specularBrightness * float4(specularColor.xyz, 1.0f)); // 鏡面反射の適応
	finalColor += 0.5 * saturate(lightColor * textureColor *  float4(ambientColor.xyz, 1.0f)); // 環境光の適応
	finalColor *= float4(1.0f, 1.0f, 1.0f, dissolve); // 透明度の適応
	finalColor = saturate(finalColor); // 丸める

	return finalColor;
}