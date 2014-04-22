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
 * ROS stack name: cob_environment_perception_intern
 * ROS package name: cob_3d_mapping_features
 * Description:
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Author: Joshua Hampp, email:joshua.hampp@ipa.fraunhofer.de
 *
 * Date of creation: 03/2014
 * ToDo:
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

#pragma once

#include <complex>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <Eigen/Geometry>
#include <cob_3d_mapping_common/polypartition.h>
#include <pcl/PolygonMesh.h>		//only used for debugging/test functions (perhaps put outside?)

namespace cob_3d_features
{
  /*! @brief feature generator for surfaces based on fourier transformation (rotation/translation invariant) */
  template<const int num_radius_, const int num_angle_, typename TSurface, typename Scalar=float, typename TAffine=Eigen::Affine3f>
  class InvariantSurfaceFeature
  {
  public:
    enum {NUM_RADIUS=num_radius_, NUM_ANGLE=num_angle_};

    typedef Eigen::Matrix<Scalar, 3, 1> TVector;
    typedef std::vector<TSurface> TSurfaceList;
    typedef boost::shared_ptr<TSurfaceList> PTSurfaceList;
    typedef Eigen::Matrix<Scalar, num_angle_, num_angle_> FeatureAngle;
    typedef Eigen::Matrix<std::complex<Scalar>, num_angle_, num_angle_> FeatureAngleComplex;
    //typedef FeatureAngle Feature[num_radius_];
    struct Feature {
	FeatureAngle vals[num_radius_];

	Scalar operator-(const Feature &o) const {
		Scalar r=0;
		for(int i=0; i<num_radius_; i++)
			r += (vals[i]-o.vals[i]).cwiseAbs().sum();
		return r;
	}
    };
    typedef struct {FeatureAngleComplex vals[num_radius_];} FeatureComplex;

    struct ResultVector {
      TVector v;
      std::vector<Feature> ft;
    };
    typedef std::vector<ResultVector> ResultVectorList;
    typedef boost::shared_ptr<ResultVectorList> PResultVectorList;
    typedef boost::shared_ptr<const ResultVectorList> PResultVectorListConst;

    typedef enum {
      INVARAINCE_X=1, INVARAINCE_Y=2, INVARAINCE_Z=4,
      INVARAINCE_INCLINATION=8, INVARAINCE_AZIMUTH=16,
      INVARAINCE_ALL=0x1f
    } EINVARAINCE;

    /*! constructor */
    InvariantSurfaceFeature() :
      invariance_(INVARAINCE_ALL)
    {
      //TODO: default radi
    }

    /*! destructor */
    virtual ~InvariantSurfaceFeature() {}

	//resets the input (list of surfaces with holes) and computes initial fft
    void setInput(PTSurfaceList surfs);
    void compute();

    PResultVectorListConst getResult() const {return result_;}

    const TAffine &getTransformation() const {return transform_;}
    void setTransformation(const TAffine &t) {transform_=t;}

    const EINVARAINCE &getInvarianceSettings() const {return invariance_;}
    void setInvarianceSettings(const EINVARAINCE &t) {invariance_=t;}

    const std::vector<float> &getRadii() const {return radii_;}
    void addRadius(const float r) {	//insert sorted!
		std::vector<float>::iterator it = radii_.begin();
		while(it!=radii_.end() && *it>r) ++it;
		radii_.insert(it, r);
	}

    /*const int getNumRadius() const {return num_radius_;}
    void setNumRadius(const int t) {num_radius_=t;}
    const int getNumAngle() const {return num_angle_;}
    void setNumAngle(const int t) {num_angle_=t;}*/


    /* UNIT TESTS: available in invariant_surface_feature_unit_tests.hpp */
    bool test_singleTriangle(const int num) const;
	pcl::PolygonMesh::Ptr test_subsampling_of_Map(const int num, const Scalar r2);
	
