//
// Effect file - Rainbow
//

float4 lhtDir < string UIDirectional = "Light Direction"; >;
float4 EyePos : EyePosition;
matrix mTot : WorldViewProjection;
matrix mWorldIT : WorldIT;
matrix mWld : World;

string XFile = "sphere.x";
texture tRainbow < string name = "rainbow.tga"; >;

technique tec0
{ 
    pass p0
    {
        VertexShaderConstant[0] = <mTot>;
        VertexShaderConstant[4] = <lhtDir>;
        VertexShaderConstant[5] = <mWorldIT>;
        VertexShaderConstant[9] = {0.0, 0.5, 1.0, -1.0};
        VertexShaderConstant[10] = <EyePos>;
        VertexShaderConstant[11] = <mWld>;

        Texture[0]   = <tRainbow>;
        Texture[1]   = <tRainbow>;
        
        AddressU[0] = Wrap;
        AddressV[0] = Wrap;
        AddressU[1] = Wrap;
        AddressV[1] = Wrap;

        MinFilter[0] = Linear;
        MagFilter[0] = Linear;
        MipFilter[0] = Linear;
        MinFilter[1] = Linear;
        MagFilter[1] = Linear;
        MipFilter[1] = Linear;

        ColorOp[0]   = Modulate2X; 
        ColorArg1[0] = Texture;
        ColorArg2[0] = Texture | AlphaReplicate;
        ColorOp[1]   = AddSigned; 
        ColorArg1[1] = Texture | AlphaReplicate;
        ColorArg2[1] = Current;

        VertexShader = 
        asm
        {
            vs.1.0
           
            dcl_position      v0        // Position
            dcl_normal        v3        // Normal

		; v0  -- position
		; v3  -- normal
		;
		; c0-3   -- world/view/proj matrix
		; c4     -- light vector
		; c5-8   -- inverse/transpose world matrix
		; c9     -- {0.0, 0.5, 1.0, -1.0}
		; c10    -- eye point
		; c11-14 -- world matrix

		;transform position
		dp4 oPos.x, v0, c0
		dp4 oPos.y, v0, c1
		dp4 oPos.z, v0, c2
		dp4 oPos.w, v0, c3

		;transform normal
		dp3 r0.x, v3, c5
		dp3 r0.y, v3, c6
		dp3 r0.z, v3, c7

		;normalize normal
		dp3 r0.w, r0, r0
		rsq r0.w, r0.w
		mul r0, r0, r0.w

		;compute world space position
		dp4 r1.x, v0, c11
		dp4 r1.y, v0, c12
		dp4 r1.z, v0, c13
		dp4 r1.w, v0, c14

		;vector from point to eye
		add r2, c10, -r1

		;normalize e
		dp3 r2.w, r2, r2
		rsq r2.w, r2.w
		mul r2, r2, r2.w

		;h = Normalize( n + e )
		add r1, r0, r2

		;normalize h
		dp3 r1.w, r1, r1
		rsq r1.w, r1.w
		mul r1, r1, r1.w

		;h dot n
		dp3 oT0.x, r0, r2
		dp3 oT0.y, r1, r0

		mov oT1.x, r2
		mov oT1.y, r1
        };
    }
}
