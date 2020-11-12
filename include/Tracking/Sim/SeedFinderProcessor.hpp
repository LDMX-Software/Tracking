#ifndef TRACKING_SIM_SEEDFINDERPROCESSOR_H_
#define TRACKING_SIM_SEEDFINDERPROCESSOR_H_

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
//#include "Framework/Event.h" //needed??!
#include "Framework/Configure/Parameters.h"
#include "Framework/EventProcessor.h"

/*~~~~~~~~~~~~~~~*/
/*   Tracking    */
/*~~~~~~~~~~~~~~~*/
#include "Tracking/Sim/LdmxSpacePoint.hpp"


/*~~~~~~~~~~~~~~~*/
/*    STD C++    */
/*~~~~~~~~~~~~~~~*/

#include <iostream>
#include <random>


/*~~~~~~~~~~~~*/
/*    ACTS    */
/*~~~~~~~~~~~~*/
#include "Acts/Seeding/SpacePointGrid.hpp"
#include "Acts/Seeding/Seedfinder.hpp"


namespace tracking {
namespace sim {

class SeedFinderProcessor : public ldmx::Producer {

public:
  /**
   * Constructor.
   *
   * @param name The name of the instance of this object.
   * @param process The process running this producer.
   */
  SeedFinderProcessor(const std::string &name, ldmx::Process &process);

  /// Destructor
  ~SeedFinderProcessor();

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


    std::shared_ptr<ldmx::LdmxSpacePoint> convertSimHitToLdmxSpacePoint(const ldmx::SimTrackerHit& hit);

private:
    Acts::SpacePointGridConfig m_gridConf;
    Acts::SeedfinderConfig<ldmx::LdmxSpacePoint> m_config;
    //std::default_random_engine* m_def_random_engine;
    std::shared_ptr<std::normal_distribution<double> > distN;  
    
    
}; // SeedFinderProcessor
    

} // namespace sim
} // namespace tracking

#endif // TRACKING_SIM_TRACKINGGEOMAKER_H_
