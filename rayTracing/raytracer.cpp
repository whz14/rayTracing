#include "raytracer.h"
#include "config.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
using namespace std;

void Engine::render() {
	cout << "holy shit\n";
	//vec3 camPosi(0, 0, -5);
	Primitive* lastPrim = NULL;
	DWORD s_time = GetTickCount();
	for(; presentY < screen.row() - 20; ++presentY) {
		//int start = GetTickCount();
		//cout << presentY << endl;
		for(; presentX < screen.col(); ++presentX) {	// for each pixel in the screen
			/*if(presentX == 400 && presentY == 310) {
				for(int fuck = 0; fuck < 30000; ++fuck)
					cout << "fuck\n";
			}*/
			Color acc(0, 0, 0);
			vec3 scrPoint(SCRX1 + DX * (presentX), SCRY1 + DY * (presentY), 0);
			//Ray ray(camPosi, (scrPoint - camPosi).normlize());
			double dist = INF;
			Primitive* thisPrim = renderRay(scrPoint, acc);
			//Primitive* thisPrim = rayTrace(ray, acc, 1, 1, dist);	// depth is 1, refraction index is 1, i.e. air}
			
			if(lastPrim != thisPrim) {
				lastPrim = thisPrim;
				acc = Color(0, 0, 0);
				for(int i = 0; i < SUPERSAMPLENUM; ++i) {
					for(int j = 0; j < SUPERSAMPLENUM; ++j) {
						vec3 scrPoint1(SCRX1 + DX * (presentX + i *DX / SUPERSAMPLENUM), SCRY1 + DY * (presentY + j*DY / SUPERSAMPLENUM), 0);
						//Ray ray(camPosi, (scrPoint - camPosi).normlize());
						double dist = INF;
						renderRay(scrPoint1, acc);
						//rayTrace(ray, acc, 1, 1, dist);	// depth is 1, refraction index is 1, i.e. air
					}
				}
				acc /= SUPERSAMPLENUM*SUPERSAMPLENUM;
			}
			//cout << acc << endl;
			assign(screen.at(presentY, presentX), acc);
			//cout << presentX << ", " << presentY << "\t" << screen.col() << endl;
		}
		presentX = 0;
		
		//cout << presentY << "wocaonimalegebi\n";
#ifdef REALTIMERENDER
		if(presentY % 10 == 0)
		screen.show("real time render");
#endif
	}
	cout << "ttl time" << (GetTickCount() - s_time)*1.0/1000 << endl;
}

