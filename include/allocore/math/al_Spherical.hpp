#ifndef INCLUDE_AL_SPHERICAL_HPP
#define INCLUDE_AL_SPHERICAL_HPP

/*	Allocore --
	Multimedia / virtual environment application class library

	Copyright (C) 2009. AlloSphere Research Group, Media Arts & Technology, UCSB.
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


	File description:
	A collection of classes related to spherical geometry

	File author(s):
	Lance Putnam, 2011, putnam.lance@gmail.com
*/


#include "allocore/math/al_Complex.hpp"
#include "allocore/math/al_Functions.hpp"
#include "allocore/math/al_Vec.hpp"

namespace al{


/// Spherical coordinate in terms of two complex numbers

/// The first component, theta, is the angle on the x-y plane and the second
/// component, phi, is the angle from the +z axis. The magnitude of theta
/// should always be 1, while the magnitude of phi is the radius.
template <class T>
struct SphereCoord {
	typedef Complex<T> C;

	C t;	///< Theta component, longitudinal angle (angle from +x towards +y)
	C p;	///< Phi component, latitudinal angle (angle from +z axis)

	///
	SphereCoord(const C& theta =C(1,0), const C& phi =C(1,0))
	:	t(theta), p(phi){}

	/// @param[in] v	Cartesian position
	template <class U>
	SphereCoord(const Vec<3,U>& v){ fromCart(v); }


	/// Get negation in Cartesian space
	SphereCoord operator - () const { return SphereCoord(t, -p); }

	/// Get radius
	T radius() const { return p.mag(); }
	
	/// Returns Cartesian coordinate
	Vec<3,T> toCart() const{
		return Vec<3,T>(t.r*p.i, t.i*p.i, p.r);
	}
	
	/// Set from two angles, in radians, and radius
	
	/// @param[in] theta	longitudinal angle (angle from +x towards +y)
	/// @param[in] phi		latitudinal angle (angle from +z axis)
	SphereCoord& fromAngle(const T& theta, const T& phi, const T& radius =T(1)){
		t.fromPolar(theta);
		p.fromPolar(radius, phi);
		return *this;
	}

	/// Set from Cartesian coordinate
	template <class U>
	SphereCoord& fromCart(const Vec<3,U>& v){
		t(v[0], v[1]);
		T tmag = t.mag();
		p(v[2], tmag);
		tmag != 0 ? t*=(1./tmag) : t(1,0);
		return *this;
	}
};

typedef SphereCoord<float> SphereCoordf;
typedef SphereCoord<double> SphereCoordd;




/// Spherical harmonic evaluator using cached coefficients

/// Spherical harmonics are solutions to Laplace's differential equation on the
/// surface of a 2-sphere. The solutions are complex functions parameterized by
/// two integers, l and m, and two angles defining an orientation in space.
/// The l number determines the number of nodal lines (circles with zero 
/// magnitude) and m determines the number of latitudinal nodal lines
/// (geodesics intersecting the z axis). When |m| = l, the harmonics are "beach
/// ball"-like (sectoral) and when m = 0, the harmonics are "target"-like
/// (zonal). Other values of m produce a checkerboard pattern (tesseral).
template <int L_MAX=16>
class SphericalHarmonic{
public:

	SphericalHarmonic(){
		createLUT();
	}

	/// @param[in] l		number of nodal lines
	/// @param[in] m		number of latitudinal nodal lines, |m| <= l
	/// @param[in] ctheta	longitudinal complex angle in [0, 2pi]
	/// @param[in] cphi		latitudinal complex angle in [0, pi]
	template <class T>
	Complex<T> operator()(int l, int m, const Complex<T>& ctheta, const Complex<T>& cphi) const {
		return coef(l,m) * al::legendreP(l, al::abs(m), cphi.r, cphi.i) * expim(m, ctheta);
	}

	template <class T>
	static Complex<T> expim(int m, const Complex<T>& ctheta){
		Complex<T> res(1, 0);

		// compute e^im recursively
		// TODO: this could be turned into a table lookup
		for(int i=0; i<al::abs(m); ++i){
			res *= ctheta;
		}
		
		if(m < 0) res.i = -res.i;
		return res;
	}
	
	/// Get normalization coefficient
	static double coef(int l, int m){ return l<=L_MAX ? coefTab(l,m) : coefCalc(l,m); }
	
	/// Get normalization coefficient (tabulated)
	static const double& coefTab(int l, int m){ return LUT(l,m); }

	/// Get normalization coefficient (calculated)
	static double coefCalc(int l, int m){
		int M = al::abs(m);
		double res = ::sqrt((2*l + 1) / M_4PI) * al::factorialSqrt(l-M) / al::factorialSqrt(l+M);
		return (M<0 && al::odd(M)) ? -res : res;
	}

private:
	// this holds precomputed coefficients for each basis
	static double& LUT(int l, int m){
		static double t[L_MAX+1][L_MAX*2+1];
		return t[l][m+L_MAX];
	}

	static void createLUT(){
	
		static bool make=true;

		if(make){
			make=false;
			for(int l=0; l<=L_MAX; ++l){
				for(int m=-L_MAX; m<=L_MAX; ++m){					
					double c=0;
					 // m must be in [-l,l]
					if(al::abs(m) <= l)	c = coefCalc(l,m);
					LUT(l, m) = c;
				}
			}
		}
	}

};


/// Spherical harmonic function
static SphericalHarmonic<> spharm;


/// Stereographic projection from an n-sphere to an n-1 dimensional hyperplane

/// \tparam N		dimensions of sphere
/// \tparam T		element type
/// @param[in] v	unit n-vector describing point on n-sphere
/// \returns		vector describing projected coordinate on n-1 hyperplane
template <int N, class T>
inline Vec<N-1,T> sterProj(const Vec<N,T>& v){	
	return sub<N-1>(v) * (T(1)/v[N-1]);
}


} // ::al

#endif
