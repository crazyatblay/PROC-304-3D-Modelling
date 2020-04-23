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
\******************************************************************************/
#include "maths_funcs.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

/*--------------------------------CONSTRUCTORS--------------------------------*/
veC2::veC2() {}

veC2::veC2( float x, float y ) {
	v[0] = x;
	v[1] = y;
}

veC3::veC3() {}

veC3::veC3( float x, float y, float z ) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

veC3::veC3( const veC2 &vv, float z ) {
	v[0] = vv.v[0];
	v[1] = vv.v[1];
	v[2] = z;
}

veC3::veC3( const veC4 &vv ) {
	v[0] = vv.v[0];
	v[1] = vv.v[1];
	v[2] = vv.v[2];
}

veC4::veC4() {}

veC4::veC4( float x, float y, float z, float w ) {
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}

veC4::veC4( const veC2 &vv, float z, float w ) {
	v[0] = vv.v[0];
	v[1] = vv.v[1];
	v[2] = z;
	v[3] = w;
}

veC4::veC4( const veC3 &vv, float w ) {
	v[0] = vv.v[0];
	v[1] = vv.v[1];
	v[2] = vv.v[2];
	v[3] = w;
}

maT3::maT3() {}

/* note: entered in COLUMNS */
maT3::maT3( float a, float b, float c, float d, float e, float f, float g, float h,
						float i ) {
	m[0] = a;
	m[1] = b;
	m[2] = c;
	m[3] = d;
	m[4] = e;
	m[5] = f;
	m[6] = g;
	m[7] = h;
	m[8] = i;
}

maT4::maT4() {}

/* note: entered in COLUMNS */
maT4::maT4( float a, float b, float c, float d, float e, float f, float g, float h,
						float i, float j, float k, float l, float mm, float n, float o,
						float p ) {
	m[0] = a;
	m[1] = b;
	m[2] = c;
	m[3] = d;
	m[4] = e;
	m[5] = f;
	m[6] = g;
	m[7] = h;
	m[8] = i;
	m[9] = j;
	m[10] = k;
	m[11] = l;
	m[12] = mm;
	m[13] = n;
	m[14] = o;
	m[15] = p;
}

/*-----------------------------PRINT FUNCTIONS--------------------------------*/
void print( const veC2 &v ) { printf( "[%.2f, %.2f]\n", v.v[0], v.v[1] ); }

void print( const veC3 &v ) {
	printf( "[%.2f, %.2f, %.2f]\n", v.v[0], v.v[1], v.v[2] );
}

void print( const veC4 &v ) {
	printf( "[%.2f, %.2f, %.2f, %.2f]\n", v.v[0], v.v[1], v.v[2], v.v[3] );
}

void print( const maT3 &m ) {
	printf( "\n" );
	printf( "[%.2f][%.2f][%.2f]\n", m.m[0], m.m[3], m.m[6] );
	printf( "[%.2f][%.2f][%.2f]\n", m.m[1], m.m[4], m.m[7] );
	printf( "[%.2f][%.2f][%.2f]\n", m.m[2], m.m[5], m.m[8] );
}

void print( const maT4 &m ) {
	printf( "\n" );
	printf( "[%.2f][%.2f][%.2f][%.2f]\n", m.m[0], m.m[4], m.m[8], m.m[12] );
	printf( "[%.2f][%.2f][%.2f][%.2f]\n", m.m[1], m.m[5], m.m[9], m.m[13] );
	printf( "[%.2f][%.2f][%.2f][%.2f]\n", m.m[2], m.m[6], m.m[10], m.m[14] );
	printf( "[%.2f][%.2f][%.2f][%.2f]\n", m.m[3], m.m[7], m.m[11], m.m[15] );
}

/*------------------------------VECTOR FUNCTIONS------------------------------*/
float length( const veC3 &v ) {
	return sqrt( v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2] );
}

// squared length
float length2( const veC3 &v ) {
	return v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
}

// note: proper spelling (hehe)
veC3 normalise( const veC3 &v ) {
	veC3 vb;
	float l = length( v );
	if ( 0.0f == l ) {
		return veC3( 0.0f, 0.0f, 0.0f );
	}
	vb.v[0] = v.v[0] / l;
	vb.v[1] = v.v[1] / l;
	vb.v[2] = v.v[2] / l;
	return vb;
}

