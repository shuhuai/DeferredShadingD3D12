#include "DeferredRender.hlsli"



vs_out  main(vs_in vIn)
{
	vs_out vOut;
	vOut.position = vIn.position;
	vOut.texcoord = vIn.texcoord;
	return vIn;
}