//kernel to add two vectors in parallel, similar to nvidia sdk sample.
__kernel void oclVectorAdd( __global const float* vect_a, __global const float* vect_b, __global float* result, unsigned vect_size )
{
	int global_id = get_global_id(0);

	if( global_id <= vect_size )
		result[global_id] = vect_a[global_id] + vect_b[global_id];
}