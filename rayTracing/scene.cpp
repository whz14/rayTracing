#include <cmath>
#include "config.h"
#include "scene.h"
#include "raytracer.h"
using namespace std;

Material::Material() :
	color(Color(0.2, 0.2, 0.2)),
	diff(0.2), spec(0.8), refl(0), refrInd(1.5), refr(0), dRefl(0) {
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

bool planePrim::intersect(const aabb & box) {
	vec3 v[2] = {box.getPosi(),  box.getSize() + box.getPosi()};
	int side1 = 0, side2 = 0;
	for(int i = 0; i < 8; ++i) {
		vec3 p(v[i & 1].x, v[(i >> 1) & 1].y, v[(i >> 2) & 1].z);	// for each vertex of the box
		if(p.dotPro(pla.nvec) + pla.D < 0)
			++side1;
		else
			++side2;
	}
	if((side1 == 0) || (side2 == 0)) {	// 0 point is to the left of the plane or 0 point to the right of the plane
		return false;
	}
	return true;
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

bool sphere::intersect(const aabb & box) {
	double dmin = 0;
	vec3 v1 = box.getPosi(), v2 = box.getPosi() + box.getSize();
	if(center.x < v1.x) {
		dmin = dmin + (center.x - v1.x) * (center.x - v1.x);
	}
	else if(center.x > v2.x) {
		dmin = dmin + (center.x - v2.x) * (center.x - v2.x);
	}
	if(center.y < v1.y) {
		dmin = dmin + (center.y - v1.y) * (center.y - v1.y);
	}
	else if(center.y > v2.y) {
		dmin = dmin + (center.y - v2.y) * (center.y - v2.y);
	}
	if(center.z < v1.z) {
		dmin = dmin + (center.z - v1.z) * (center.z - v1.z);
	}
	else if(center.z > v2.z) {
		dmin = dmin + (center.z - v2.z) * (center.z - v2.z);
	}
	return (dmin <= R_2);

	// return false;
}

vec3 box::getNorm(const vec3 & position) {
	cout << "fuck in box::getNorm\n";
	return vec3();
}

int box::intersect(Ray & ray, double & d0) {
	double dist[6];
	vec3 ip[6], d = ray.getDir(), o = ray.getOri();
	bool retval = MISS;
	for(int i = 0; i < 6; i++) dist[i] = -1;
	vec3 v1 = getPosi(), v2 = getPosi() + getSize();
	if(d.x) {
		double rc = 1.0f / d.x;
		dist[0] = (v1.x - o.x) * rc;
		dist[3] = (v2.x - o.x) * rc;
	}
	if(d.y) {
		double rc = 1.0f / d.y;
		dist[1] = (v1.y - o.y) * rc;
		dist[4] = (v2.y - o.y) * rc;
	}
	if(d.z) {
		double rc = 1.0f / d.z;
		dist[2] = (v1.z - o.z) * rc;
		dist[5] = (v2.z - o.z) * rc;
	}
	for(int i = 0; i < 6; i++) if(dist[i] > 0) {
		ip[i] = o + dist[i] * d;
		if((ip[i].x > (v1.x - EPS)) && (ip[i].x < (v2.x + EPS)) &&
			(ip[i].y >(v1.y - EPS)) && (ip[i].y < (v2.y + EPS)) &&
			(ip[i].z >(v1.z - EPS)) && (ip[i].z < (v2.z + EPS))) {
			if(dist[i] < d0) {
				d0 = dist[i];
				retval = HIT;
			}
		}
	}
	return retval;
	//return 0;
}

void box::setLight(bool lon) {
	isLight = lon;
	if(isLight) {
		//grid = new double[32];
		grid[0] = 1, grid[1] = 2;
		grid[2] = 3, grid[3] = 3;
		grid[4] = 2, grid[5] = 0;
		grid[6] = 0, grid[7] = 1;
		grid[8] = 2, grid[9] = 3;
		grid[10] = 0, grid[11] = 3;
		grid[12] = 0, grid[13] = 0;
		grid[14] = 2, grid[15] = 2;
		grid[16] = 3, grid[17] = 1;
		grid[18] = 1, grid[19] = 3;
		grid[20] = 1, grid[21] = 0;
		grid[22] = 3, grid[23] = 2;
		grid[24] = 2, grid[25] = 1;
		grid[26] = 3, grid[27] = 0;
		grid[28] = 1, grid[29] = 1;
		grid[30] = 0, grid[31] = 2;
		for(int i = 0; i < 16; i++) {
			grid[i * 2] = grid[i * 2] * b.getSize().x / 4 + b.getPosi().x;
			grid[i * 2 + 1] = grid[i * 2 + 1] * b.getSize().z / 4 + b.getPosi().z;
		}
	}
}


void Scene::buildGrid() {
	grids = new PriList*[GRIDNUM * GRIDNUM * GRIDNUM];
	memset(grids, 0, GRIDNUM * GRIDNUM * GRIDNUM * sizeof(PriList*));
	vec3 ldf(-14, -5, -6), rub(14, 8, 30);	// left-down-front and right-up-back
	extends = aabb(ldf, rub - ldf);
	double dx = (rub.x - ldf.x) / GRIDNUM, dx_1 = 1 / dx;
	double dy = (rub.y - ldf.y) / GRIDNUM, dy_1 = 1 / dy;
	double dz = (rub.z - ldf.z) / GRIDNUM, dz_1 = 1 / dz;
	extends = aabb(ldf, rub - ldf);
	lights = new Primitive*[MAXLIGHT];
	lightNum = 0;
	for(int i = 0; i < priNum; ++i) {
	//for(Primitive* pr = pris[0]; pr != pris[priNum]; ++pr) {
		Primitive* pr = pris[i];
		if(pr->IsLight())
			lights[lightNum++] = pr;
		aabb bound = pr->getAABB();
		vec3 ldf1 = bound.getPosi(), rub1 = bound.getSize() + bound.getPosi();
		int x1 = (int)((ldf1.x - ldf.x) * dx_1), x2 = (int)((rub1.x - ldf.x) * dx_1) + 1;
		int y1 = (int)((ldf1.y - ldf.y) * dy_1), y2 = (int)((rub1.y - ldf.y) * dy_1) + 1;
		int z1 = (int)((ldf1.z - ldf.z) * dz_1), z2 = (int)((rub1.z - ldf.z) * dz_1) + 1;
		x1 = max(0, x1); x2 = min(x2, GRIDNUM - 1);
		y1 = max(0, y1); y2 = min(y2, GRIDNUM - 1);
		z1 = max(0, z1); z2 = min(z2, GRIDNUM - 1);
		for(int x = x1; x <= x2; ++x)
			for(int y = y1; y <= y2; ++y)
				for(int z = z1; z <= z2; ++z) {
					int idx = z * GRIDNUM * GRIDNUM + y * GRIDNUM + x;
					aabb cell(ldf+ vec3(x*dx, y*dy, z * dz), vec3(dx, dy, dz));
					if(pr->intersect(cell)) {
						PriList* l = new PriList;
						l->setPri(pr);
						l->setNext(grids[idx]);
						grids[idx] = l;
					}
				}
	}
}

void Scene::addPris
(int type, const std::pair<vec3, vec3>& paramater, const std::string & name, double refl, double refr, double refrInd, double spec, double diff, Color col, double diffRefl) {
	if(type == Primitive::AABBBOX) {
		pris[priNum] = new box(paramater.first, paramater.second);
	}
	else {
		cout << type << endl;
		system("pause");
		exit(-1);
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
	if(diffRefl >= 0)
		pris[priNum]->getMaterial()->setDRefl(diffRefl);
	pris[priNum]->getMaterial()->setColor(col);
	priNum++;
}

void Scene::addPris
(int type, const std::pair<vec3, double>& paramater, const std::string& name, double refl, double refr, double refrInd, double spec, double diff, Color col, double diffRefl) {
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
	if(diffRefl >= 0)
		pris[priNum]->getMaterial()->setDRefl(diffRefl);
	pris[priNum]->getMaterial()->setColor(col);
	priNum++;
}

void Scene::init() {
	//pris = new Primitive*[500];
	//// ground plane
	//addPris(Primitive::PLANE, make_pair(vec3(0, 1, 0), 4.4), "ground plane", 
	//	0, 0, -1, -1, 1, Color(0.4, 0.3, 0.3));
	//// big sphere
	//addPris(Primitive::SPHERE, make_pair(vec3(0, -0.8f, 7), 2), "big sphere", 
	//	0.2, 0.8, 1.3, -1, -1, Color(0.7f, 0.7f, 0.7f), 0.3);
	//// 2 small spheres
	//addPris(Primitive::SPHERE, make_pair(vec3(-5, -0.8, 7), 2), "small sphere",
	//	0.5, 0, 1.3, -1, 0.1, Color(0.7, 0.7, 1), 0.6);
	//addPris(Primitive::SPHERE, make_pair(vec3(5, -0.8, 7), 2), "small sphere",
	//	0.5, 0.0, 1.3, -1.0, 0.1, Color(0.7, 0.7, 1));

	//// plane light source 1
	//addPris(Primitive::AABBBOX, make_pair(vec3(-1, 5, 4), vec3(2, 0.1, 2)), "plane light",
	//	-1, -1, -1, -1, -1, Color(1, 1, 1));
	//pris[priNum - 1]->setLight(true);

	//// back plane
	//addPris(Primitive::PLANE, make_pair(vec3(0, 0, -1), 12), "back_plane", 
	//	0, 0, -1, 0, 1, Color(0.4, 0.3, 0.3));
	//// celling plane
	//addPris(Primitive::PLANE, make_pair(vec3(0, -1, 0), 5.2), "celling_plane", 
	//	0, 0, -1, 0, 1, Color(0.4f, 0.3f, 0.3f));
	pris = new Primitive*[500];
	// ground plane
	pris[0] = new planePrim(vec3(0, 1, 0), 4.4f);
	pris[0]->setName("plane");
	pris[0]->getMaterial()->setRefl(0.0f);
	pris[0]->getMaterial()->setRefr(0.0f);
	pris[0]->getMaterial()->setDiff(1.0f);
	pris[0]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));
	// big sphere
	pris[1] = new sphere(vec3(0, -0.8f, 7), 2);
	pris[1]->setName("big sphere");
	pris[1]->getMaterial()->setRefl(0.2f);
	pris[1]->getMaterial()->setRefr(0.8f);
	pris[1]->getMaterial()->setRefrInd(1.3f);
	pris[1]->getMaterial()->setColor(Color(0.7f, 0.7f, 1.0f));
	pris[1]->getMaterial()->setDRefl(0.3f);
	// small sphere
	pris[2] = new sphere(vec3(-5, -0.8f, 7), 2);
	pris[2]->setName("small sphere");
	pris[2]->getMaterial()->setRefl(0.5f);
	pris[2]->getMaterial()->setRefr(0.0f);
	pris[2]->getMaterial()->setRefrInd(1.3f);
	pris[2]->getMaterial()->setDiff(0.1f);
	pris[2]->getMaterial()->setColor(Color(0.7f, 0.7f, 1.0f));
	pris[2]->getMaterial()->setDRefl(0.6f);
	// third sphere
	pris[3] = new sphere(vec3(5, -0.8f, 7), 2);
	pris[3]->setName("small sphere");
	pris[3]->getMaterial()->setRefl(0.5f);
	pris[3]->getMaterial()->setRefr(0.0f);
	pris[3]->getMaterial()->setRefrInd(1.3f);
	pris[3]->getMaterial()->setDiff(0.1f);
	pris[3]->getMaterial()->setColor(Color(0.7f, 0.7f, 1.0f));
#if 1
	// area light
	pris[4] = new box(aabb(vec3(-1, 5, 4), vec3(2, 0.1f, 2)));
	pris[4]->setLight(true);
	pris[4]->getMaterial()->setColor(Color(1, 1, 1));
#else
	// light source 1
	pris[4] = new Sphere(vec3(0, 5, 5), 0.1f);
	pris[4]->Light(true);
	pris[4]->getMaterial()->SetColor(Color(1, 1, 1));
#endif
	// back plane
	pris[5] = new planePrim(vec3(0, 0, -1), 12.4f);
	pris[5]->setName("backplane");
	pris[5]->getMaterial()->setRefl(0.0f);
	pris[5]->getMaterial()->setRefr(0.0f);
	pris[5]->getMaterial()->setDiff(1.0f);
	pris[5]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));
	// ceiling
	pris[6] = new planePrim(vec3(0, -1, 0), 5.2f);
	pris[6]->setName("ceiling");
	pris[6]->getMaterial()->setRefl(0.0f);
	pris[6]->getMaterial()->setRefr(0.0f);
	pris[6]->getMaterial()->setDiff(1.0f);
	pris[6]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));
	priNum = 6;

	buildGrid();
}