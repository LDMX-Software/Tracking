#ifndef TRACKING_SIM_TRACKINGGEOMAKER_H_
#define TRACKING_SIM_TRACKINGGEOMAKER_H_

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Configure/Parameters.h"
#include "Framework/EventProcessor.h"

/*~~~~~~~~~~~~*/
/*   DD4hep   */
/*~~~~~~~~~~~~*/
#include "DD4hep/Detector.h"

namespace tracking {
namespace sim {

class TrackingGeometryMaker : public ldmx::Producer {

public:
  /**
   * Constructor.
   *
   * @param name The name of the instance of this object.
   * @param process The process running this producer.
   */
  TrackingGeometryMaker(const std::string &name, ldmx::Process &process);

  /// Destructor
  ~TrackingGeometryMaker();

  /**
   *
   */
  void onProcessStart() final override;

  /**
   * Configure the processor using the given user specified parameters.
   *
   * @param parameters Set of parameters used to configure this processor.
   */
  void configure(ldmx::Parameters &parameters) final override;

  /**
   * Run the processor and create a collection of results which
   * indicate if a charge particle can be found by the recoil tracker.
   *
   * @param event The event to process.
   */
  void produce(ldmx::Event &event);

  void collectSubDetectors_dd4hep(dd4hep::DetElement& detElement,
                                  std::vector<dd4hep::DetElement>& subdetectors);
  
private:
  /// The detector
  dd4hep::Detector* detector_{nullptr};
}; // TrackerDigiProducer

} // namespace sim
} // namespace tracking

#endif // TRACKING_SIM_TRACKINGGEOMAKER_H_
