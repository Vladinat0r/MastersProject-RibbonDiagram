//OpenCL multi-threaded implementation of the Cubic B-Spline algorithm
//Author: Volodymyr Nazarenko (100174968)

__kernel void calc_Cubic_Spline(__global const float *points,	__global const float *normals,
								__global float *splineSegCA,	__global float *splineSegO) {
	int i = get_global_id(0);
	int LOD = get_global_size(0);

	float t = (float)i / (LOD - 1);
	float it = 1.0f - t; // t value inverted

	//Calculate blending functions
	float t2 = t*t;
	float t3 = t*t*t;
	float it3 = it*it*it;

	float b0 = it3 / 6.0f;
	float b1 = (3 * t3 - 6 * t2 + 4) / 6.0f;
	float b2 = (-3 * t3 + 3 * t2 + 3 * t + 1) / 6.0f;
	float b3 = t3 / 6.0f;

	//Sum the control points mulitplied by their respective blending functions
	int index0 = 0;
	int index1 = 3;
	int index2 = 6;
	int index3 = 9;

	float x =	b0 * points[index0] +
				b1 * points[index1] +
				b2 * points[index2] +
				b3 * points[index3];

	float y =	b0 * points[index0 + 1] +
				b1 * points[index1 + 1] +
				b2 * points[index2 + 1] +
				b3 * points[index3 + 1];

	float z =	b0 * points[index0 + 2] +
				b1 * points[index1 + 2] +
				b2 * points[index2 + 2] +
				b3 * points[index3 + 2];


	float xNorm =	b0 * (points[index0] + normals[index0]) +
					b1 * (points[index1] + normals[index1]) +
					b2 * (points[index2] + normals[index2]) +
					b3 * (points[index3] + normals[index3]);

	float yNorm =	b0 * (points[index0 + 1] + normals[index0 + 1]) +
					b1 * (points[index1 + 1] + normals[index1 + 1]) +
					b2 * (points[index2 + 1] + normals[index2 + 1]) +
					b3 * (points[index3 + 1] + normals[index3 + 1]);

	float zNorm =	b0 * (points[index0 + 2] + normals[index0 + 2]) +
					b1 * (points[index1 + 2] + normals[index1 + 2]) +
					b2 * (points[index2 + 2] + normals[index2 + 2]) +
					b3 * (points[index3 + 2] + normals[index3 + 2]);

	float3 temp = (float3)(x, y, z);
	vstore3(temp, i, splineSegCA);

	temp = (float3)(xNorm, yNorm, zNorm);
	vstore3(temp, i, splineSegO);
}
