#include <cmath>
#include "scene.h"
#include "raytracer.h"
using namespace std;
void Scene::addPris
(int type, const std::pair<vec3, double>& paramater, const std::string& name, double refl, double refr, double refrInd, double spec, double diff, Color col) {
	if(type == Primitive::SPHERE) {
		pris[priNum] = new sphere(paramater.first, paramater.second);
	}
	else if(type == Primitive::PLANE) {
		pris[priNum] = new planePrim(paramater.first, paramater.second);
	}
	pris[priNum]->setName(name);
	if(refl >= 0)
		pris[priNum]->getMaterial()->setRefl(refl);
	if(refr >= 0)
		pris[priNum]->getMaterial()->setRefr(refr);
	if(refrInd >= 0)
		pris[priNum]->getMaterial()->setRefrInd(refrInd);
	if(spec >= 0)
		pris[priNum]->getMaterial()->setSpec(spec);
	if(diff >= 0)
		pris[priNum]->getMaterial()->setDiff(diff);

	pris[priNum]->getMaterial()->setColor(col);
	priNum++;
}

void Scene::init() {
	pris = new Primitive*[500];
	// ground plane
	addPris(Primitive::PLANE, make_pair(vec3(0, 1, 0), 4.4), "ground plane", 0, 0, -1, -1, 1, Color(0.4, 0.3, 0.3));
	
	// big sphere
	addPris(Primitive::SPHERE, make_pair(vec3(2, 0.8f, 3), 2.5f), "big sphere", 0.2, 0.8, 1.3, -1, -1, Color(0.7f, 0.7f, 0.7f));
	
	// small sphere
	addPris(Primitive::SPHERE, make_pair(vec3(-5.5f, -0.5, 7), 2), "small sphere", 0.5, 0, 1.3, -1, 0.1, Color(0.7, 0.7, 1));
	
	// light source 1
	addPris(Primitive::SPHERE, make_pair(vec3(0, 5, 5), 0.1f), "light 1", -1, -1, -1, -1, -1, Color(0.4, 0.4, 0.4));
	pris[priNum - 1]->setLight(true);
	
	// light source 2
	addPris(Primitive::SPHERE, make_pair(vec3(-3, 5, 1), 0.1f), "light 2", -1, -1, -1, -1, -1, Color(0.6, 0.6, 0.8));
	pris[priNum - 1]->setLight(true);
	
	// extra sphere
	addPris(Primitive::SPHERE, make_pair(vec3(-1.5, -3.8, 1), 1.5), "extra sphere", 0, 0.8, -1, -1, -1, Color(1, 0.4, 0.4));
	
	// back plane
	addPris(Primitive::PLANE, make_pair(vec3(0.4, 0, -1), 12), "back_plane", 0, 0, -1, 0, 0.6, Color(0.5, 0.3, 0.5));

	// celling plane
	addPris(Primitive::PLANE, make_pair(vec3(0, -1, 0), 7.4f), "celling_plane", 0, 0, -1, 0, 0.5, Color(0.4f, 0.7f, 0.7f));
	
	// grid sphere
	for(int x = 0; x < 8; ++x) {
		for(int y = 0; y < 8; ++y) {
			addPris(Primitive::SPHERE, make_pair(vec3(-4.5f + x * 1.5f, -4.3f + y * 1.5f, 10), 0.3f), "grid_sphere", 0, 0, -1, 0.6, 0.6, Color(0.3f, 1.0f, 0.4f));
		}
	}

}

int planePrim::intersect(Ray & ray, double & dist) {
	double dot = pla.nvec.dotPro(ray.getDir());
	if(abs(dot) < EPS) {
		return MISS;
	}
	double dis = -(pla.D + ray.getOri().dotPro(pla.nvec)) / dot;
	if(dis > 0 && dis < dist) {	// intercept the plane before others
		dist = dis;
		return HIT;
	}
	return MISS;
}

int sphere::intersect(Ray & ray, double & dist) {
	vec3 v = center - ray.getOri();
	double d1 = ray.getDir().dotPro(v);
	double d_2 = v.sqrLength() - d1*d1;
	
	if(d_2 > R_2)
		return MISS;

	double d = sqrt(R_2 - d_2);
	double i1 = d1 - d, i2 = d1 + d;
	if(i2 < 0) {	// wrong direction, no intercept
		return MISS;
	}
	// else d1 + d >= 0
	if(i1 > 0) {	// hit from outside
		if(dist > i1) {
			dist = i1;
			return HIT;
		}
	}
	else {		// hit from inside0
		if(dist > i2) {
			dist = i2;
			return BCKINTER;
		}
	}
	return MISS;
}

Material::Material():
	color(Color(0.2, 0.2, 0.2)),
	diff(0.2),spec(0.8), refl(0), refrInd(1.5), refr(0) {}
