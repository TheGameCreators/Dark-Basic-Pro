// vertex shader version
vs.1.0

dcl_position      v0
dcl_normal        v3
dcl_color         v6
dcl_texcoord0     v7

// take the vertex position data ( v0 ) and multiply it by the matrix which we have put in 
// register 0, the result is stored in oPos which is the position output register
m4x4 oPos, v0, c0 

// calculate the amount of light falling on a vertex by taking the normal and dot it with the light vector
dp3 r0, v3, c4

// pass the texture coordinates from the input register ( v7 ) into the output register ( oT0 )
mov oT0.xy, v7