	/* DEBUG functions */
    void dbg_mesh_of_subsamp(const TVector &at, const Scalar radius, std::vector<TVector> &pts, std::vector<int> &inds) const;
    void dbg_keypoints(std::vector<TVector> &keypoints) const {generateKeypoints(keypoints);}
	pcl::PolygonMesh::Ptr dbg_Mesh_of_Map() const {return dbg_triangles2mesh(triangulated_input_);}
	
  protected:
    struct Triangle {
		struct Tri2D {
			const Eigen::Matrix<Scalar, 2, 1> *p_[3];
		};

		Eigen::Matrix<Scalar, 2, 1> p_[3];
		Eigen::Matrix<Scalar, 3, 1> p3_[3];
		std::vector<FeatureComplex> f_;
		typename TSurface::Model *model_;
		
		inline static void set(Eigen::Matrix<Scalar, 2, 1> &p, const TPPLPoint &tp) {
			p(0) = tp.x;
			p(1) = tp.y;
		}
		
		void compute(const std::vector<float> &radii);
		void subsample(const std::vector<float> &radii, const TVector &at, const Scalar r2, std::vector<Triangle> &res) const;
		std::complex<Scalar> kernel(const Scalar m, const Scalar n, const Scalar p) const;

		void print() const;
    private:
		Eigen::Matrix<Scalar, 2, 1> intersection_on_line(const TVector &at, const Scalar r2, const Eigen::Matrix<Scalar, 2, 1> &a, const Eigen::Matrix<Scalar, 2, 1> &b)  const;
		std::complex<Scalar> sub_kernel(const Scalar m, const Scalar n, const Scalar p, const Tri2D &tri) const;
		std::complex<Scalar> kernel_lin(const Scalar m, const Scalar n, const Scalar p, const Scalar x0, const Scalar y0, const Scalar y1, const Scalar d1, const Scalar d2) const;
		std::complex<Scalar> kernel_lin_tri(const Scalar m, const Scalar n, const Scalar p, const Tri2D &tri) const;

		inline Eigen::Matrix<Scalar, 3, 1> at(const Scalar x, const Scalar y) const {
			Eigen::Matrix<Scalar, 2, 1> p;
			p(0)=x;p(1)=y;
			return at(p);
		}

		inline Eigen::Matrix<Scalar, 3, 1> at(const Eigen::Matrix<Scalar, 2, 1> &p) const {
			Eigen::Matrix<Scalar, 3, 1> v;
			v(0) = p(0); v(1) = p(1);
			v(2) = model_->model(p(0),p(1));
			return v;
		}

		template<const int Degree>
		Scalar area(const Tri2D &tri) const {
			//TODO: at the moment kind of "approximation"
			const Eigen::Matrix<Scalar, 2, 1> mid2 = ((*tri.p_[0])+(*tri.p_[1])+(*tri.p_[2]))/3;
			Eigen::Matrix<Scalar, 3, 1> mid23, mid3;

			mid23(0) = mid2(0);
			mid23(1) = mid2(1);
			mid3 = mid23;
			mid3(2) =  (model_->model((*tri.p_[0])(0), (*tri.p_[0])(1))+model_->model((*tri.p_[1])(0), (*tri.p_[0])(1))+model_->model((*tri.p_[1])(0), (*tri.p_[0])(1)))/3;
			mid23(2) = model_->model(mid2(0),mid2(1));
			return (mid3 - mid23).squaredNorm();
		}
	};
	
    struct VectorWithParams {
      TVector v_;
      typename TSurface::TParam::VectorU *param_;
    };
    
    PTSurfaceList input_;
    std::vector<Triangle> triangulated_input_;
    
    TAffine transform_;
    EINVARAINCE invariance_;
    std::vector<float> radii_;	//descending sorted radius list (reuse of subsampled map)
    PResultVectorList result_;
    //int num_radius_, num_angle_;

    void generateKeypoints(std::vector<TVector> &keypoints) const;
    void subsample(const TVector &at, const Scalar r2, std::vector<Triangle> &res) const;
	
	/* DEBUG functions */
	pcl::PolygonMesh::Ptr dbg_triangles2mesh(const std::vector<Triangle> &res) const;
  };
}
