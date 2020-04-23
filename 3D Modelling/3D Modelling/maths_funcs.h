/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries' separate legal notices                             |
|******************************************************************************|
| Commonly-used maths structures and functions                                 |
| Simple-as-possible. No disgusting templates.                                 |
| Structs veC3, maT4, versor. just hold arrays of floats called "v","m","q",   |
| respectively. So, for example, to get values from a maT4 do: my_mat.m        |
| A versor is the proper name for a unit quaternion.                           |
| This is C++ because it's sort-of convenient to be able to use maths operators|
\******************************************************************************/
#ifndef _MATHS_FUNCS_H_
#define _MATHS_FUNCS_H_

// const used to convert degrees into radians
#define TAU 2.0 * M_PI
#define ONE_DEG_IN_RAD ( 2.0 * M_PI ) / 360.0 // 0.017444444
#define ONE_RAD_IN_DEG 360.0 / ( 2.0 * M_PI ) // 57.2957795

struct veC2;
struct veC3;
struct veC4;
struct versor;


struct veC2 {
	veC2();
	veC2( float x, float y );
	float v[2];
};

struct veC3 {
	veC3();
	// create from 3 scalars
	veC3( float x, float y, float z );
	// create from veC2 and a scalar
	veC3( const veC2 &vv, float z );
	// create from truncated veC4
	veC3( const veC4 &vv );
	// add vector to vector
	veC3 operator+( const veC3 &rhs );
	// add scalar to vector
	veC3 operator+( float rhs );
	// because user's expect this too
	veC3 &operator+=( const veC3 &rhs );
	// subtract vector from vector
	veC3 operator-( const veC3 &rhs );
	// add vector to vector
	veC3 operator-( float rhs );
	// because users expect this too
	veC3 &operator-=( const veC3 &rhs );
	// multiply with scalar
	veC3 operator*( float rhs );
	// because users expect this too
	veC3 &operator*=( float rhs );
	// divide vector by scalar
	veC3 operator/( float rhs );
	// because users expect this too
	veC3 &operator=( const veC3 &rhs );

	// internal data
	float v[3];
};

struct veC4 {
	veC4();
	veC4( float x, float y, float z, float w );
	veC4( const veC2 &vv, float z, float w );
	veC4( const veC3 &vv, float w );
	float v[4];
};

/* stored like this:
a d g
b e h
c f i */
struct maT3 {
	maT3();
	maT3( float a, float b, float c, float d, float e, float f, float g, float h,
				float i );
	float m[9];
};

/* stored like this:
0 4 8  12
1 5 9  13
2 6 10 14
3 7 11 15*/
struct maT4 {
	maT4();
	// note! this is entering components in ROW-major order
	maT4( float a, float b, float c, float d, float e, float f, float g, float h,
				float i, float j, float k, float l, float mm, float n, float o, float p );
	veC4 operator*( const veC4 &rhs );
	maT4 operator*( const maT4 &rhs );
	maT4 &operator=( const maT4 &rhs );
	float m[16];
};

struct versor {
	versor();
	versor operator/( float rhs );
	versor operator*( float rhs );
	versor operator*( const versor &rhs );
	versor operator+( const versor &rhs );
	float q[4];
};

void print( const veC2 &v );
void print( const veC3 &v );
void print( const veC4 &v );
void print( const maT3 &m );
void print( const maT4 &m );
// vector functions
float length( const veC3 &v );
float length2( const veC3 &v );
veC3 normalise( const veC3 &v );
float dot( const veC3 &a, const veC3 &b );
veC3 cross( const veC3 &a, const veC3 &b );
float get_squared_dist( veC3 from, veC3 to );
float direction_to_heading( veC3 d );
veC3 heading_to_direction( float degrees );
// matrix functions
maT3 zero_maT3();
maT3 identity_maT3();
maT4 zero_maT4();
maT4 identity_maT4();
float determinant( const maT4 &mm );
maT4 inverse( const maT4 &mm );
maT4 transpose( const maT4 &mm );
// affine functions
maT4 translate( const maT4 &m, const veC3 &v );
maT4 rotate_x_deg( const maT4 &m, float deg );
maT4 rotate_y_deg( const maT4 &m, float deg );
maT4 rotate_z_deg( const maT4 &m, float deg );
maT4 scale( const maT4 &m, const veC3 &v );
// camera functions
maT4 look_at( const veC3 &cam_pos, veC3 targ_pos, const veC3 &up );
maT4 perspective( float fovy, float aspect, float near, float far );
// quaternion functions
versor quat_from_axis_rad( float radians, float x, float y, float z );
versor quat_from_axis_deg( float degrees, float x, float y, float z );
maT4 quat_to_maT4( const versor &q );
float dot( const versor &q, const versor &r );
versor slerp( const versor &q, const versor &r );
// stupid overloading wouldn't let me use const
versor normalise( versor &q );
void print( const versor &q );
versor slerp( versor &q, versor &r, float t );
#endif
