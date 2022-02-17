#ifndef TRACKING_SIM_FINDABLETRACKPROCESSOR_H_
#define TRACKING_SIM_FINDABLETRACKPROCESSOR_H_

//---< C++ >---//
#include <map>
#include <vector>

//---< Framework >---//
#include "Framework/Configure/Parameters.h"
#include "Framework/Event.h"
#include "Framework/EventProcessor.h"

//---< SimCore >---//
#include "SimCore/Event/SimTrackerHit.h"

namespace tracking::sim {

/**
 * @brief Processor used to find all particles that pass through a silicon
 *        tracker and leave hits consistent with a findable track.
 */
class FindableTrackProcessor : public framework::Producer {

public:
  /**
   * Constructor.
   *
   * @param name The name of the instance of this object.
   * @param process The process running this producer.
   */
  FindableTrackProcessor(const std::string &name, framework::Process &process);

  /// Destructor
  ~FindableTrackProcessor() = default;

  /**
   * Configure the processor using the user specified parameters.
   *
   * @param parameters Set of parameters used to configure this processor.
   */
  void configure(framework::config::Parameters &parameters) final override;

  /**
   * Run the processor and create a collection of results which
   * indicate if a charge particle can be found by the tracker.
   *
   * @param event The event to process.
   */
  void produce(framework::Event &event) final override;

private:
  /**
   * Create a hit map which associates a charged particle to it's
   * hits in the tracker.
   *
   * @param hits Collection of SimTrackerHits.
   */
  void createHitMap(const std::vector<ldmx::SimTrackerHit> &hits);

  /**
   * Given a set of hits, check if a sim particle is expected to fall
   * within the acceptance of the tracker.
   *
   * @param result The object used to encapsulate the results.
   * @param hitCount Vector indicating whether the nth layer of the
   *                 recoil tracker has a hit.
   */
  //void isFindable(FindableTrackResult &result, std::vector<int> hit_count);

  /// Map between a sim particle and it's hit array. 
  std::map<int, std::vector<int>> hit_map_;

  /// The input SimTrackerHit collection to smear
  std::string input_hit_coll_;

}; // FindableTrackProcessor
} // namespace tracking::sim

#endif // TRACKING_SIM_FINDABLETRACKPROCESSOR_H_
