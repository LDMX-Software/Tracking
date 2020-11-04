#include "Tracking/Sim/TrackingGeometryMaker.hpp"
#include "Acts/Geometry/CuboidVolumeBuilder.hpp"
#include "Acts/Plugins/DD4hep/ActsExtension.hpp"



/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include "DD4hep/DetElement.h"
#include <iostream>

namespace tracking {
namespace sim {

TrackingGeometryMaker::TrackingGeometryMaker(const std::string &name,
                                             ldmx::Process &process)
    : ldmx::Producer(name, process) {}

TrackingGeometryMaker::~TrackingGeometryMaker() {}

void TrackingGeometryMaker::onProcessStart() {

  detector_ = &detector();
  // Get the world detector element
  dd4hep::DetElement world{detector_->world()};
  std::cout << "World volume name: " << world.name() << std::endl;
  Acts::CuboidVolumeBuilder cvb;
  std::vector<dd4hep::DetElement> subdetectors;
  collectSubDetectors_dd4hep(world,subdetectors);
}

void TrackingGeometryMaker::configure(ldmx::Parameters &parameters) {}

void TrackingGeometryMaker::produce(ldmx::Event &event) {}
    
    
    
    //#####################//
    
void TrackingGeometryMaker::collectSubDetectors_dd4hep(dd4hep::DetElement& detElement,
                                                       std::vector<dd4hep::DetElement>& subdetectors) {
    const dd4hep::DetElement::Children& children = detElement.children();
    for (auto& child : children) {
        dd4hep::DetElement childDetElement = child.second;
        std::cout<<"Child:: "<<childDetElement.name()<<std::endl;
        std::cout<<childDetElement.type()<<std::endl;
        
        //Check if an Acts extension is attached to the det Element (not necessary)
        Acts::ActsExtension* detExtension = nullptr;
        try {
            detExtension = childDetElement.extension<Acts::ActsExtension>();
            std::cout<<detExtension->toString()<<std::endl;
        }
        catch (std::runtime_error& e) {
            std::cout<<"Caught exception in "<<__PRETTY_FUNCTION__<<std::endl;
            continue;
        }
        
    }//children loop
}
    
} // namespace sim
} // namespace tracking

DECLARE_PRODUCER_NS(tracking::sim, TrackingGeometryMaker)
