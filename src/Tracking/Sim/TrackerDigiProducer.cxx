
#include "Tracking/Sim/TrackerDigiProducer.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include "DD4hep/DetElement.h"
#include <iostream>

namespace tracking {
namespace sim {

TrackerDigiProducer::TrackerDigiProducer(const std::string &name,
                                         ldmx::Process &process)
    : ldmx::Producer(name, process) {}

TrackerDigiProducer::~TrackerDigiProducer() {}

void TrackerDigiProducer::onProcessStart() {

  detector_ = &detector();
  // Get the world detector element
  dd4hep::DetElement world{detector_->world()};
  std::cout << "World volume name: " << world.name() << std::endl;
  
}

void TrackerDigiProducer::configure(ldmx::Parameters &parameters) {}

void TrackerDigiProducer::produce(ldmx::Event &event) {}

} // namespace sim
} // namespace tracking

DECLARE_PRODUCER_NS(tracking::sim, TrackerDigiProducer)
