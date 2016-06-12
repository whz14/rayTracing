#include "raytracer.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
using namespace std;

void Engine::render() {
	cout << "holy shit\n";
	vec3 camPosi(0, 0, -5);
	for(; presentY < screen.row() - 20; ++presentY) {
		int start = GetTickCount();
		for(; presentX < screen.col(); ++presentX) {	// for each pixel in the screen
			if(presentX == 640 && presentY == 570) {
				for(int fuck = 0; fuck < 1e9; ++fuck);
					//cout << "fuck\n";
			}
			vec3 scrPoint(SCRX1 + DX * presentX, SCRY1 + DY * presentY, 0);
			Ray ray(camPosi, (scrPoint - camPosi).normlize());
			Color acc(0, 0, 0);
			double dist = INF;
			rayTrace(ray, acc, 1, 0, dist);
			//cout << acc << endl;
			assign(screen.at(presentY, presentX), acc);
			//cout << presentX << ", " << presentY << "\t" << screen.col() << endl;
		}
		presentX = 0;
		
		//cout << presentY << "wocaonimalegebi\n";
		//if(presentY % 5 == 0)
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

Primitive * Engine::rayTrace(Ray & ray, Color & resCol, int depth, double index, double & dist) {
	/*resCol = Color(1, 1, 1);
	return NULL;*/
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
	//else {						// intersected with sth else
	poi = ray.getOri() + dist*ray.getDir();	// defuse and Phong
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
	
	double refl = pri->getMaterial()->getRefl();	// reflection
	if(refl > EPS && depth < TRACEDEPTH) {
		vec3 v = ray.getDir(), n = pri->getNorm(poi);
		vec3 r = v - 2 * v.dotPro(n) * n;
		Color refCol(0, 0, 0);
		double dis;
		rayTrace(Ray(poi + EPS*r, r), refCol, depth + 1, index, dis);
		resCol += refl * refCol * pri->getMaterial()->getColor();
	}


	return pri;
}
