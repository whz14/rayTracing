#pragma once
#ifndef SCENE_H
#define SCENE_H
#define HIT 1
#define MISS 0
#define BCKINTER -1
#include "common.h"
//#include "raytracer.h"

class Material {
	double diff, spec;	// diffuse and specular. sum = 1
	double refl, refr;	// reflection and refraction. 
	double refrInd;		// absolute refraction index (to vacuum)
	double dRefl;		// diffuse reflection
	Color color;
public:
	double getRefl() { return refl; }
	double getDiff() { return diff; }
	double getSpec() { return spec; }
	double getRefr() { return refr; }
	double getRefrInd() { return refrInd; }
	double getDRelf() { return dRefl; }
	Color getColor() { return color; }
	void setDiff(double di) { diff = di; }
	void setSpec(double sp) { spec = sp; }
	void setRefl(double re) { refl = re; }
	void setRefr(double re) { refr = re; }
	void setRefrInd(double n) { refrInd = n; }
	void setDRefl(double dr) { dRefl = dr; }
	void setColor(const Color& co) { color = co; }
	Material();
};

class Ray;
class Primitive {
public:
	enum {
		PLANE = 1, SPHERE, AABBBOX
	};
	Primitive() : isLight(false), name("") {}
	void setName(const std::string& nme) { name = nme; }
	void setMaterial(Material* mater) { material = *mater; }
	Material* getMaterial() { return &material; }
	std::string getName() { return name; }
	bool IsLight() { return isLight; }
	
	virtual vec3 getNorm(const vec3& position) = 0;
	virtual int intersect(Ray& ray, double& dist) = 0;
	virtual bool intersect(const aabb& box) = 0;
	virtual int getType() = 0;
	virtual aabb getAABB() = 0;
	virtual void setLight(bool lon) { isLight = lon; }

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
	bool intersect(const aabb& box);
	int getType() { return PLANE; }
	aabb getAABB() { return aabb(vec3(-1000, -1000, -1000), vec3(3000, 3000, 3000)); }
};

class sphere : public Primitive {
	vec3 center;
	double R, R_inv, R_2;	// radius, 1/radius and radius^2
public:
	sphere(const vec3& cen, double r1):center(cen), R(r1), R_inv(1/r1), R_2(r1*r1) {}
	vec3 getCenter() { return center; }
	
	vec3 getNorm(const vec3& position) { vec3 res = (position - center); res.normlize(); return res; }
	int intersect(Ray& ray, double& dist);
	bool intersect(const aabb& box);
	aabb getAABB() { return aabb(center - vec3(R, R, R), 2 * vec3(R, R, R)); }
	int getType() { return SPHERE; }
};

class box : public Primitive {
	aabb b;
	double grid[32];	// WTF???
public:
	box() { memset(grid, 0, sizeof(grid)); }
	box(const aabb& b1) :b(b1) { memset(grid, 0, sizeof(grid)); }
	box(const vec3& posi, const vec3& size) : b(posi, size) {}
	int getType() { return AABBBOX; }
	vec3 getNorm(const vec3& position);
	int intersect(Ray& ray, double& dist);
	bool intersect(const aabb& b1) { return b.intersect(b1); }
	aabb getAABB() { return b; }

	void setLight(bool lon);
	bool contains(const vec3& p) { return b.contains(p); }
	double gridX(int index) { return grid[index << 1]; }
	double gridZ(int index) { return grid[(index << 1) + 1]; }
	vec3& getPosi() { return b.getPosi(); }
	vec3& getSize() { return b.getSize(); }
};

class PriList {
	Primitive* pri;
	PriList* next;
public:
	PriList(): pri(NULL), next(NULL) {}
	~PriList() { if(next) delete next; }
	
	void setPri(Primitive*  pr) { pri = pr; }
	Primitive* getPri() { return pri; }
	void setNext(PriList* ne) { next = ne; }
	PriList* getNext() { return next; }
};


class Scene {
	int priNum, lightNum;
	Primitive** pris;
	Primitive** lights;
	//PriList* (fuck[100]);
	PriList** grids;
	aabb extends;
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
	void addPris(int type, const std::pair<vec3, vec3>& paramater, const std::string& name,
		double refl, double refr, double refrInd, double spec, double diff,
		Color col = Color(0.2, 0.2, 0.2), double diffRefl = -1);
	void addPris(int type, const std::pair<vec3, double>& paramater, const std::string& name, 
		double refl, double refr, double refrInd, double spec, double diff, 
		Color col = Color(0.2, 0.2, 0.2), double diffRefl = -1);
	void init();
	void buildGrid();
	//Primitive* operator[] (int index) { return pris[index]; }
	Primitive* getPri(int index) { return pris[index]; }
	Primitive* getLight(int index) { return lights[index]; }
	int getPriNum() { return priNum; }
	int getLightNum() { return lightNum; }
	PriList** getGrids() { return grids; }
	const aabb& getExtends() { return extends; }
};

#endif