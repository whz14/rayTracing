#pragma once
#ifndef SCENE_H
#define SCENE_H
#define HIT 1
#define MISS 0
#define BCKINTER -1
#include "common.h"
//#include "raytracer.h"

class Material {
	double diff, spec;	// diffuse reflection and specular. sum = 1
	double refl, refr;	// reflection and refraction. 
	double refrInd;		// absolute refraction index (to vacuum)
	Color color;
public:
	double getRefl() { return refl; }
	double getDiff() { return diff; }
	double getSpec() { return spec; }
	double getRefr() { return refr; }
	double getRefrInd() { return refrInd; }
	Color getColor() { return color; }
	void setDiff(double di) { diff = di; }
	void setSpec(double sp) { spec = sp; }
	void setRefl(double re) { refl = re; }
	void setRefr(double re) { refr = re; }
	void setRefrInd(double n) { refrInd = n; }
	void setColor(const Color& co) { color = co; }
	Material();
};

class Ray;
class Primitive {
public:
	enum {
		PLANE = 1, SPHERE
	};
	Primitive() : isLight(false), name("") {}
	void setName(const std::string& nme) { name = nme; }
	void setMaterial(Material* mater) { material = *mater; }
	void setLight(bool lon) { isLight = lon; }
	Material* getMaterial() { return &material; }
	std::string getName() { return name; }
	bool IsLight() { return isLight; }
	
	virtual vec3 getNorm(const vec3& position) = 0;
	virtual int intersect(Ray& ray, double& dist) = 0;
	virtual int getType() = 0;

protected:
	Material material;
	bool isLight;
	std::string name;
};

class planePrim : public Primitive {
	plane pla;
public:
	planePrim(const vec3& normVec, double dist) : pla(normVec, dist) {}
	vec3 getNorm(const vec3& positon) { return pla.nvec; }
	int intersect(Ray& ray, double& dist);
	int getType() { return PLANE; }
};

class sphere : public Primitive {
	vec3 center;
	double R, R_inv, R_2;	// radius, 1/radius and radius^2
public:
	sphere(const vec3& cen, double r1):center(cen), R(r1), R_inv(1/r1), R_2(r1*r1) {}
	vec3 getNorm(const vec3& position) { vec3 res = (position - center); res.normlize(); return res; }
	int intersect(Ray& ray, double& dist);
	int getType() { return SPHERE; }
	vec3 getCenter() { return center; }
};

class Scene {
	int priNum;
	Primitive** pris;
public:
	Scene() :priNum(0), pris(NULL) {
		//init();
	}
	~Scene() {
		for(int i = 0; i < priNum; ++i) {
			delete pris[i];
		}
		delete[] pris;
	}
	void addPris(int type, const std::pair<vec3, double>& paramater, const std::string& name, double refl, double refr, double refrInd, double spec, double diff, Color col = Color(0.2, 0.2, 0.2));
	void init();
	//Primitive* operator[] (int index) { return pris[index]; }
	Primitive* getPri(int index) { return pris[index]; }
	int getNum() { return priNum; }
};

#endif