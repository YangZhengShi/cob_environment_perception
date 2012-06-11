/****************************************************************
 *
 * Copyright (c) 2011
 *
 * Fraunhofer Institute for Manufacturing Engineering
 * and Automation (IPA)
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Project name: care-o-bot
 * ROS stack name: cob_vision
 * ROS package name: cob_env_model
 * Description: Feature Map for storing and handling geometric features
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Author: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 * Supervised by: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 *
 * Date of creation: 08/2011
 * ToDo:
 *
 *
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Fraunhofer Institute for Manufacturing
 *       Engineering and Automation (IPA) nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License LGPL as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License LGPL for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License LGPL along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************/

#ifndef __GEOMETRY_MAP_H__
#define __GEOMETRY_MAP_H__

//##################
//#### includes ####

// external includes
#include <Eigen/Core>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>

// internal includes
//extern "C" {
//#include "cob_3d_mapping_common/include/gpc.h"
//}
//#ifndef __GEOMETRY_MAP_VISUALISATION_H__
#include "cob_3d_mapping_geometry_map/vis/geometry_map_visualisation.h"
#include "cob_3d_mapping_common/polygon.h"
#include "cob_3d_mapping_common/cylinder.h"

//#include "cob_3d_mapping_common/shape.h"



//#endif
//#include "cob_3d_mapping_geometry_map/vis/TestPlanes.h"


template <class T>
class GeometryMap
{
public:
  /*inline std::ostream& operator << (std::ostream& os, const MapEntry& m)
  {
    os << "(" << m.d << "," << m.normal << "," << ")";
    return (os);
  }*/

	 // std::ofstream outputFile;
	  int counter_output;

//  typedef boost::shared_ptr<MapEntry> MapEntryPtr;

  // Constructor
  GeometryMap()
  :new_id_(0),
   counter_output(0),
   file_path_("./"),
   save_to_file_(false),
   cos_angle_(0.97),
   d_(0.1)
  {
	//  outputFile.open("/home/goa-hh/test.txt");

  }

  // Destructor
  ~GeometryMap()
  {
	  //outputFile.close();
  }

  void
  addMapEntry(boost::shared_ptr<T> p_ptr);



  void
  computeCentroid(T& p);


  void
  printMapEntry(T& p);

  void
  printMap();




  void
  saveMapEntry(std::string path, int ctr, T& p);

  void
  saveMap(std::string path);

  void
  clearMap();

//  void
//  getCoordinateSystemOnPlane(const Eigen::Vector3f &normal,
//                             Eigen::Vector3f &u,
//                             Eigen::Vector3f &v);

//  void
//  getTransformationFromPlaneToWorld(const Eigen::Vector3f &normal,
//                                    const Eigen::Vector3f &origin,
//                                    Eigen::Affine3f &transformation);

//  void
//  getPointOnPlane(const Eigen::Vector3f &normal,double d,Eigen::Vector3f &point);

  float
  rounding(float x);

  void
  colorizeMap();

  boost::shared_ptr<std::vector<boost::shared_ptr<T> > >
  getMap()
  {
    return boost::make_shared< std::vector< boost::shared_ptr<T> > >(map_);
  }

  void
  setFilePath(std::string file_path)
  {
    file_path_ = file_path;
  }

  void
  setSaveToFile(bool save_to_file)
  {
    save_to_file_ = save_to_file;
  }

  void
  setMergeThresholds(double cos_angle, double d)
  {
    cos_angle_ = cos_angle;
    d_ = d;
  }

protected:
  std::vector<boost::shared_ptr<T> > map_;
  unsigned int new_id_;
  std::string file_path_;
  bool save_to_file_;
  double cos_angle_, d_;

};

#endif //__GEOMETRY_MAP_H__
