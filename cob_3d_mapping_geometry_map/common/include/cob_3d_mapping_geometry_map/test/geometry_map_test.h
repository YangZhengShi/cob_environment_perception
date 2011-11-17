/*
 * TestPlanes.h
 *
 *  Created on: 02.11.2011
 *      Author: goa-hh
 */

#ifndef GeometryMapTest_H_
#define GeometryMapTest_H_

#include <vector>
#include <Eigen/Geometry>


#include "cob_3d_mapping_geometry_map/map_entry.h"


class GeometryMapTest {
public:
	GeometryMapTest();
	~GeometryMapTest();

	void
	fillingHole();

private:
	TestPlanes tp;
	GeometryMapVisualisation gmv;
	GeometryMap gm;
};

#endif /* GeometryMapTest_H_ */
