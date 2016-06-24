#pragma once
#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "common.h"
#include "myMat.h"
#include "scene.h"

class Ray {
	vec3 origin;
	vec3 direction;
public:
	Ray(const vec3& ori, const vec3& dir)
		:origin(ori), direction(dir) {}
	vec3& getOri() { return origin; }
	vec3& getDir() { return direction; }
};

class Engine {
public:
	Engine(myMat& m) : screen(m){
		scene = new Scene;
		initRender();
	}
	void initRender();
	void render();
	int findNearest(Ray& ray, double& dist, Primitive*& prim);
	double calcShade(Primitive* light, const vec3& poi, vec3& dir);
	Primitive* renderRay(const vec3& scr, Color& col);
	Primitive* rayTrace(Ray& ray, Color& res, int depth, double index, double& dist);
private:
	double SCRX1, SCRX2, SCRY1, SCRY2, DX, DY;
	vec3 SR, CW;
	int presentX, presentY;
	// Primitive** lastRow???
	Scene* scene;
	myMat& screen;
};

#endif