int Engine::findNearest(Ray & ray, double & dist, Primitive *& prim) {
	int retval = MISS;
	vec3 raydir, curpos;
	box e = scene->getExtends();
	curpos = ray.getOri();
	raydir = ray.getDir();
	// setup 3DDDA (double check reusability of primary ray data)
	vec3 cb, tmax, tdelta, cell;
	// m_SR: for unit V, how much/many cell[s]?
	// cell: a diag of a cell pointing from e.posi to ray.origin. unit: how many grids
	cell = (curpos - e.getPosi()) * SR;
	int stepX, outX, X = (int)cell.x;
	int stepY, outY, Y = (int)cell.y;
	int stepZ, outZ, Z = (int)cell.z;
	if((X < 0) || (X >= GRIDNUM) || (Y < 0) || (Y >= GRIDNUM) || (Z < 0) || (Z >= GRIDNUM))	// ray origin out of the scene 
		return 0;
	if(raydir.x > 0) {
		stepX = 1, outX = GRIDNUM;
		cb.x = e.getPosi().x + (X + 1) * CW.x;
	}
	else {
		stepX = -1, outX = -1;
		cb.x = e.getPosi().x + X * CW.x;
	}
	if(raydir.y > 0.0f) {
		stepY = 1, outY = GRIDNUM;
		cb.y = e.getPosi().y + (Y + 1) * CW.y;
	}
	else {
		stepY = -1, outY = -1;
		cb.y = e.getPosi().y + Y * CW.y;
	}
	if(raydir.z > 0.0f) {
		stepZ = 1, outZ = GRIDNUM;
		cb.z = e.getPosi().z + (Z + 1) * CW.z;
	}
	else {
		stepZ = -1, outZ = -1;
		cb.z = e.getPosi().z + Z * CW.z;
	}

	double rxr, ryr, rzr;
	if(raydir.x != 0) {
		rxr = 1.0f / raydir.x;
		tmax.x = (cb.x - curpos.x) * rxr;
		tdelta.x = CW.x * stepX * rxr;
	}
	else tmax.x = 1000000;
	if(raydir.y != 0) {
		ryr = 1.0f / raydir.y;
		tmax.y = (cb.y - curpos.y) * ryr;
		tdelta.y = CW.y * stepY * ryr;
	}
	else tmax.y = 1000000;
	if(raydir.z != 0) {
		rzr = 1.0f / raydir.z;
		tmax.z = (cb.z - curpos.z) * rzr;
		tdelta.z = CW.z * stepZ * rzr;
	}
	else tmax.z = 1000000;
	// start stepping
	PriList* list = 0;
	PriList** grid = scene->getGrids();
	prim = 0;
	// trace primary ray
	for(int cnt = 0; ;cnt++) {
		list = grid[X + (Y << GRIDSHFT) + (Z << (GRIDSHFT * 2))];
		while(list) {
			//cout << "cnt is" << cnt << endl;
			//cout << list << endl;
			Primitive* pr = list->getPri();
			int result;
			/*if(pr->GetLastRayID() != ray.GetID())*/ if(result = pr->intersect(ray, dist)) {
				retval = result;
				prim = pr;
				goto testloop;
			}
			list = list->getNext();
		}
		if(tmax.x < tmax.y) {
			if(tmax.x < tmax.z) {
				X = X + stepX;
				if(X == outX) 
					return MISS;
				tmax.x += tdelta.x;
			}
			else {
				Z = Z + stepZ;
				if(Z == outZ)
					return MISS;
				tmax.z += tdelta.z;
			}
		}
		else {
			if(tmax.y < tmax.z) {
				Y = Y + stepY;
				if(Y == outY)
					return MISS;
				tmax.y += tdelta.y;
			}
			else {
				Z = Z + stepZ;
				if(Z == outZ) 
					return MISS;
				tmax.z += tdelta.z;
			}
		}
	}
testloop:
	while(1) {
		list = grid[X + (Y << GRIDSHFT) + (Z << (GRIDSHFT * 2))];
		while(list) {
			Primitive* pr = list->getPri();
			int result;
			/*if(pr->GetLastRayID() != ray.GetID())*/ if(result = pr->intersect(ray, dist)) {
				prim = pr;
				retval = result;
			}
			list = list->getNext();
		}
		if(tmax.x < tmax.y) {
			if(tmax.x < tmax.z) {
				if(dist < tmax.x) break;
				X = X + stepX;
				if(X == outX) break;
				tmax.x += tdelta.x;
			}
			else {
				if(dist < tmax.z) break;
				Z = Z + stepZ;
				if(Z == outZ) break;
				tmax.z += tdelta.z;
			}
		}
		else {
			if(tmax.y < tmax.z) {
				if(dist < tmax.y) break;
				Y = Y + stepY;
				if(Y == outY) break;
				tmax.y += tdelta.y;
			}
			else {
				if(dist < tmax.z) break;
				Z = Z + stepZ;
				if(Z == outZ) break;
				tmax.z += tdelta.z;
			}
		}
	}
	//if(retval != 0) {
		//cout << "hehehehe\n";
	//}
	return retval;
	//return 0;
}

Primitive* Engine::renderRay(const vec3 & scrPnt, Color& col) {
	vec3 camPosi(0, 0, -5);
	vec3 dir = (scrPnt - camPosi).normlize();
	Ray ray(camPosi, dir);
	box e = scene->getExtends();

	if(!e.contains(camPosi)) {
		cout << "really not contained in Engin::renderRay\n";
		double infDist = INF;
		e.intersect(ray, infDist);
		ray.getOri() = camPosi + (infDist + EPS) * dir;
	}
	double dist;
	return rayTrace(ray, col, 1, 1, dist);
}

void Engine::initRender() {
	// init the scene. containing spheres, plane and lights
	scene->init();
	// init SR and CW after scene is inited, how much grids unit volumn take and how much volumn a grid take. in diag form
	SR = GRIDNUM / scene->getExtends().getSize();
	CW = scene->getExtends().getSize() / GRIDNUM;
	// the start position
	presentX = 0, presentY = 20;
	// screen position
	SCRX1 = -4, SCRX2 = 4;
	SCRY1 = 3, SCRY2 = -3;
	//deltaX and deltaY;
	DX = (SCRX2 - SCRX1) / screen.col();
	DY = (SCRY2 - SCRY1) / screen.row();
}

double Engine::calcShade(Primitive * light, const vec3 & poi, vec3 & dir) {
	double shade;
	if(light->getType() == Primitive::SPHERE) {
		shade = 1;
		vec3 l = ((sphere*)light)->getCenter() - poi;
		double ldist = l.length();
		l /= ldist;
		Ray ray1(poi + EPS*l, l);
		Primitive* tmppri = NULL;
		findNearest(ray1, ldist, tmppri);
		if(light != tmppri) {	// blocked with sth else
			//continue;
			shade = 0;
		}
	}
	else if(light->getType() == Primitive::AABBBOX) {
		shade = 0;
		box* blight = (box*)light;
		dir = (blight->getPosi() + 0.5 * blight->getSize() - poi).normlize();
		double dx = blight->getSize().x * 0.25, dz = blight->getSize().z * 0.25;
		for(int i = 0; i < SAMPLENUM; ++i) {
			vec3 d1(blight->gridX(i % 16) + dx * rand1(1) - poi.x, blight->getPosi().y - poi.y, blight->gridZ(i % 16) + dz * rand1(1) - poi.z);
			Primitive* tmppri = NULL;
			double len = d1.length();
			d1 /= len;
			if(findNearest(Ray(poi + EPS* d1, d1), len, tmppri)) {
				if(tmppri == light) {
					//cout << "i is " << i << endl;
					shade += SMPNUM_1;
				}
			}
		}
	}
	return shade;
}

