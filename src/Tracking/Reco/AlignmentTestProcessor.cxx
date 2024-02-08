#include "Tracking/Reco/AlignmentTestProcessor.h"



namespace tracking::reco {

AlignmentTestProcessor::AlignmentTestProcessor(const std::string& name,
                                               framework::Process& process)

    : TrackingGeometryUser(name,process) {}


AlignmentTestProcessor::~AlignmentTestProcessor() {}

void AlignmentTestProcessor::configure(framework::config::Parameters &parameters) {
  
}


void AlignmentTestProcessor::onNewRun(const ldmx::RunHeader& rh) {

  // Load the tracking geometry
  auto tg(geometry());

  // Load the tracking geometry default transformations
  test_gctx_.loadTransformations(tg.layer_surface_map_);

  std::cout<<"Storing corrections to default transformations" <<std::endl;
  
  //100um in tu
  Acts::Vector3 deltaT{0.1  ,0., 0.};
  
  //10 mrad in rw
  Acts::Vector3 deltaR{0.   ,0., 0.01};
  test_gctx_.addAlignCorrection(2100, deltaT, deltaR);
  test_gctx_.addAlignCorrection(2101, deltaT, deltaR);

}


void AlignmentTestProcessor::produce(framework::Event& event) {
  
  std::cout<<"Check propagation of alignment corrections to surfaces"<<std::endl;
  std::cout<<"wrapping the ldmx test geometry context"<<std::endl;
  
  Acts::GeometryContext align_gctx(&test_gctx_);
  
  std::cout<<"Loading the tracking geometry"<<std::endl;
    
  auto tg(geometry());
  
  std::cout<<"Layers in Tracking Geometry"<<std::endl;
  for (auto entry : tg.layer_surface_map_) {
    std::cout<<entry.first<<std::endl;
    
    std::cout<<"Dumping surfaces information"<<std::endl;
    (entry.second)->toStream(align_gctx,std::cout);
    
  }
    
}


void AlignmentTestProcessor::onProcessStart() {
  
  
}


void AlignmentTestProcessor::onProcessEnd(){
}

}

DECLARE_PRODUCER_NS(tracking::reco, AlignmentTestProcessor)

