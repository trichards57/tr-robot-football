#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
__kernel void main(__constant float4 * ball, __constant float4 *field, __global float4 * out)
{
	float4 gridPos = (float4)(get_global_id(0),get_global_id(1),0,0);

	size_t index = gridPos.x + gridPos.y * get_global_size(0);

	float4 realPos = gridPos * field[0].z;

	float4 diff = ball[0] - realPos;

	float dist = sqrt(diff.x*diff.x+diff.y*diff.y);

	float attractField = 0.5 * ball[0].w * dist;

	float4 res = (float4)(attractField, realPos.x,realPos.y,0);

	out[index] = res;
}