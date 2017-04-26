//transform.vsh - renders a screen-aligned quad and outputs texture coords
vs.1.1

dcl_position	v0
dcl_texcoord0	v1

//output vertex position
mov oPos, v0

//output texture coordinates
mov oT0, v1
