#include"BasicType.hlsli"
Texture2D<float4> tex:register(t0);//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp:register(s0);//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��
/*
Texture2D<float4> tex:register(t0);//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(�x�[�X)
Texture2D<float4> sph:register(t1);//1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(��Z)
Texture2D<float4> spa:register(t2);//2�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(���Z)
Texture2D<float4> toon:register(t3);//3�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(�g�D�[��)
*/

//SamplerState smp:register(s0);//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��
//SamplerState smpToon:register(s1);//1�ԃX���b�g�ɐݒ肳�ꂽ�T���v��

//�萔�o�b�t�@0
cbuffer SceneData : register(b0) {
	matrix world;//���[���h�ϊ��s��
	matrix view;
	matrix proj;//�r���[�v���W�F�N�V�����s��
	float3 eye;
};
//�萔�o�b�t�@1
//�}�e���A���p

cbuffer Material : register(b1) {
	float3 ambientColor; // ����
	float3 diffuseColor; // �g�U���ˌ�
	float3 specularColor; // ���ʔ��ˌ�
	float dissolve; // �����x
	float specularity; // ���ʔ��˗�
};

float4 BasicPS(BasicType input) : SV_TARGET{
	//return float4(input.normal.xyz, 1.0f);
	//return float4(specular.xyz, 1.0f);

	float4 lightColor = {1.0f, 1.0f, 1.0f, 1.0f}; // ���C�g�̌�
	float3 lightVec = normalize(float3(1.0f, -1.0f, 1.0f)); // ���C�g�i�f�B���N�V�������C�g�j�̕����x�N�g��

	float diffuseBrightness = saturate(dot(-lightVec, input.normal)); // �g�U���˂̋���

	float3 reflectLightVec = normalize(reflect(lightVec, input.normal.xyz)); // ���C�g���e�N�Z���̖@���Ŕ��˂����Ƃ��̃x�N�g��
	float specularBrightness = pow(saturate(dot(reflectLightVec, -input.ray)), 3) * specularity/1000; // ���ʔ��˂̋���

	float4 textureColor = tex.Sample(smp, input.uv); // �e�N�X�`���̐F

	float4 finalColor = saturate(lightColor * textureColor * diffuseBrightness * float4(diffuseColor.xyz, 1.0f)); // �g�U���˂̓K��
	finalColor += saturate(lightColor * textureColor * specularBrightness * float4(specularColor.xyz, 1.0f)); // ���ʔ��˂̓K��
	finalColor += 0.5 * saturate(lightColor * textureColor *  float4(ambientColor.xyz, 1.0f)); // �����̓K��
	finalColor *= float4(1.0f, 1.0f, 1.0f, dissolve); // �����x�̓K��
	finalColor = saturate(finalColor); // �ۂ߂�

	return finalColor;
}