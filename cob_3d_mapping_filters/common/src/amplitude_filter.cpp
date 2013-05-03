/*!
 *****************************************************************
 * \file
 *
 * \note
 *   Copyright (c) 2012 \n
 *   Fraunhofer Institute for Manufacturing Engineering
 *   and Automation (IPA) \n\n
 *
 *****************************************************************
 *
 * \note
 *  Project name: care-o-bot
 * \note
 *  ROS stack name: cob_environment_perception_intern
 * \note
 *  ROS package name: cob_3d_mapping_filters
 *
 * \author
 *  Author: Waqas Tanveer, email:waqas.informatik@googlemail.com
 * \author
 *  Supervised by: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 *
 * \date Date of creation: 05/2011
 *
 * \brief
 * Description:
 *
 *****************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     - Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer. \n
 *     - Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution. \n
 *     - Neither the name of the Fraunhofer Institute for Manufacturing
 *       Engineering and Automation (IPA) nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission. \n
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License LGPL as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License LGPL for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License LGPL along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************/
//##################
//#### includes ####

// PCL includes
#include "pcl/point_types.h"
#include "pcl/impl/instantiate.hpp"
#include <cob_3d_mapping_common/point_types.h>

// cob_3d_mapping_filters includes
#include "cob_3d_mapping_filters/amplitude_filter.h"
#include "cob_3d_mapping_filters/impl/amplitude_filter.hpp"
#include <pcl/filters/impl/filter.hpp>

#if defined(PCL_VERSION_COMPARE) && PCL_MINOR_VERSION >= 7
#include <pcl/impl/pcl_base.hpp>
#endif

void
cob_3d_mapping_filters::AmplitudeFilter<sensor_msgs::PointCloud2>::applyFilter (PointCloud2 &pc_out)
{
  pc_out.header = input_->header;
  pc_out.fields = input_->fields;
  pc_out.point_step = input_->point_step;
  pc_out.data.resize (input_->data.size());

  int x_offset = 0, i_offset = 0;
  for (size_t d = 0; d < input_->fields.size(); ++d)
  {
    if(input_->fields[d].name == "x")
      x_offset = input_->fields[d].offset;
    if(input_->fields[d].name == "amplitude")
      i_offset = input_->fields[d].offset;
  }
  //std::cout<<" x_offset: "<<x_offset<<std::endl;
  //std::cout<<" i_offset: "<<i_offset<<std::endl;

    int nr_p = 0;
    float amplitude;
    const unsigned int total_points = input_->width*input_->height;

    for ( unsigned int pc_msg_idx = 0; pc_msg_idx < total_points; pc_msg_idx++)
    {
      amplitude = *(float*)&input_->data[pc_msg_idx * input_->point_step + i_offset];
      if(amplitude > amplitude_min_threshold_  && amplitude < amplitude_max_threshold_ )
      {
        memcpy(&pc_out.data[nr_p * pc_out.point_step], &input_->data[pc_msg_idx * pc_out.point_step],pc_out.point_step);
        nr_p++;
      }
    }

    pc_out.width = nr_p;
    pc_out.height = 1;
    pc_out.data.resize(nr_p*pc_out.point_step);
    pc_out.is_dense = true;

}

using namespace pcl;
PCL_INSTANTIATE(AmplitudeFilter, (PointXYZA));
