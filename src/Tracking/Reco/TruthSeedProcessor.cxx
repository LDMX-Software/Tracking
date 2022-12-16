#include "Tracking/Reco/TruthSeedProcessor.h"

#include <chrono>

#include "SimCore/Event/SimParticle.h"
#include "TLorentzVector.h"

using namespace framework;

namespace tracking::reco {

TruthSeedProcessor::TruthSeedProcessor(const std::string &name,
                                       framework::Process &process)
    : framework::Producer(name, process) {}

TruthSeedProcessor::~TruthSeedProcessor() {}

void TruthSeedProcessor::onProcessStart() { gctx_ = Acts::GeometryContext(); }

void TruthSeedProcessor::configure(framework::config::Parameters &parameters) {
  out_trk_coll_name_ =
      parameters.getParameter<std::string>("trk_coll_name", "TruthSeeds");
  pdgIDs_ = parameters.getParameter<std::vector<int> >("pdgIDs", {11});
  scoring_hits_ = parameters.getParameter<std::string>(
      "scoring_hits", "TargetScoringPlaneHits_sim");
  sim_hits_ = parameters.getParameter<std::string>("sim_hits", "RecoilSimHits");
  n_min_hits_ = parameters.getParameter<int>("n_min_hits", 7);
  z_min_ = parameters.getParameter<double>("z_min", -999);  // mm
  track_id_ = parameters.getParameter<int>("track_id", -999);
  pz_cut_ = parameters.getParameter<double>("pz_cut", -9999);  // MeV
  p_cut_ = parameters.getParameter<double>("p_cut", 0.);
  p_cutMax_ = parameters.getParameter<double>("p_cutMax", 100000.);  // MeV
  k0_sel_ = parameters.getParameter<bool>("k0_sel", false);
  p_cutEcal_ = parameters.getParameter<double>("p_cutEcal", -1.);  // MeV
}

// Look at the scoring planes trackID
//  -> The trackID is the unique identification of the particles in the
//  generated event
//  -> 1 is the primary electron since it's the first one generated
//  -> For brehmsstrahlung / Energy Loss due to Ionization,  the outgoing
//  electron trackID is the same of the incoming
//  --- > The recoil electron will still trackID == 1
//  --- > Get the TargetScoringPlaneHits. Filter on z > 4.5mm, Get the
//  TrackID==1 and get the (x,y,z,px,py,pz).

// -> For eN interactions G4 is not able to follow the particle through the
// interaction. New particles will be created
// --- > Look through the recoil sim hits, get the track ID from those hits and
// *then* find the TargetScoringPlaneHits with that trackID to do truth
// matching.
// --- > There will be other trackIDs in the hits. Pick the ones that are
// electrons, and pick the highest energy ones.

// One thing that I can check is the endpoint of the trackID==1 particle to
// understand what kind of interaction the particle went through.

// Truth based initial track parameters

// In the case of Recoil tracking take only TrackID==1 and the generation
// surface is the obtained from the TargetScoringPlane

void TruthSeedProcessor::produce(framework::Event &event) {
  nevents_++;

  auto start = std::chrono::high_resolution_clock::now();

  // Tagger truth seeds
  std::vector<ldmx::Track> tagger_truth_seeds_{};

  // Retrieve the scoring plane hits
  const std::vector<ldmx::SimTrackerHit> scoring_hits =
      event.getCollection<ldmx::SimTrackerHit>(scoring_hits_);

  // Retrieve the scoring plane hits at the ECAL
  const std::vector<ldmx::SimTrackerHit> scoring_hits_ecal =
      event.getCollection<ldmx::SimTrackerHit>("EcalScoringPlaneHits");

  // Retrieve the particle map to get the tagger seeds
  auto particleMap{event.getMap<int, ldmx::SimParticle>("SimParticles")};
  ldmx::SimParticle gen_e = particleMap[1];
  Acts::Vector3 gen_e_pos{gen_e.getVertex().data()};
  Acts::Vector3 gen_e_mom{gen_e.getMomentum().data()};
  Acts::ActsScalar gen_e_time = 0.;

  gen_e_pos = tracking::sim::utils::Ldmx2Acts(gen_e_pos);
  gen_e_mom = tracking::sim::utils::Ldmx2Acts(gen_e_mom);

  // Grabbing only the electrons for the tagger truth seeds
  Acts::ActsScalar q = -1 * Acts::UnitConstants::e;
  Acts::FreeVector tagger_free_params =
      tracking::sim::utils::toFreeParameters(gen_e_pos, gen_e_mom, q);

  std::shared_ptr<const Acts::PerigeeSurface> tagger_gen_surface =
      Acts::Surface::makeShared<Acts::PerigeeSurface>(
          Acts::Vector3(tagger_free_params[Acts::eFreePos0],
                        tagger_free_params[Acts::eFreePos1],
                        tagger_free_params[Acts::eFreePos2]));

  // Transform the free parameters to the bound parameters
  auto tagger_bound_params = Acts::detail::transformFreeToBoundParameters(
                                 tagger_free_params, *tagger_gen_surface, gctx_)
                                 .value();

  // Check the parameters at the perigee
  std::shared_ptr<const Acts::PerigeeSurface> tagger_per_surface =
      Acts::Surface::makeShared<Acts::PerigeeSurface>(
          Acts::Vector3(-700, 0., 0.));

  auto tagger_bound_params_perigee =
      Acts::detail::transformFreeToBoundParameters(tagger_free_params,
                                                   *tagger_per_surface, gctx_)
          .value();

  ldmx_log(debug) << "(TRUTH) Tagger bound params to the -700,0.,0. surface"<<std::endl
                  << tagger_bound_params_perigee;
  
      
  // Try something reasonable
  Acts::BoundVector stddev;
  stddev[Acts::eBoundLoc0] = 500 * Acts::UnitConstants::um;
  stddev[Acts::eBoundLoc1] = 1 * Acts::UnitConstants::mm;
  stddev[Acts::eBoundTime] = 1000 * Acts::UnitConstants::ns;
  stddev[Acts::eBoundPhi] = 2 * Acts::UnitConstants::degree;
  stddev[Acts::eBoundTheta] = 2 * Acts::UnitConstants::degree;

  double p_tagger =
      sqrt(gen_e_mom(0) * gen_e_mom(0) + gen_e_mom(1) * gen_e_mom(1) +
           gen_e_mom(2) * gen_e_mom(2)) *
      Acts::UnitConstants::MeV;
  // 50% of uncertainty on momentum from seed fit // Passing 2 GeV, Expected:
  // 500 MeV for 4 GeV electrons
  double sigma_p_tagger = 0.5 * p_tagger * Acts::UnitConstants::GeV;
  double sigma_qop_tagger = (1. / p_tagger) * (1. / p_tagger) * sigma_p_tagger;
  stddev[Acts::eBoundQOverP] = sigma_qop_tagger;

  Acts::BoundSymMatrix tagger_bound_cov =
      stddev.cwiseProduct(stddev).asDiagonal();

  ldmx::Track tagger_seedTrack = ldmx::Track();
  tagger_seedTrack.setPerigeeLocation(tagger_free_params[Acts::eFreePos0],
                                      tagger_free_params[Acts::eFreePos1],
                                      tagger_free_params[Acts::eFreePos2]);
  tagger_seedTrack.setChi2(0.);
  tagger_seedTrack.setNhits(0.);
  tagger_seedTrack.setNdf(0);
  tagger_seedTrack.setNsharedHits(0.);

  std::vector<double> tagger_ldmx_cov;
  tracking::sim::utils::flatCov(tagger_bound_cov, tagger_ldmx_cov);
  tagger_seedTrack.setPerigeeParameters(
      tracking::sim::utils::convertActsToLdmxPars(tagger_bound_params));
  tagger_seedTrack.setPerigeeCov(tagger_ldmx_cov);

  tagger_truth_seeds_.push_back(tagger_seedTrack);

  event.add("TaggerTruthSeeds", tagger_truth_seeds_);

  if (scoring_hits.size() < 1) return;

  // Retrieve the sim hits in the tracker of interest
  const std::vector<ldmx::SimTrackerHit> sim_hits =
      event.getCollection<ldmx::SimTrackerHit>(sim_hits_);

  // TODO:: change to indices instead objects
  std::vector<ldmx::SimTrackerHit> selected_sp_hits;

  for (auto &t_sp_hit : scoring_hits) {
    // Clean some of the hits we don't want
    if (t_sp_hit.getPosition()[2] < z_min_) continue;

    // Check if the track_id was requested
    if (track_id_ > 0 && t_sp_hit.getTrackID() != track_id_) continue;

    // Check if we are requesting particular particles
    if (std::find(pdgIDs_.begin(), pdgIDs_.end(), t_sp_hit.getPdgID()) ==
        pdgIDs_.end())
      continue;

    Acts::Vector3 t_sp_p{t_sp_hit.getMomentum()[0], t_sp_hit.getMomentum()[1],
                         t_sp_hit.getMomentum()[2]};

    // p cut
    if (p_cut_ >= 0. && t_sp_p.norm() < p_cut_) continue;

    // p cut Max
    if (p_cut_ < 100000. && t_sp_p.norm() > p_cutMax_) continue;

    // pz cut
    if (pz_cut_ > -9999 && t_sp_p(2) < pz_cut_) continue;

    // Check the ecal scoring plane
    bool passEcalScoringPlane = true;

    if (p_cutEcal_ > 0) {  // only check if we care about it.

      for (auto &e_sp_hit : scoring_hits_ecal) {
        if (e_sp_hit.getTrackID() == t_sp_hit.getTrackID() &&
            e_sp_hit.getPdgID() == t_sp_hit.getPdgID()) {
          Acts::Vector3 e_sp_p{e_sp_hit.getMomentum()[0],
                               e_sp_hit.getMomentum()[1],
                               e_sp_hit.getMomentum()[2]};

          if (e_sp_p.norm() < p_cutEcal_) passEcalScoringPlane = false;

          // Skip the rest of the scoring plane hits since we already found the
          // track we care about
          break;

        }  // check that the hit belongs to the inital particle from the target
           // scoring plane hit
      }    // loop on Ecal scoring plane hits
    }      // pcutEcal

    if (!passEcalScoringPlane) continue;

    // add the point
    selected_sp_hits.push_back(t_sp_hit);
  }
  
  ldmx_log(debug) << "Selected scoring hits::" << selected_sp_hits.size();
  
  
  std::vector<ldmx::Track> truth_seeds_{};

  for (auto &sel_hit : selected_sp_hits) {
    int seed_particle_hits = 0;

    // Let's check how many hits this particle leaves in the recoil
    for (auto &sim_hit : sim_hits)
      if (sim_hit.getTrackID() == sel_hit.getTrackID()) seed_particle_hits++;

    if (seed_particle_hits < n_min_hits_) continue;

    Acts::Vector3 gen_pos{sel_hit.getPosition()[0], sel_hit.getPosition()[1],
                          sel_hit.getPosition()[2]};
    Acts::Vector3 gen_mom{sel_hit.getMomentum()[0], sel_hit.getMomentum()[1],
                          sel_hit.getMomentum()[2]};

    // Rotate into the ACTS Frame
    gen_pos = tracking::sim::utils::Ldmx2Acts(gen_pos);
    gen_mom = tracking::sim::utils::Ldmx2Acts(gen_mom);

    // Check the pdg id
    Acts::ActsScalar q = -1 * Acts::UnitConstants::e;

    // Electrons or muons
    if (abs(sel_hit.getPdgID()) == 11 || abs(sel_hit.getPdgID() == 13)) {
      q = sel_hit.getPdgID() > 0 ? -1 * Acts::UnitConstants::e
                                 : Acts::UnitConstants::e;
    }

    // Whatever else: pi, K ...
    else {
      q = sel_hit.getPdgID() > 0 ? Acts::UnitConstants::e
                                 : -1 * Acts::UnitConstants::e;
    }

    ldmx_log(debug) << "Preparing seed from hit:" << std::endl
                    << sel_hit.getPdgID() << " " << sel_hit.getPosition()[0] << ","
                    << sel_hit.getPosition()[1] << "," << sel_hit.getPosition()[2]
                    << std::endl
                    << q << " " << sel_hit.getMomentum()[0] << ","
                    << sel_hit.getMomentum()[1] << "," << sel_hit.getMomentum()[2];
    
    Acts::FreeVector free_params =
        tracking::sim::utils::toFreeParameters(gen_pos, gen_mom, q);
    std::shared_ptr<const Acts::PerigeeSurface> gen_surface =
        Acts::Surface::makeShared<Acts::PerigeeSurface>(Acts::Vector3(
            free_params[Acts::eFreePos0], free_params[Acts::eFreePos1],
            free_params[Acts::eFreePos2]));
    auto bound_params = Acts::detail::transformFreeToBoundParameters(
                            free_params, *gen_surface, gctx_)
                            .value();

    // Try something reasonable
    Acts::BoundVector recoil_stddev;
    recoil_stddev[Acts::eBoundLoc0] = 500 * Acts::UnitConstants::um;
    recoil_stddev[Acts::eBoundLoc1] = 1 * Acts::UnitConstants::mm;
    recoil_stddev[Acts::eBoundTime] = 1000 * Acts::UnitConstants::ns;
    recoil_stddev[Acts::eBoundPhi] = 2 * Acts::UnitConstants::degree;
    recoil_stddev[Acts::eBoundTheta] = 5 * Acts::UnitConstants::degree;

    double p = sqrt(gen_mom(0) * gen_mom(0) + gen_mom(1) * gen_mom(1) +
                    gen_mom(2) * gen_mom(2)) *
               Acts::UnitConstants::MeV;

    // 50% of uncertainty on momentum from seed fit // Passing 2 GeV, Expected:
    // 500 MeV for 4 GeV electrons
    double sigma_p = 0.5 * p * Acts::UnitConstants::GeV;
    double sigma_qop = (1. / p) * (1. / p) * sigma_p;

    recoil_stddev[Acts::eBoundQOverP] = sigma_qop;
    Acts::BoundSymMatrix bound_cov =
        recoil_stddev.cwiseProduct(recoil_stddev).asDiagonal();

    // Form the seed track for the event bus

    ldmx::Track seedTrack = ldmx::Track();
    seedTrack.setPerigeeLocation(free_params[Acts::eFreePos0],
                                 free_params[Acts::eFreePos1],
                                 free_params[Acts::eFreePos2]);
    seedTrack.setChi2(0.);
    seedTrack.setNhits(seed_particle_hits);
    seedTrack.setNdf(0);
    seedTrack.setNsharedHits(0.);

    std::vector<double> ldmx_cov;
    tracking::sim::utils::flatCov(bound_cov, ldmx_cov);
    seedTrack.setPerigeeParameters(
        tracking::sim::utils::convertActsToLdmxPars(bound_params));
    seedTrack.setPerigeeCov(ldmx_cov);

    truth_seeds_.push_back(seedTrack);

  }  // selected hits

  // Do the k0 selection.
  // std::vector<ldmx::Track> k0_decay_pions;

  // if (k0_sel_) {
  //   if (truth_seeds_.size() == 2 && truth_seeds_.at(0).charge() *
  //   truth_seeds_.at(1).charge() < 0) {
  //     TLorentzVector p1, p2;
  //    p1.SetXYZM(truth_seeds_.at(0).momentum()(0),
  //                truth_seeds_.at(0).momentum()(1),
  //                truth_seeds_.at(0).momentum()(2),
  //                pion_mass);
  //
  //     p2.SetXYZM(truth_seeds_.at(1).momentum()(0),
  //                truth_seeds_.at(1).momentum()(1),
  //                truth_seeds_.at(1).momentum()(2),
  //                pion_mass);
  //   }
  // }// k0 selection

  event.add(out_trk_coll_name_, truth_seeds_);

  auto end = std::chrono::high_resolution_clock::now();
  // long long microseconds =
  // std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
  auto diff = end - start;
  processing_time_ += std::chrono::duration<double, std::milli>(diff).count();
}

void TruthSeedProcessor::onProcessEnd() {
  std::cout << "PROCESSOR:: " << this->getName()
            << "   AVG Time/Event: " << processing_time_ / nevents_ << " ms"
            << std::endl;
}

}  // namespace tracking::reco

DECLARE_PRODUCER_NS(tracking::reco, TruthSeedProcessor)
