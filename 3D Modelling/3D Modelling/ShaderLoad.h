//////////////////////////////////////////////////////////////////////////////
//
//  --- LoadShaders.h ---
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __LOAD_SHADERS_H__
#define __LOAD_SHADERS_H__

#include <GL/glew.h>
#include "lib/glm/glm.hpp"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

	typedef struct
	{
		GLenum       type;
		const char* filename;
		GLuint       shader;
	} ShaderInfo;

	GLuint LoadShaders(ShaderInfo*);

	//----------------------------------------------------------------------------

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __LOAD_SHADERS_H__
