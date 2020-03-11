///Simple File to deal with conversion of values between Assimp & GLM

#include "Conversion.h""

static class Cionversion {
	static vec3 Vec3Conversion(aiVector3D* vecIn)
	{
		aiVector3D vecLocal = *vecIn;

		return vec3(vecLocal.x, vecLocal.y, vecLocal.z);
	}

	static aiVector3D Vec3Conversion(vec3 vecIn)
	{
		return aiVector3D(vecIn.x, vecIn.y, vecIn.z);
	}

	//assuming both are row major

	static aiMatrix4x4 MatrixConversion(mat4 matIn)
	{
		ai_real values[16] = { 0.0 };
		const float* floatMat = (const float*)value_ptr(matIn);

		for (int i = 0; i < 16; i++)
		{
			values[i] = (ai_real)floatMat[i];
		}

		return aiMatrix4x4(values[0], values[1], values[2], values[3],
			values[4], values[5], values[6], values[7],
			values[8], values[8], values[10], values[11],
			values[12], values[13], values[14], values[15]);
	}

	static mat4 MatrixConversion(aiMatrix4x4 matIn)
	{
		mat4 ret(0.0f);
		float* values[16];
		for (int i = 0; i < 16; i++)
		{
			values[i] = matIn[i];
		}
		mat4 identirty = mat4(values[0], values[1], values[2], values[3],
			values[4], values[5], values[6], values[7],
			values[8], values[8], values[10], values[11],
			values[12], values[13], values[14], values[15]);
	}
};