Primitive * Engine::rayTrace(Ray & ray, Color & resCol, int depth, double refrIndex, double & dist) {
	if(depth > TRACEDEPTH) return NULL;
	dist = INF;
	Primitive* pri = NULL;
	int hitOrNot = 2;
	if((hitOrNot = findNearest(ray, dist, pri)) == MISS)	// no intersection
		return NULL;
	if(pri->IsLight()) {		// intersected with light
		resCol = pri->getMaterial()->getColor();
		return pri;
	}
	
	//else: intersected with sth else
	vec3 poi = ray.getOri() + dist*ray.getDir();;	// point of intersection

	// defuse and Phong spectecular
	for(int i = 0; i < scene->getLightNum(); ++i) {
	// for each light, calculate its defuse, shade and phong contribution to the pixel on the screen
		Primitive* light = scene->getLight(i);
		vec3 l;								// pointing to the light
		double shade = calcShade(light, poi, l);
		vec3 n = pri->getNorm(poi);			// normal of the primitive
		vec3 v = ray.getDir();				// direction of the ray
		vec3 r = l - 2 * l.dotPro(n) * n;	// reflection of the vec3 'l'
		// shading
		if(shade <= EPS) {
			continue;
		}

		// contribution of defuse reflection
		if(pri->getMaterial()->getDiff() > 0) {			
			double dot = n.dotPro(l);
			if(dot > EPS) {	// we can see the light's defuse
				resCol += shade * (dot * pri->getMaterial()->getDiff()) * pri->getMaterial()->getColor() * light->getMaterial()->getColor();
			}
		}
		// phong model. spec
		if(pri->getMaterial()->getSpec() > 0) {	
			double dot = v.dotPro(r);
			if(dot > 0) {
				double spec = pow(dot, 20) * pri->getMaterial()->getSpec() * shade;
				resCol += spec * light->getMaterial()->getColor();
			}
		}
	}

	// reflection
	double refl = pri->getMaterial()->getRefl();
	if(refl > EPS && depth < TRACEDEPTH) {
		double drefl = pri->getMaterial()->getDRelf();
		if(drefl > 0 && depth < 2) {
			vec3 n = pri->getNorm(poi);
			vec3 r = ray.getDir() - 2 * n.dotPro(ray.getDir()) * n;
			vec3 r1(-r.z, r.y, r.x);
			vec3 r2 = r.crossPro(r1);
			refl *= SMPNUM_1;
			for(int i = 0; i < SAMPLENUM; ++i) {
				double dx, dy;
				do {
					dx = rand1(1), dy = rand1(1);
				} while(dx*dx + dy*dy > 1);
				dx *= drefl, dy *= drefl;
				vec3 dir1 = (r + dx * r1 + dy * r2).normlize();
				double dist1;
				Color col1;
				rayTrace(Ray(poi + EPS * dir1, dir1), col1, depth + 1, refrIndex, dist1);
				resCol += refl * col1 * pri->getMaterial()->getColor();
			}
		}
		else {
			vec3 v = ray.getDir(), n = pri->getNorm(poi);
			vec3 r = v - 2 * v.dotPro(n) * n;
			Color refCol(0, 0, 0);
			double dis;
			rayTrace(Ray(poi + EPS*r, r), refCol, depth + 1, refrIndex, dis);
			resCol += refl * refCol * pri->getMaterial()->getColor();
		}
	}

	// refraction
	double refr = pri->getMaterial()->getRefr();
	if(refr > EPS && depth < TRACEDEPTH) {
		double n2 = pri->getMaterial()->getRefrInd();
		double n21;
		if(hitOrNot == HIT)
			n21 = n2 / refrIndex;
		else
			n21 = 1 / n2;
		//assert(hitOrNot != 0);
		vec3 n = hitOrNot * pri->getNorm(poi);
		double cosI = -ray.getDir().dotPro(n);
		double cosT_2 = 1 - (1 - cosI*cosI) / (n21*n21);
		if(cosT_2 > 0) {	// no total reflection
			vec3 t = ray.getDir() / n21 + (cosI / n21 - sqrt(cosT_2)) * n;
			Color refrCol(0, 0, 0);
			double dist;
			assert(t.dotPro(n) <= 0);
			rayTrace(Ray(poi + 10*EPS*t, t), refrCol, depth + 1, /*hitOrNot > 0 ?*/ 1 /*: n2*/, dist);
			Color trans(1, 1, 1);
#ifdef BEER_LAW
			Color absorbance = -0.15 * dist * pri->getMaterial()->getColor();
			trans = Color(exp(absorbance.r), exp(absorbance.g), exp(absorbance.b));
#endif
			resCol += refrCol * trans;
		}
	}

	return pri;
}
