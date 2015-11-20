//
// FX Effect file - Cartoon
//

// Constants
matrix wvp : WorldViewProjection;
matrix world : World;
matrix worldIT : WorldIT;
float4 eyePos : EyePosition;
float4 lhtDir < string UIDirectional = "Light Direction"; >;

// Model and Texture Names
string XFile = "default.x";
texture toonshade < string name = "shade.bmp"; >;
texture toonedge < string name = "edge.dds"; >;

technique Toon
{
	pass p0
	{
		VertexShaderConstant[0] = <wvp>;
		VertexShaderConstant[4] = <worldIT>;
		VertexShaderConstant[8] = <world>;
		VertexShaderConstant[12] = <eyePos>;
		VertexShaderConstant[13] = <lhtDir>;

		VertexShader = 
		asm
		{

			// v0  -- position
			// v3  -- normal

			vs.1.1

			dcl_position      v0
			dcl_normal        v3

			// transform position
			dp4 oPos.x, v0, c0
			dp4 oPos.y, v0, c1
			dp4 oPos.z, v0, c2
			dp4 oPos.w, v0, c3

			// transform normal
			dp3 r0.x, v3, c8
			dp3 r0.y, v3, c9
			dp3 r0.z, v3, c10

			// normalize normal
			dp3 r0.w, r0, r0
			rsq r0.w, r0.w
			mul r0, r0, r0.w

			// compute world space position
			dp4 r1.x, v0, c8
			dp4 r1.y, v0, c9
			dp4 r1.z, v0, c10
			dp4 r1.w, v0, c11

			// vector from point to eye
			add r2, c12, -r1

			// normalize e
			dp3 r2.w, r2, r2
			rsq r2.w, r2.w
			mul r2, r2, r2.w

			// e dot n
			dp3 oT1.x, r0, r2

			// l dot n
			dp3 oT0.x, r0, -c13
		};

		Texture[0] = <toonshade>;
		Texture[1] = <toonedge>;

		MinFilter[0] = Linear;
		MagFilter[0] = Linear;
		MipFilter[0] = Linear;
		MinFilter[1] = Linear;
		MagFilter[1] = Linear;
		MipFilter[1] = Linear;

		AddressU[0] = Clamp;
		AddressV[0] = Clamp;
		AddressU[1] = Clamp;
		AddressV[1] = Clamp;

		ColorOp[0] = SelectArg1;
		ColorArg1[0] = Texture;

		ColorOp[1] = Modulate;
		ColorArg1[1] = Texture;
		ColorArg2[1] = Current;
	}
}
