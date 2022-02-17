#include "Tracking/Sim/FindableTrackProcessor.h"

//---< SimCore >---//
#include "SimCore/Event/SimParticle.h"

namespace tracking::sim {

FindableTrackProcessor::FindableTrackProcessor(const std::string &name,
                                               framework::Process &process)
    : framework::Producer(name, process) {}

void FindableTrackProcessor::configure(
    framework::config::Parameters &parameters) {}

void FindableTrackProcessor::produce(framework::Event &event) {

  // Get the collection of sim particles from the event
  auto particles{event.getMap<int, ldmx::SimParticle>("SimParticles")};

  // Get the collection of SimTrackerHits to process from the event.
  auto sim_hits{event.getCollection<ldmx::SimTrackerHit>(input_hit_coll_)};

  // Create the hit map
  createHitMap(sim_hits);

  // Loop through all sim particles and check which are findable
  int result_count = 0;
  // std::vector<FindableTrackResult> findableTrackResults;
  for (const auto & [track_id, particle] : particles) {

    // If the sim particle is neutral, skip it.
    if (abs(particle.getCharge()) != 1)
      continue;

    // If the particle hasn't been added to the hit map, skip it.
    if (hit_map_.count(track_id) == 0) continue; 

    // Create a result instance
    // findableTrackResults.emplace_back();

    // Set the sim particle associated with the result
    // findableTrackResults.back().setParticleTrackID(keyVal.first);

    // Check if the track is findable
    // this->isFindable(findableTrackResults.back(), hit_map_[keyVal.first]);
    // result_count++;
  }

  // Add the result to the collection
  // event.add("FindableTracks", findableTrackResults);
}

void FindableTrackProcessor::createHitMap(
    const std::vector<ldmx::SimTrackerHit> &hits) {

  // Clear the hit map to remove any previous relations
  hit_map_.clear();

  // Loop over all SimTrackerHits and check which layers, if any,
  // the sim particle deposited energy in.  A layer is defined as a
  // single sensor.
  std::vector<int> hit_count(10, 0);
  for (const auto hit : hits) {

    // Retrieve the Geant4 track ID associated with a SimTrackerHit. The
    // track ID will serve as a proxy for the MC particle.
    int track_id{hit.getTrackID()};

    // Add the track ID if it isn't in the map already
    if (hit_map_.count(track_id) == 0)
      hit_map_[track_id] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // Increment the hit count for the layer
    hit_map_[track_id][hit.getLayerID() - 1]++;
  }
}

/*
void FindableTrackProcessor::isFindable(FindableTrackResult &result,
                                        std::vector<int> hitCount) {

  //std::cout << "[ FindableTrackProcessor ]: Hit Vec [ ";
  //for (auto index : hitCount) {
  //    std::cout << index << ", ";
  //}
  //std::cout << " ]" << std::endl;

  // Count how many 3D stereo hits are created by this particle
  std::vector<int> hit3dVec{0, 0, 0, 0};
  int hit3dCount{0};
  for (int layerN = 0; layerN < 8; layerN += 2) {
    if (hitCount[layerN] * hitCount[layerN + 1] != 0) {
      hit3dCount++;
      hit3dVec[layerN / 2]++;
    }
  }
  //std::cout << "[ FindableTrackProcessor ]: "
  //          << "Total 3D Hits: " << hit3dCount << std::endl;

  // A track is considered findable if
  // 1) The first four stereo layers are hit
  // 2) Three of the first four layers are hit and an axial layer is hit
  // 3) Two of the first four layers are hit and both axial layers are hit
  bool trackFound{false};
  if (hit3dCount == 4) {
    result.setResult(FindableTrackResult::STRATEGY_4S, true);
    trackFound = true;
    std::cout << "[ FindableTrackProcessor ]: "
              << "Can be found using 4S strategy." << std::endl;
  }

  if ((hit3dVec[0] * hit3dVec[1] * hit3dVec[2] > 0) &&
      (hitCount[8] > 0 || hitCount[9] > 0)) {
    result.setResult(FindableTrackResult::STRATEGY_3S1A, true);
    trackFound = true;
    std::cout << "[ findabletrackprocessor ]: "
              << "can be found using 3s1a strategy." << std::endl;
  }

  if (hit3dVec[0] * hit3dVec[1] > 0 && (hitCount[8] > 0 && hitCount[9] > 0)) {
    result.setResult(FindableTrackResult::STRATEGY_2S2A, true);
    trackFound = true;
    std::cout << "[ findabletrackprocessor ]: "
              << "can be found using 2s2a strategy." << std::endl;
  }

  if (hitCount[8] > 0 && hitCount[9] > 0) {
    result.setResult(FindableTrackResult::STRATEGY_2A, true);
    trackFound = true;
    std::cout << "[ findabletrackprocessor ]: "
              << "can be found using 2a strategy." << std::endl;
  }

  if (hit3dVec[0] * hit3dVec[1] > 0) {
    result.setResult(FindableTrackResult::STRATEGY_2S, true);
    trackFound = true;
    std::cout << "[ findabletrackprocessor ]: "
              << "can be found using 2s strategy." << std::endl;
  }

  if (hit3dVec[0] * hit3dVec[1] * hit3dVec[2] > 0) {
    result.setResult(FindableTrackResult::STRATEGY_3S, true);
    trackFound = true;
    std::cout << "[ findabletrackprocessor ]: "
              << "can be found using 3s strategy." << std::endl;
  }

  if (!trackFound) {
    result.setResult(FindableTrackResult::STRATEGY_NONE, false);
  }
}*/
} // namespace tracking::sim

DECLARE_PRODUCER_NS(tracking::sim, FindableTrackProcessor)
