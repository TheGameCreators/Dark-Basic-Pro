//
// Aniso
//

string Category = "Effects\\Lighting";
string keywords = "aniso,dx8,assembler,fixed function,pointlight";
string description = "Using a texture for BRDF in DX8. No pixel shader! Prog in vs1.1 assembler";

// un-tweakables

float4x4 worldITa : WorldIT;
float4x4 wvp : WorldViewProjection;
float4x4 worlda : World;
float4x4 viewIT : ViewIT;

// tweakables

float4 lightPos : Position
<
	string Object = "PointLight";
	string Space = "World";
> = { 100.0f, 100.0f, 100.0f, 0.0f };

texture anisoTexture
<
	 string File = "aniso2.dds";
	 string TextureType = "2D";
>;

///////////////////////

technique Aniso
{
	pass p0
	{
		VertexShaderConstant[0] = <wvp>;
		VertexShaderConstant[4] = <worldITa>;
		VertexShaderConstant[8] = <worlda>;
		VertexShaderConstant[12] = <lightPos>;
		VertexShaderConstant[16] = <viewIT>;

		VertexShader = 
		asm
		{
			vs.1.1
			// Transform pos to screen space.
			m4x4 oPos, v0, c0
			
			// Normal to world space:
			dp3 r0.x, v3, c8
			dp3 r0.y, v3, c9
			dp3 r0.z, v3, c10

			// normalize normal
			dp3 r0.w, r0, r0
			rsq r0.w, r0.w
			mul r0, r0, r0.w			// r0 has normalized normal.
			
			// vpos to world space.
			dp4 r1.x, v0, c8
			dp4 r1.y, v0, c9
			dp4 r1.z, v0, c10
			dp4 r1.w, v0, c11			// r1 has position in world space.

			// eye vector, normalize.
			add r2, c19, -r1
			dp3 r2.w, r2, r2
			rsq r2.w, r2.w
			mul r2, r2, r2.w			// r2 has normalized eye vector in world space.
			
            // light vector, normalize
            add r3, c12, -r1
			dp3 r3.w, r3, r3
			rsq r3.w, r3.w
			mul r3, r3, r3.w            // r3 has normalized light vector in world space.

			// half-angle direction.
			add r1, r2, r3

			// normalize h
			dp3 r1.w, r1, r1
			rsq r1.w, r1.w
			mul r1, r1, r1.w

			// L dot N.
			dp3 oT0.x, r3, r0

			// h dot N
			dp3 oT0.y, r1, r0
		};

		ZEnable = true;
		ZWriteEnable = true;
		AlphaBlendEnable = false;
		CullMode = None;
		Lighting = false;

		Texture[0] = <anisoTexture>;
        TexCoordIndex[0] = 0;
		Target[0] = Texture2D;

		MinFilter[0] = Linear;
		MagFilter[0] = Linear;
		MipFilter[0] = None;

        AddressU[0] = Clamp;
        AddressV[0] = Clamp;

		ColorOp[0] = Modulate4x;
		ColorArg1[0] = Texture;
		ColorArg2[0] = Texture|AlphaReplicate;
	}
}
