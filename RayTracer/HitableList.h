#pragma once

#include "Hitable.h"

class HitableList : public Hitable
{
public:
	HitableList();
	HitableList(Hitable** _list, int _n)
	{
		hitable_list = _list;
		list_size = _n;
	}

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;

private:
	Hitable** hitable_list;
	int list_size;
};


/////////////////////////////////////////////////////////////////////////////////////////
bool HitableList::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	HitRecord temp_rec;
	bool hit_anything = false;
	double closest_so_far = tmax;

	for (int i = 0; i < list_size; i++)
	{
		if (hitable_list[i]->hit(r, tmin, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}