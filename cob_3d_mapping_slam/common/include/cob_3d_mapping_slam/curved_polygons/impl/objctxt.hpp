

#include <eigen3/Eigen/Dense>

template<typename _DOF6>
bool OBJCTXT<_DOF6>::registration(const OBJCTXT &ctxt, DOF6 &tf, typename DOF6::TYPE &probability_success_rate, typename DOF6::TYPE &probability_error_rate){

#ifdef DEBUG_
  ROS_INFO("registration %d %d",(int)objs_.size(), (int)ctxt.objs_.size());
#endif

  //1. correspondences
  used_cors_.clear();
  findCorrespondences(ctxt, used_cors_, tf);

  //2. optimize
  tf.getSource1()->reset();
  //tf = optimizeLink(tf, cors);
  tf = optimizeLink(tf, used_cors_, tf.getRotationVariance(), tf.getTranslationVariance(), tf.getRotation(), tf.getTranslation());

  std::cout<<tf<<"\n";

  //set result to time
  // if one registration failed
  // it can be retrieved over time
  tf.setVariance(  tf.getTranslationVariance(), tf.getTranslation(), tf.getRotationVariance(), tf.getRotation() );

  std::cout<<tf<<"\n";

  if((tf.getSource1()->getTranslationVariance()+tf.getSource1()->getRotationVariance())>0.3)
  {
    tf.getSource1()->reset();
    return false;
  }

  return true;
}


template<typename _DOF6>
bool OBJCTXT<_DOF6>::merge(const OBJCTXT &ctxt, const DOF6 &tf, std::map<typename OBJECT::Ptr,bool> &used_out, const bool only_merge)
{
  ++frames_;

  const size_t old = objs_.size();

  if(old>0 && (tf.getSource1()->getTranslationVariance()+tf.getSource1()->getRotationVariance())>0.3 )
    return false;

  ROS_INFO("add ctxt");
  std::cout<<tf<<"\n";

  for(size_t i=0; i<objs_.size(); i++)
  {
    objs_[i]->processed();
  }

  std::map<typename OBJECT::Ptr,std::vector<typename OBJECT::Ptr> > used;

  for(typename std::list<SCOR>::const_iterator it = used_cors_.begin(); it!=used_cors_.end(); it++)
  {
    //continue;
    if(!it->a || !it->b) continue;

    typename OBJECT::Ptr o=it->b->makeShared();
    o->transform(Eigen::Matrix3f::Identity(),-tf.getTranslation(),0,0);
    o->transform(((Eigen::Matrix3f)tf.getRotation()).transpose(),Eigen::Vector3f::Zero(),
                 tf.getRotationVariance(), tf.getTranslationVariance());
    //o->transform(((Eigen::Matrix3f)tf.getRotation()).transpose(),-tf.getTranslation());
    //
    //    if( //it->a->compatible(*o) &&
    //        (
    //            ///*((*it->a)|(*o)) ||*/ ((*it->a)&(*o)) ||
    //            //it->a->isReachable(*o,tf.getRotationVariance()+0.01f,tf.getTranslationVariance()+0.01f)
    //            //||
    //            //((*it->a)&(*o)) &&
    //            (it->a->getData().canMerge(o->getData()))
    //        ))
    //    {
    //      found = true;
    //
    //      typename OBJECT::TFLIST list = it->a->getTFList(*it->b, tf.getRotationVariance()+0.1, tf.getTranslationVariance()+0.1, tf.getRotation(), tf.getTranslation());
    //      if(1||list.size()>0 && it->a->isReachable(*it->b,tf.getRotationVariance(),tf.getTranslationVariance()))
    //      {
    ROS_INFO("update object");
    if(((*it->a) += *o)//||1
    )
    {
      //      }
      //      else
      //        ROS_INFO("ignoring object");

      if(used.find(it->b)==used.end())
        used[it->b] = std::vector<typename OBJECT::Ptr>();

    }
    used[it->b].push_back(it->a);
    //    }
  }

  for(size_t i=0; i<ctxt.objs_.size(); i++)
  {
    if(used_out.find(ctxt.objs_[i])!=used_out.end())
      continue;

    typename std::map<typename OBJECT::Ptr,std::vector<typename OBJECT::Ptr> >::const_iterator it = used.find(ctxt.objs_[i]);
    if(used.end() == it)
    {
      if(!only_merge)
      {
        typename OBJECT::Ptr o=ctxt.objs_[i]->makeShared();
        o->transform(Eigen::Matrix3f::Identity(),-tf.getTranslation(),0,0);
        o->transform(((Eigen::Matrix3f)tf.getRotation()).inverse(),Eigen::Vector3f::Zero(),
                     tf.getRotationVariance(), tf.getTranslationVariance());

        ROS_INFO("adding object");

        *this += o;
      }
    }
    else if(it->second.size()>1)
    {
      ROS_INFO("merging objects %d", (int)it->second.size());

      for(size_t k=1; k<it->second.size(); k++)
      {
        if(*(it->second)[0] += *(it->second)[k])
        {
          for(size_t j=0; j<objs_.size(); j++)
            if(objs_[j] == (it->second)[k])
            {
              objs_.erase(objs_.begin() + j);
              --j;
              ROS_INFO("erase object");
            }
        }
      }
    }
  }

  for(typename std::list<SCOR>::const_iterator it = used_cors_.begin(); it!=used_cors_.end(); it++)
  {
    used_out[it->b] = true;
  }

  update();

  return true;
}


template<typename _DOF6>
void OBJCTXT<_DOF6>::update()
{
  filter();
  updateBB();
}

template<typename _DOF6>
void OBJCTXT<_DOF6>::filter()
{
  for(size_t i=0; i<objs_.size(); i++)
  {
    size_t c = objs_[i]->getCreationCounter(), u = objs_[i]->getUsedCounter();

    if(c>10 && std::log(u)/std::log(c) < 0.2f)
    {
      objs_.erase(objs_.begin()+i);
      --i;
    }
  }
}

template<typename _DOF6>
void OBJCTXT<_DOF6>::updateBB()
{
  bb_.update(*this);
}
