#include <cmath>
#include "scene.h"
#include "raytracer.h"

void Scene::init() {
	pris = new Primitive*[100];
	//return;
	// ground plane
	pris[0] = new planePrim(vec3(0, 1, 0), 4.4f);
	pris[0]->setName("plane");
	pris[0]->getMaterial()->setRefl(0);
	pris[0]->getMaterial()->setDiff(1.0f);
	pris[0]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));
	// big sphere
	pris[1] = new sphere(vec3(1, -0.8f, 3), 2.5f);
	pris[1]->setName("big sphere");
	pris[1]->getMaterial()->setRefl(0.6f);
	pris[1]->getMaterial()->setColor(Color(0.7f, 0.7f, 0.7f));
	// small sphere
	pris[2] = new sphere(vec3(-5.5f, -0.5, 7), 2);
	pris[2]->setName("small sphere");
	pris[2]->getMaterial()->setRefl(1.0f);
	pris[2]->getMaterial()->setDiff(0.1f);
	pris[2]->getMaterial()->setColor(Color(0.7f, 0.7f, 1.0f));
	// light source 1
	pris[3] = new sphere(vec3(0, 5, 5), 0.1f);
	pris[3]->setLight(true);
	pris[3]->getMaterial()->setColor(Color(0.6f, 0.6f, 0.6f));
	// light source 2
	pris[4] = new sphere(vec3(2, 5, 1), 0.1f);
	pris[4]->setLight(true);
	pris[4]->getMaterial()->setColor(Color(0.7f, 0.7f, 0.9f));
	// set number of primitives
	priNum = 5;
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
		if(dist > i1) {
			dist = i1;
			return BCKINTER;
		}
	}
	return MISS;
}

Material::Material():color(Color(0.2, 0.2, 0.2)), diff(0.2), refl(0){
}
