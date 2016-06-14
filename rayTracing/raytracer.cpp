#include "raytracer.h"
#include "config.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
using namespace std;

void Engine::render() {
	cout << "holy shit\n";
	vec3 camPosi(0, 0, -5);
	Primitive* lastPrim = NULL;
	for(; presentY < screen.row() - 20; ++presentY) {
		int start = GetTickCount();
		for(; presentX < screen.col(); ++presentX) {	// for each pixel in the screen
			/*if(presentX == 270 && presentY == 540) {
				cout << "fuck\n";
				for(int fuck = 0; fuck < 1e9; ++fuck);
			}*/
			Color acc(0, 0, 0);
			vec3 scrPoint(SCRX1 + DX * (presentX), SCRY1 + DY * (presentY), 0);
			Ray ray(camPosi, (scrPoint - camPosi).normlize());
			double dist = INF;
			Primitive* thisPrim = rayTrace(ray, acc, 1, 1, dist);	// depth is 1, refraction index is 1, i.e. air}
			
			if(lastPrim != thisPrim) {
				lastPrim = thisPrim;
				acc = Color(0, 0, 0);
				for(int i = 0; i < SAMPLENUM; ++i) {
					for(int j = 0; j < SAMPLENUM; ++j) {
						vec3 scrPoint(SCRX1 + DX * (presentX + i *DX / SAMPLENUM), SCRY1 + DY * (presentY + j*DY / SAMPLENUM), 0);
						Ray ray(camPosi, (scrPoint - camPosi).normlize());
						double dist = INF;
						rayTrace(ray, acc, 1, 1, dist);	// depth is 1, refraction index is 1, i.e. air
					}
				}
				acc /= SAMPLENUM*SAMPLENUM;
			}
			//cout << acc << endl;
			assign(screen.at(presentY, presentX), acc);
			//cout << presentX << ", " << presentY << "\t" << screen.col() << endl;
		}
		presentX = 0;
		
		//cout << presentY << "wocaonimalegebi\n";
		if(presentY % 10 == 0)
		screen.show("real time render");
	}
}

void Engine::initRender() {
	// the start position
	presentX = 0, presentY = 20;
	// screen position
	SCRX1 = -4, SCRX2 = 4;
	SCRY1 = 3, SCRY2 = -3;
	//deltaX and deltaY;
	DX = (SCRX2 - SCRX1) / screen.col();
	DY = (SCRY2 - SCRY1) / screen.row();
	// init the scene. containing spheres, plane and lights
	scene->init();
}

Primitive * Engine::rayTrace(Ray & ray, Color & resCol, int depth, double refrIndex, double & dist) {
	if(depth > TRACEDEPTH) return NULL;
	dist = INF;
	Primitive* pri = NULL;
	vec3 poi;	// point of intersection
	int hitOrNot = 2;
	for(int i = 0; i < scene->getNum(); ++i) {
		Primitive* pr = scene->getPri(i);
		if(int res = pr->intersect(ray, dist)) {
			pri = pr;
			hitOrNot = res;
		}
	}
	if(pri == NULL) {			// no intersection
		return pri;
	}
	if(pri->IsLight()) {		// intersected with light
		resCol = Color(1, 1, 1);
		return pri;
	}
	
	//else: intersected with sth else
	poi = ray.getOri() + dist*ray.getDir();

	// defuse and Phong spectecular
	for(int i = 0; i < scene->getNum(); ++i) {
		if(scene->getPri(i)->IsLight()) {
		//  if it is light, calculate its defuse, shade and phong contribution to the pixel on the screen
			Primitive* light = scene->getPri(i);
			double shade = 1.0;
			// shading
			if(light->getType() == Primitive::SPHERE) {
				vec3 l = ((sphere*)light)->getCenter() - poi;
				double ldist = l.length();
				l /= ldist;
				Ray ray1(poi + EPS*l, l);
				for(int j = 0; j < scene->getNum(); ++j) {
					Primitive* pr = scene->getPri(j);
					if(!(pr->IsLight()) && pr->intersect(ray1, ldist)) {
						shade = 0;
						break;
					}
				}
			}
			
			vec3 n = pri->getNorm(poi);						// normal of the primitive
			vec3 v = ray.getDir();							// direction of the ray
			vec3 l = ((sphere*)light)->getCenter() - poi;	// pointing to the light
			l.normlize();
			vec3 r = l - 2 * l.dotPro(n) * n;				// reflection of the vec3 'l'
			
			
			// contribution of defuse reflection
			if(shade && pri->getMaterial()->getDiff() > 0) {			
				double dot = n.dotPro(l);
				if(dot > EPS) {	// we can see the light's defuse
					resCol += shade * (dot * pri->getMaterial()->getDiff()) * pri->getMaterial()->getColor() * light->getMaterial()->getColor();
				}
			}
			// phong model. spec
			if(shade > EPS && pri->getMaterial()->getSpec() > 0) {	
				double dot = v.dotPro(r);
				if(dot > 0) {
					double spec = pow(dot, 20) * pri->getMaterial()->getSpec() * shade;
					resCol += spec * light->getMaterial()->getColor();
				}
			}
		}
	}

	// refraction
	double refr = pri->getMaterial()->getRefr();
	if(refr > EPS && depth < TRACEDEPTH) {
		double n2 = pri->getMaterial()->getRefrInd();
		double n21 = n2 / refrIndex;
		assert(hitOrNot != 0);
		vec3 n = hitOrNot * pri->getNorm(poi);
		double cosI = -ray.getDir().dotPro(n);
		double cosT_2 = 1 - (1 - cosI*cosI) / (n21*n21);
		if(cosT_2 > 0) {	// no total reflection
			//vec3 t = ray.getDir() / n21 - (cosI / n21 + sqrt(cosT_2)) * n;// WTF?
			vec3 t = ray.getDir() / n21 + (cosI / n21 - sqrt(cosT_2)) * n;
			Color refrCol(0, 0, 0);
			double dist;
			assert(t.dotPro(n) <= 0);
			rayTrace(Ray(poi + 10*EPS*t, t), refrCol, depth + 1, hitOrNot > 0 ? n2 : 1, dist);
			Color trans(1, 1, 1);
#ifdef BEER_LAW
			Color absorbance = -0.15 * dist * pri->getMaterial()->getColor();
			trans = Color(exp(absorbance.r), exp(absorbance.g), exp(absorbance.b));
#endif
			resCol += /*refr */refrCol * trans;
		}
	}

	// reflection
	double refl = pri->getMaterial()->getRefl();
	if(refl > EPS && depth < TRACEDEPTH) {
		vec3 v = ray.getDir(), n = pri->getNorm(poi);
		vec3 r = v - 2 * v.dotPro(n) * n;
		Color refCol(0, 0, 0);
		double dis;
		rayTrace(Ray(poi + EPS*r, r), refCol, depth + 1, refrIndex, dis);
		resCol += refl * refCol * pri->getMaterial()->getColor();
	}
	return pri;
}
