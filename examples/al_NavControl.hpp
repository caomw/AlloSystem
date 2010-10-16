#include "io/al_WindowGL.hpp"
#include "spatial/al_CoordinateFrame.hpp"

namespace al {

/// Mapping from keyboard and mouse controls to a Nav object
struct NavInputControl : public InputEventHandler {

	NavInputControl(Nav * nav): mNav(nav){}

	void nav(Nav * v){ mNav=v; }

	bool onKeyDown(const Keyboard& k){	 	
	
		static double a = 2;		// rotational speed: degrees per update
		static double v = 0.125;	// speed: units per update
		
		switch(k.key()){
			case '`':			nav().halt().home(); return false;
			case 's':			nav().halt(); return false;
			case Key::Up:		nav().spinR(-a); return false;
			case Key::Down:		nav().spinR( a); return false;
			case Key::Right:	nav().spinU( a); return false;
			case Key::Left:		nav().spinU(-a); return false;
			case 'q':			nav().spinF( a); return false;
			case 'z':			nav().spinF(-a); return false;
			case 'a':			nav().moveR(-v); return false;
			case 'd':			nav().moveR( v); return false;
			case 'e':			nav().moveU( v); return false;
			case 'c':			nav().moveU(-v); return false;
			case 'x':			nav().moveF(-v); return false;
			case 'w':			nav().moveF( v); return false;
			default:;
		}
		return true;
	}
	bool onKeyUp(const Keyboard& k) {
		switch (k.key()) {
			case Key::Up:
			case Key::Down:		nav().spinR(0); return false;
			case Key::Right:
			case Key::Left:		nav().spinU(0); return false;
			case 'q':
			case 'z':			nav().spinF(0); return false;
			case 'a':
			case 'd':			nav().moveR(0); return false;
			case 'e':
			case 'c':			nav().moveU(0); return false;
			case 'x':
			case 'w':			nav().moveF(0); return false;
			default:;
		}
		return true;
	}

	bool onMouseDrag(const Mouse& m){
		if(m.left()){
			nav().turnU( m.dx() * 0.2);
			nav().turnR( m.dy() * 0.2);
		}
		else if(m.right()){
			nav().turnF(m.dx() * 0.2);
			//incBehind(m.dy()*0.005);
		}
		return false;
	}

	Nav& nav(){ return *mNav; }

protected:
	Nav * mNav;
};


struct NavInputControlCosm : public NavInputControl {

	NavInputControlCosm(Nav * nav): NavInputControl(nav){}

	bool onKeyDown(const Keyboard& k){	 	
	
		static double a = 1;		// rotational speed: degrees per update
		static double v = 0.25;		// speed: units per update
		
		switch(k.key()){
			case '`':			nav().halt().home(); return false;
			case 'w':			nav().spinR(-a); return false;
			case 'x':			nav().spinR( a); return false;
			case Key::Right:	nav().spinU( a); return false;
			case Key::Left:		nav().spinU(-a); return false;
			case 'a':			nav().spinF( a); return false;
			case 'd':			nav().spinF(-a); return false;
			case ',':			nav().moveR(-v); return false;
			case '.':			nav().moveR( v); return false;
			case '\'':			nav().moveU( v); return false;
			case '/':			nav().moveU(-v); return false;
			case Key::Up:		nav().moveF( v); return false;
			case Key::Down:		nav().moveF(-v); return false;
			default:;
		}
		return true;
	}

	bool onKeyUp(const Keyboard& k) {
		switch (k.key()) {
			case 'w':
			case 'x':			nav().spinR(0); return false;
			case Key::Right:
			case Key::Left:		nav().spinU(0); return false;
			case 'a':
			case 'd':			nav().spinF(0); return false;
			case ',':
			case '.':			nav().moveR(0); return false;
			case '\'':
			case '/':			nav().moveU(0); return false;
			case Key::Up:
			case Key::Down:		nav().moveF(0); return false;
			default:;
		}
		return true;
	}
	
	bool onMouseDrag(const Mouse& m){ return true; }
};



} // al::