veC3 veC3::operator+( const veC3 &rhs ) {
	veC3 vc;
	vc.v[0] = v[0] + rhs.v[0];
	vc.v[1] = v[1] + rhs.v[1];
	vc.v[2] = v[2] + rhs.v[2];
	return vc;
}

veC3 &veC3::operator+=( const veC3 &rhs ) {
	v[0] += rhs.v[0];
	v[1] += rhs.v[1];
	v[2] += rhs.v[2];
	return *this; // return self
}

veC3 veC3::operator-( const veC3 &rhs ) {
	veC3 vc;
	vc.v[0] = v[0] - rhs.v[0];
	vc.v[1] = v[1] - rhs.v[1];
	vc.v[2] = v[2] - rhs.v[2];
	return vc;
}

veC3 &veC3::operator-=( const veC3 &rhs ) {
	v[0] -= rhs.v[0];
	v[1] -= rhs.v[1];
	v[2] -= rhs.v[2];
	return *this;
}

veC3 veC3::operator+( float rhs ) {
	veC3 vc;
	vc.v[0] = v[0] + rhs;
	vc.v[1] = v[1] + rhs;
	vc.v[2] = v[2] + rhs;
	return vc;
}

veC3 veC3::operator-( float rhs ) {
	veC3 vc;
	vc.v[0] = v[0] - rhs;
	vc.v[1] = v[1] - rhs;
	vc.v[2] = v[2] - rhs;
	return vc;
}

veC3 veC3::operator*( float rhs ) {
	veC3 vc;
	vc.v[0] = v[0] * rhs;
	vc.v[1] = v[1] * rhs;
	vc.v[2] = v[2] * rhs;
	return vc;
}

veC3 veC3::operator/( float rhs ) {
	veC3 vc;
	vc.v[0] = v[0] / rhs;
	vc.v[1] = v[1] / rhs;
	vc.v[2] = v[2] / rhs;
	return vc;
}

veC3 &veC3::operator*=( float rhs ) {
	v[0] = v[0] * rhs;
	v[1] = v[1] * rhs;
	v[2] = v[2] * rhs;
	return *this;
}

veC3 &veC3::operator=( const veC3 &rhs ) {
	v[0] = rhs.v[0];
	v[1] = rhs.v[1];
	v[2] = rhs.v[2];
	return *this;
}

float dot( const veC3 &a, const veC3 &b ) {
	return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
}

veC3 cross( const veC3 &a, const veC3 &b ) {
	float x = a.v[1] * b.v[2] - a.v[2] * b.v[1];
	float y = a.v[2] * b.v[0] - a.v[0] * b.v[2];
	float z = a.v[0] * b.v[1] - a.v[1] * b.v[0];
	return veC3( x, y, z );
}

float get_squared_dist( veC3 from, veC3 to ) {
	float x = ( to.v[0] - from.v[0] ) * ( to.v[0] - from.v[0] );
	float y = ( to.v[1] - from.v[1] ) * ( to.v[1] - from.v[1] );
	float z = ( to.v[2] - from.v[2] ) * ( to.v[2] - from.v[2] );
	return x + y + z;
}

/* converts an un-normalised direction into a heading in degrees
NB i suspect that the z is backwards here but i've used in in
several places like this. d'oh! */
float direction_to_heading( veC3 d ) {
	return atan2( -d.v[0], -d.v[2] ) * ONE_RAD_IN_DEG;
}

veC3 heading_to_direction( float degrees ) {
	float rad = degrees * ONE_DEG_IN_RAD;
	return veC3( -sinf( rad ), 0.0f, -cosf( rad ) );
}

