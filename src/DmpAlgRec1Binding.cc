/*
 *  $Id: DmpAlgRec1Binding.cc, 2015-04-29 20:05:53 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 03/09/2014
*/

#include <boost/python.hpp>
#include "DmpAlgRec1_Hits.h"

BOOST_PYTHON_MODULE(libDmpRec1_Hits){
  using namespace boost::python;

  class_<DmpAlgRec1_Hits,boost::noncopyable,bases<DmpVAlg> >("DmpAlgRec1_Hits",init<>())
     .def("SetPedestalFile",    &DmpAlgRec1_Hits::SetPedestalFile)
     .def("SetRelationFile",    &DmpAlgRec1_Hits::SetRelationFile)
     .def("SetMipsFile",    &DmpAlgRec1_Hits::SetMipsFile)
     .def("SetCutOverflow",    &DmpAlgRec1_Hits::SetCutOverflow)
     ;
}

