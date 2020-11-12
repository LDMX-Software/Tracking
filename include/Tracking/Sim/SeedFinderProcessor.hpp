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
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seed.hpp"
#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"


//ROOT forward declarations
class TH2D;
class TFile;

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
     *
     */
    void onProcessEnd() final override;
       

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


    //std::shared_ptr<ldmx::LdmxSpacePoint> convertSimHitToLdmxSpacePoint(const ldmx::SimTrackerHit& hit);
    ldmx::LdmxSpacePoint* convertSimHitToLdmxSpacePoint(const ldmx::SimTrackerHit& hit);

private:
    Acts::SpacePointGridConfig m_gridConf;
    Acts::SeedfinderConfig<ldmx::LdmxSpacePoint> m_config;
    Acts::SeedFilterConfig m_seedFilter_cfg;
    
    std::shared_ptr<Acts::Seedfinder<ldmx::LdmxSpacePoint> > m_seedFinder;
    std::shared_ptr<Acts::BinFinder<ldmx::LdmxSpacePoint> > bottomBinFinder;
    std::shared_ptr<Acts::BinFinder<ldmx::LdmxSpacePoint> > topBinFinder;  

    std::mt19937 m_randomEngine;
    std::normal_distribution<double> distN;  

    
    std::shared_ptr<TH2F>  h_SimHit_edep_vs_mom;
    std::shared_ptr<TFile> m_outFile;
    
    
}; // SeedFinderProcessor
    

} // namespace sim
} // namespace tracking

#endif // TRACKING_SIM_TRACKINGGEOMAKER_H_