/*-----------------------------MATRIX FUNCTIONS-------------------------------*/
maT3 zero_maT3() {
	return maT3( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
}

maT3 identity_maT3() {
	return maT3( 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f );
}

maT4 zero_maT4() {
	return maT4( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
							 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
}

maT4 identity_maT4() {
	return maT4( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
							 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
}

/* maT4 array layout
 0  4  8 12
 1  5  9 13
 2  6 10 14
 3  7 11 15
*/

veC4 maT4::operator*( const veC4 &rhs ) {
	// 0x + 4y + 8z + 12w
	float x = m[0] * rhs.v[0] + m[4] * rhs.v[1] + m[8] * rhs.v[2] + m[12] * rhs.v[3];
	// 1x + 5y + 9z + 13w
	float y = m[1] * rhs.v[0] + m[5] * rhs.v[1] + m[9] * rhs.v[2] + m[13] * rhs.v[3];
	// 2x + 6y + 10z + 14w
	float z = m[2] * rhs.v[0] + m[6] * rhs.v[1] + m[10] * rhs.v[2] + m[14] * rhs.v[3];
	// 3x + 7y + 11z + 15w
	float w = m[3] * rhs.v[0] + m[7] * rhs.v[1] + m[11] * rhs.v[2] + m[15] * rhs.v[3];
	return veC4( x, y, z, w );
}

maT4 maT4::operator*( const maT4 &rhs ) {
	maT4 r = zero_maT4();
	int r_index = 0;
	for ( int col = 0; col < 4; col++ ) {
		for ( int row = 0; row < 4; row++ ) {
			float sum = 0.0f;
			for ( int i = 0; i < 4; i++ ) {
				sum += rhs.m[i + col * 4] * m[row + i * 4];
			}
			r.m[r_index] = sum;
			r_index++;
		}
	}
	return r;
}

maT4 &maT4::operator=( const maT4 &rhs ) {
	for ( int i = 0; i < 16; i++ ) {
		m[i] = rhs.m[i];
	}
	return *this;
}

// returns a scalar value with the determinant for a 4x4 matrix
// see
// http://www.euclideanspace.com/maths/algebra/matrix/functions/determinant/fourD/index.htm
float determinant( const maT4 &mm ) {
	return mm.m[12] * mm.m[9] * mm.m[6] * mm.m[3] -
				 mm.m[8] * mm.m[13] * mm.m[6] * mm.m[3] -
				 mm.m[12] * mm.m[5] * mm.m[10] * mm.m[3] +
				 mm.m[4] * mm.m[13] * mm.m[10] * mm.m[3] +
				 mm.m[8] * mm.m[5] * mm.m[14] * mm.m[3] -
				 mm.m[4] * mm.m[9] * mm.m[14] * mm.m[3] -
				 mm.m[12] * mm.m[9] * mm.m[2] * mm.m[7] +
				 mm.m[8] * mm.m[13] * mm.m[2] * mm.m[7] +
				 mm.m[12] * mm.m[1] * mm.m[10] * mm.m[7] -
				 mm.m[0] * mm.m[13] * mm.m[10] * mm.m[7] -
				 mm.m[8] * mm.m[1] * mm.m[14] * mm.m[7] +
				 mm.m[0] * mm.m[9] * mm.m[14] * mm.m[7] +
				 mm.m[12] * mm.m[5] * mm.m[2] * mm.m[11] -
				 mm.m[4] * mm.m[13] * mm.m[2] * mm.m[11] -
				 mm.m[12] * mm.m[1] * mm.m[6] * mm.m[11] +
				 mm.m[0] * mm.m[13] * mm.m[6] * mm.m[11] +
				 mm.m[4] * mm.m[1] * mm.m[14] * mm.m[11] -
				 mm.m[0] * mm.m[5] * mm.m[14] * mm.m[11] -
				 mm.m[8] * mm.m[5] * mm.m[2] * mm.m[15] +
				 mm.m[4] * mm.m[9] * mm.m[2] * mm.m[15] +
				 mm.m[8] * mm.m[1] * mm.m[6] * mm.m[15] -
				 mm.m[0] * mm.m[9] * mm.m[6] * mm.m[15] -
				 mm.m[4] * mm.m[1] * mm.m[10] * mm.m[15] +
				 mm.m[0] * mm.m[5] * mm.m[10] * mm.m[15];
}

/* returns a 16-element array that is the inverse of a 16-element array (4x4
matrix). see
http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
*/
maT4 inverse( const maT4 &mm ) {
	float det = determinant( mm );
	/* there is no inverse if determinant is zero (not likely unless scale is
	broken) */
	if ( 0.0f == det ) {
		fprintf( stderr, "WARNING. matrix has no determinant. can not invert\n" );
		return mm;
	}
	float inv_det = 1.0f / det;

	return maT4(
		inv_det * ( mm.m[9] * mm.m[14] * mm.m[7] - mm.m[13] * mm.m[10] * mm.m[7] +
								mm.m[13] * mm.m[6] * mm.m[11] - mm.m[5] * mm.m[14] * mm.m[11] -
								mm.m[9] * mm.m[6] * mm.m[15] + mm.m[5] * mm.m[10] * mm.m[15] ),
		inv_det * ( mm.m[13] * mm.m[10] * mm.m[3] - mm.m[9] * mm.m[14] * mm.m[3] -
								mm.m[13] * mm.m[2] * mm.m[11] + mm.m[1] * mm.m[14] * mm.m[11] +
								mm.m[9] * mm.m[2] * mm.m[15] - mm.m[1] * mm.m[10] * mm.m[15] ),
		inv_det * ( mm.m[5] * mm.m[14] * mm.m[3] - mm.m[13] * mm.m[6] * mm.m[3] +
								mm.m[13] * mm.m[2] * mm.m[7] - mm.m[1] * mm.m[14] * mm.m[7] -
								mm.m[5] * mm.m[2] * mm.m[15] + mm.m[1] * mm.m[6] * mm.m[15] ),
		inv_det * ( mm.m[9] * mm.m[6] * mm.m[3] - mm.m[5] * mm.m[10] * mm.m[3] -
								mm.m[9] * mm.m[2] * mm.m[7] + mm.m[1] * mm.m[10] * mm.m[7] +
								mm.m[5] * mm.m[2] * mm.m[11] - mm.m[1] * mm.m[6] * mm.m[11] ),
		inv_det * ( mm.m[12] * mm.m[10] * mm.m[7] - mm.m[8] * mm.m[14] * mm.m[7] -
								mm.m[12] * mm.m[6] * mm.m[11] + mm.m[4] * mm.m[14] * mm.m[11] +
								mm.m[8] * mm.m[6] * mm.m[15] - mm.m[4] * mm.m[10] * mm.m[15] ),
		inv_det * ( mm.m[8] * mm.m[14] * mm.m[3] - mm.m[12] * mm.m[10] * mm.m[3] +
								mm.m[12] * mm.m[2] * mm.m[11] - mm.m[0] * mm.m[14] * mm.m[11] -
								mm.m[8] * mm.m[2] * mm.m[15] + mm.m[0] * mm.m[10] * mm.m[15] ),
		inv_det * ( mm.m[12] * mm.m[6] * mm.m[3] - mm.m[4] * mm.m[14] * mm.m[3] -
								mm.m[12] * mm.m[2] * mm.m[7] + mm.m[0] * mm.m[14] * mm.m[7] +
								mm.m[4] * mm.m[2] * mm.m[15] - mm.m[0] * mm.m[6] * mm.m[15] ),
		inv_det * ( mm.m[4] * mm.m[10] * mm.m[3] - mm.m[8] * mm.m[6] * mm.m[3] +
								mm.m[8] * mm.m[2] * mm.m[7] - mm.m[0] * mm.m[10] * mm.m[7] -
								mm.m[4] * mm.m[2] * mm.m[11] + mm.m[0] * mm.m[6] * mm.m[11] ),
		inv_det * ( mm.m[8] * mm.m[13] * mm.m[7] - mm.m[12] * mm.m[9] * mm.m[7] +
								mm.m[12] * mm.m[5] * mm.m[11] - mm.m[4] * mm.m[13] * mm.m[11] -
								mm.m[8] * mm.m[5] * mm.m[15] + mm.m[4] * mm.m[9] * mm.m[15] ),
		inv_det * ( mm.m[12] * mm.m[9] * mm.m[3] - mm.m[8] * mm.m[13] * mm.m[3] -
								mm.m[12] * mm.m[1] * mm.m[11] + mm.m[0] * mm.m[13] * mm.m[11] +
								mm.m[8] * mm.m[1] * mm.m[15] - mm.m[0] * mm.m[9] * mm.m[15] ),
		inv_det * ( mm.m[4] * mm.m[13] * mm.m[3] - mm.m[12] * mm.m[5] * mm.m[3] +
								mm.m[12] * mm.m[1] * mm.m[7] - mm.m[0] * mm.m[13] * mm.m[7] -
								mm.m[4] * mm.m[1] * mm.m[15] + mm.m[0] * mm.m[5] * mm.m[15] ),
		inv_det * ( mm.m[8] * mm.m[5] * mm.m[3] - mm.m[4] * mm.m[9] * mm.m[3] -
								mm.m[8] * mm.m[1] * mm.m[7] + mm.m[0] * mm.m[9] * mm.m[7] +
								mm.m[4] * mm.m[1] * mm.m[11] - mm.m[0] * mm.m[5] * mm.m[11] ),
		inv_det * ( mm.m[12] * mm.m[9] * mm.m[6] - mm.m[8] * mm.m[13] * mm.m[6] -
								mm.m[12] * mm.m[5] * mm.m[10] + mm.m[4] * mm.m[13] * mm.m[10] +
								mm.m[8] * mm.m[5] * mm.m[14] - mm.m[4] * mm.m[9] * mm.m[14] ),
		inv_det * ( mm.m[8] * mm.m[13] * mm.m[2] - mm.m[12] * mm.m[9] * mm.m[2] +
								mm.m[12] * mm.m[1] * mm.m[10] - mm.m[0] * mm.m[13] * mm.m[10] -
								mm.m[8] * mm.m[1] * mm.m[14] + mm.m[0] * mm.m[9] * mm.m[14] ),
		inv_det * ( mm.m[12] * mm.m[5] * mm.m[2] - mm.m[4] * mm.m[13] * mm.m[2] -
								mm.m[12] * mm.m[1] * mm.m[6] + mm.m[0] * mm.m[13] * mm.m[6] +
								mm.m[4] * mm.m[1] * mm.m[14] - mm.m[0] * mm.m[5] * mm.m[14] ),
		inv_det * ( mm.m[4] * mm.m[9] * mm.m[2] - mm.m[8] * mm.m[5] * mm.m[2] +
								mm.m[8] * mm.m[1] * mm.m[6] - mm.m[0] * mm.m[9] * mm.m[6] -
								mm.m[4] * mm.m[1] * mm.m[10] + mm.m[0] * mm.m[5] * mm.m[10] ) );
}

// returns a 16-element array flipped on the main diagonal
maT4 transpose( const maT4 &mm ) {
	return maT4( mm.m[0], mm.m[4], mm.m[8], mm.m[12], mm.m[1], mm.m[5], mm.m[9],
							 mm.m[13], mm.m[2], mm.m[6], mm.m[10], mm.m[14], mm.m[3], mm.m[7],
							 mm.m[11], mm.m[15] );
}

/*--------------------------AFFINE MATRIX FUNCTIONS---------------------------*/
// translate a 4d matrix with xyz array
maT4 translate( const maT4 &m, const veC3 &v ) {
	maT4 m_t = identity_maT4();
	m_t.m[12] = v.v[0];
	m_t.m[13] = v.v[1];
	m_t.m[14] = v.v[2];
	return m_t * m;
}

// rotate around x axis by an angle in degrees
maT4 rotate_x_deg( const maT4 &m, float deg ) {
	// convert to radians
	float rad = deg * ONE_DEG_IN_RAD;
	maT4 m_r = identity_maT4();
	m_r.m[5] = cos( rad );
	m_r.m[9] = -sin( rad );
	m_r.m[6] = sin( rad );
	m_r.m[10] = cos( rad );
	return m_r * m;
}

// rotate around y axis by an angle in degrees
maT4 rotate_y_deg( const maT4 &m, float deg ) {
	// convert to radians
	float rad = deg * ONE_DEG_IN_RAD;
	maT4 m_r = identity_maT4();
	m_r.m[0] = cos( rad );
	m_r.m[8] = sin( rad );
	m_r.m[2] = -sin( rad );
	m_r.m[10] = cos( rad );
	return m_r * m;
}

// rotate around z axis by an angle in degrees
maT4 rotate_z_deg( const maT4 &m, float deg ) {
	// convert to radians
	float rad = deg * ONE_DEG_IN_RAD;
	maT4 m_r = identity_maT4();
	m_r.m[0] = cos( rad );
	m_r.m[4] = -sin( rad );
	m_r.m[1] = sin( rad );
	m_r.m[5] = cos( rad );
	return m_r * m;
}

// scale a matrix by [x, y, z]
maT4 scale( const maT4 &m, const veC3 &v ) {
	maT4 a = identity_maT4();
	a.m[0] = v.v[0];
	a.m[5] = v.v[1];
	a.m[10] = v.v[2];
	return a * m;
}

/*-----------------------VIRTUAL CAMERA MATRIX FUNCTIONS----------------------*/
// returns a view matrix using the opengl lookAt style. COLUMN ORDER.
maT4 look_at( const veC3 &cam_pos, veC3 targ_pos, const veC3 &up ) {
	// inverse translation
	maT4 p = identity_maT4();
	p = translate( p, veC3( -cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2] ) );
	// distance vector
	veC3 d = targ_pos - cam_pos;
	// forward vector
	veC3 f = normalise( d );
	// right vector
	veC3 r = normalise( cross( f, up ) );
	// real up vector
	veC3 u = normalise( cross( r, f ) );
	maT4 ori = identity_maT4();
	ori.m[0] = r.v[0];
	ori.m[4] = r.v[1];
	ori.m[8] = r.v[2];
	ori.m[1] = u.v[0];
	ori.m[5] = u.v[1];
	ori.m[9] = u.v[2];
	ori.m[2] = -f.v[0];
	ori.m[6] = -f.v[1];
	ori.m[10] = -f.v[2];

	return ori * p; // p * ori;
}

// returns a perspective function mimicking the opengl projection style.
maT4 perspective( float fovy, float aspect, float near, float far ) {
	float fov_rad = fovy * ONE_DEG_IN_RAD;
	float inverse_range = 1.0f / tan( fov_rad / 2.0f );
	float sx = inverse_range / aspect;
	float sy = inverse_range;
	float sz = -( far + near ) / ( far - near );
	float pz = -( 2.0f * far * near ) / ( far - near );
	maT4 m = zero_maT4(); // make sure bottom-right corner is zero
	m.m[0] = sx;
	m.m[5] = sy;
	m.m[10] = sz;
	m.m[14] = pz;
	m.m[11] = -1.0f;
	return m;
}

/*----------------------------HAMILTON IN DA HOUSE!---------------------------*/
versor::versor() {}

versor versor::operator/( float rhs ) {
	versor result;
	result.q[0] = q[0] / rhs;
	result.q[1] = q[1] / rhs;
	result.q[2] = q[2] / rhs;
	result.q[3] = q[3] / rhs;
	return result;
}

versor versor::operator*( float rhs ) {
	versor result;
	result.q[0] = q[0] * rhs;
	result.q[1] = q[1] * rhs;
	result.q[2] = q[2] * rhs;
	result.q[3] = q[3] * rhs;
	return result;
}

void print( const versor &q ) {
	printf( "[%.2f ,%.2f, %.2f, %.2f]\n", q.q[0], q.q[1], q.q[2], q.q[3] );
}

versor versor::operator*( const versor &rhs ) {
	versor result;
	result.q[0] =
		rhs.q[0] * q[0] - rhs.q[1] * q[1] - rhs.q[2] * q[2] - rhs.q[3] * q[3];
	result.q[1] =
		rhs.q[0] * q[1] + rhs.q[1] * q[0] - rhs.q[2] * q[3] + rhs.q[3] * q[2];
	result.q[2] =
		rhs.q[0] * q[2] + rhs.q[1] * q[3] + rhs.q[2] * q[0] - rhs.q[3] * q[1];
	result.q[3] =
		rhs.q[0] * q[3] - rhs.q[1] * q[2] + rhs.q[2] * q[1] + rhs.q[3] * q[0];
	// re-normalise in case of mangling
	return normalise( result );
}

versor versor::operator+( const versor &rhs ) {
	versor result;
	result.q[0] = rhs.q[0] + q[0];
	result.q[1] = rhs.q[1] + q[1];
	result.q[2] = rhs.q[2] + q[2];
	result.q[3] = rhs.q[3] + q[3];
	// re-normalise in case of mangling
	return normalise( result );
}

versor quat_from_axis_rad( float radians, float x, float y, float z ) {
	versor result;
	result.q[0] = cos( radians / 2.0 );
	result.q[1] = sin( radians / 2.0 ) * x;
	result.q[2] = sin( radians / 2.0 ) * y;
	result.q[3] = sin( radians / 2.0 ) * z;
	return result;
}

versor quat_from_axis_deg( float degrees, float x, float y, float z ) {
	return quat_from_axis_rad( ONE_DEG_IN_RAD * degrees, x, y, z );
}

maT4 quat_to_maT4( const versor &q ) {
	float w = q.q[0];
	float x = q.q[1];
	float y = q.q[2];
	float z = q.q[3];
	return maT4( 1.0f - 2.0f * y * y - 2.0f * z * z, 2.0f * x * y + 2.0f * w * z,
							 2.0f * x * z - 2.0f * w * y, 0.0f, 2.0f * x * y - 2.0f * w * z,
							 1.0f - 2.0f * x * x - 2.0f * z * z, 2.0f * y * z + 2.0f * w * x,
							 0.0f, 2.0f * x * z + 2.0f * w * y, 2.0f * y * z - 2.0f * w * x,
							 1.0f - 2.0f * x * x - 2.0f * y * y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );
}

versor normalise( versor &q ) {
	// norm(q) = q / magnitude (q)
	// magnitude (q) = sqrt (w*w + x*x...)
	// only compute sqrt if interior sum != 1.0
	float sum = q.q[0] * q.q[0] + q.q[1] * q.q[1] + q.q[2] * q.q[2] + q.q[3] * q.q[3];
	// NB: floats have min 6 digits of precision
	const float thresh = 0.0001f;
	if ( fabs( 1.0f - sum ) < thresh ) {
		return q;
	}
	float mag = sqrt( sum );
	return q / mag;
}

float dot( const versor &q, const versor &r ) {
	return q.q[0] * r.q[0] + q.q[1] * r.q[1] + q.q[2] * r.q[2] + q.q[3] * r.q[3];
}

versor slerp( versor &q, versor &r, float t ) {
	// angle between q0-q1
	float cos_half_theta = dot( q, r );
	// as found here
	// http://stackoverflow.com/questions/2886606/flipping-issue-when-interpolating-rotations-using-quaternions
	// if dot product is negative then one quaternion should be negated, to make
	// it take the short way around, rather than the long way
	// yeah! and furthermore Susan, I had to recalculate the d.p. after this
	if ( cos_half_theta < 0.0f ) {
		for ( int i = 0; i < 4; i++ ) {
			q.q[i] *= -1.0f;
		}
		cos_half_theta = dot( q, r );
	}
	// if qa=qb or qa=-qb then theta = 0 and we can return qa
	if ( fabs( cos_half_theta ) >= 1.0f ) {
		return q;
	}
	// Calculate temporary values
	float sin_half_theta = sqrt( 1.0f - cos_half_theta * cos_half_theta );
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to qa or qb
	versor result;
	if ( fabs( sin_half_theta ) < 0.001f ) {
		for ( int i = 0; i < 4; i++ ) {
			result.q[i] = ( 1.0f - t ) * q.q[i] + t * r.q[i];
		}
		return result;
	}
	float half_theta = acos( cos_half_theta );
	float a = sin( ( 1.0f - t ) * half_theta ) / sin_half_theta;
	float b = sin( t * half_theta ) / sin_half_theta;
	for ( int i = 0; i < 4; i++ ) {
		result.q[i] = q.q[i] * a + r.q[i] * b;
	}
	return result;
}
