#include "Tracking/Reco/PhoenixEventDumper.h"
#include "Tracking/Sim/TrackingUtils.h"

#include <fstream>

namespace tracking {
namespace reco {

PhoenixEventDumper::PhoenixEventDumper(const std::string& name, framework::Process& process)
    : TrackingGeometryUser(name,process) {}

void PhoenixEventDumper::onNewRun(const ldmx::RunHeader& rh) {

  tracking::sim::PropagatorStepWriter::Config writer_cfg;
  writer_cfg.filePath = "./propagation_steps.root";
  prop_writer_ = std::make_unique<tracking::sim::PropagatorStepWriter>(writer_cfg);
  

  //This should go to the Track Extrapolator

  // Setup a interpolated bfield map
  const auto map = std::make_shared<InterpolatedMagneticField3>(
      loadDefaultBField(field_map_,
                        default_transformPos,
                        default_transformBField));

  const auto stepper = Acts::EigenStepper<>{map};
  
  // Setup the navigator
  Acts::Navigator::Config navCfg{geometry().getTG()};
  navCfg.resolveMaterial = true;
  navCfg.resolvePassive = true;
  navCfg.resolveSensitive = true;
  navCfg.boundaryCheckLayerResolving = false;
  const Acts::Navigator navigator(navCfg);

  auto acts_loggingLevel = Acts::Logging::FATAL;
  propagator_ = std::make_unique<CkfPropagator>(stepper,
                                                navigator,
                                                Acts::getDefaultLogger("EVTDMPR",acts_loggingLevel));
  
  trk_extrap_ = std::make_shared<std::decay_t<decltype(*trk_extrap_)>>(*propagator_,
                                                                       geometry_context(),
                                                                       magnetic_field_context());
  
  
}

void PhoenixEventDumper::configure(framework::config::Parameters &parameters) {

  outfilename_ = parameters.getParameter<std::string>("outfilename","event.json");
  
  field_map_ = parameters.getParameter<std::string>("field_map");
  
  TaggerTracks_ = parameters.getParameter<std::string>("taggerTracks","");
  RecoilTracks_ = parameters.getParameter<std::string>("recoilTracks","");

  TaggerMeasurements_ = parameters.getParameter<std::string>("taggerMeasurements","");
  RecoilMeasurements_ = parameters.getParameter<std::string>("recoilMeasurements","");

  EcalHits_           = parameters.getParameter<std::string>("EcalRecHits","");
  HcalHits_           = parameters.getParameter<std::string>("HcalRecHits","");
  
  eventnr_ = parameters.getParameter<int>("eventnr",1);
  
}

void PhoenixEventDumper::produce(framework::Event & event) {
  
  nevents_++;

  // Skip if it's not the event we want to dump
  if (nevents_ != eventnr_)
    return;
  
  if (!event.exists(TaggerMeasurements_)) {
    std::cout<<"ERROR::"+TaggerMeasurements_+" doesn't exist in the event";
    return;
  }
  
  if (!event.exists(RecoilMeasurements_)) {
    std::cout<<"ERROR::"+RecoilMeasurements_+" doesn't exist in the event";
    return;
  }
  

  if (!event.exists(EcalHits_)) {
    std::cout<<"ERROR::"<< EcalHits_ << " doesn't exist in the event";
    return;
  }

  if (!event.exists(HcalHits_)) {
    std::cout<<"ERROR:: HCalHits Collection "<< HcalHits_ << " doesn't exist in the event";
    return;
  }


  //if (!event.exists(TaggerTracks_)) {
  //  std::cout<<"ERROR::"+TaggerTracks_+" doesn't exist in the event";
  //      return;
  //}

  
  if (!event.exists(RecoilTracks_)) {
    std::cout<<"ERROR::"+TaggerTracks_+" doesn't exist in the event";
    return;
  }


  //Get the tracking geometry from the conditions database
  auto tg{geometry()};
  
  
  auto tms{event.getCollection<ldmx::Measurement>(TaggerMeasurements_)};
  auto rms{event.getCollection<ldmx::Measurement>(RecoilMeasurements_)};

  auto ecalHits{event.getCollection<ldmx::EcalHit>(EcalHits_)};
  auto hcalHits{event.getCollection<ldmx::HcalHit>(HcalHits_)};

  auto recoilTracks{event.getCollection<ldmx::Track>(RecoilTracks_)};
  

  std::cout<<"Found " + TaggerMeasurements_ + " with size " + tms.size()<<std::endl;
  std::cout<<"Found " + RecoilMeasurements_ + " with size " + rms.size()<<std::endl;
  std::cout<<"Recoil tracks size "<< recoilTracks.size()<<std::endl;
    
  evtjson_ = {};
    
  evtjson_["LDMX_Event"]["event number"] = eventnr_;
  evtjson_["LDMX_Event"]["run number"]   = 0;
  
  float measSizeX = 2;
  float measSizeY = 2;
  float measSizeZ = 2;

  json jmeasurements_tagger = json::array();
  json jmeasurements_recoil = json::array();
  json jmeasurements_ecal   = json::array();
  json jmeasurements_hcal   = json::array();
  
  for (auto tmeas : tms) {

    //jsonf jmeas = jsonf::object({"type","Box"});
    auto meas_pos  = tmeas.getGlobalPosition();
    auto rot_meas_pos = meas_pos;

    // Rot back to LDMX Global
    rot_meas_pos[0] = meas_pos[1];
    rot_meas_pos[1] = meas_pos[2];
    rot_meas_pos[2] = meas_pos[0];
    
    json jmeas = json::object({{"type","Box"} , {"pos",{meas_pos}}});
    jmeasurements_tagger += jmeas;
  }

  for (auto rmeas : rms) {

    auto jloc = hitline(rmeas);    
    json jmeas = json::object({{"type","Line"} , {"pos",jloc} , {"color","0x7fff00"}});
    jmeasurements_recoil += jmeas;
  }
  
  
  // ECAL HITS
  
  for (auto ehit : ecalHits) {
    
    float posX   = ehit.getXPos();
    float posY   = ehit.getYPos();
    float posZ   = ehit.getZPos();
    
    float energy = ehit.getEnergy();
    
    std::vector<float> jloc{posX,posY,posZ, 5, 5, 5};
    
    //Define the position and the box size
    json jmeas = json::object({{"type","Box"}, {"pos",jloc}, {"color","0x3cb371"} });
    jmeasurements_ecal += jmeas;
    
  }


  //HCAL HITS

  for (auto hhit : hcalHits) {

    float posX = hhit.getXPos();
    float posY = hhit.getYPos();
    float posZ = hhit.getZPos();
    std::vector<float> jloc{posX,posY,posZ, 5, 5, 5};

    //Define the position and the box size
    json jmeas = json::object({{"type","Box"}, {"pos",jloc}, {"color","0x3cb371"} });
    jmeasurements_hcal += jmeas;
    
  }
  


  //Try to get tracks

  json jtracks_recoil = json::array();
  
  for (auto& trk : recoilTracks) {
    
    json jtrack = prepareTrack(event,
                               trk,
                               rms,
                               tg);
    
    // Negative track
    if (trk.getQoP() < 0 ) 
      jtrack["color"] = "0xf1c716";


    // Add the target track state for the track

    jtrack["d0"] = trk.getD0();
    jtrack["z0"] = trk.getZ0();
    jtrack["phi"] = trk.getPhi();
    jtrack["eta"] = -std::log(std::tan(trk.getTheta()/2));
    jtrack["qOverP"] = trk.getQoP();
    
    jtracks_recoil.push_back(jtrack);
    
  }
  
  
  if (tms.size() > 0)
    evtjson_["LDMX_Event"]["Hits"]["TaggerHits"]   = {jmeasurements_tagger};
  
  if (rms.size() > 0)
    evtjson_["LDMX_Event"]["Hits"]["RecoilHits"]   = jmeasurements_recoil;
  
  if (ecalHits.size() > 0)
    evtjson_["LDMX_Event"]["Hits"]["EcalRecoHits"] = jmeasurements_ecal;


  if (hcalHits.size() > 0)
    evtjson_["LDMX_Event"]["Hits"]["HcalRecoHits"] = jmeasurements_hcal;

  
  if (recoilTracks.size() > 0)
    evtjson_["LDMX_Event"]["Tracks"]["RecoilTracks"]  = jtracks_recoil;
  
  std::ofstream file(outfilename_);
  file<<std::setw(4) << evtjson_ << '\n';
  
} //produce


std::vector<double> PhoenixEventDumper::hitline(const ldmx::Measurement& meas) {
  
  // Load the acts Surface
  const Acts::Surface* meas_surface = tg.getSurface(meas.getLayerID());
  
  // Get the surface boundaries
  std::vector<double> bounds = meas_surface->bounds().values();
    
  double vmin = bounds[1];
  double vmax = bounds[3];
  
  // Position in local frame
  
  double local_u = meas.getLocalPosition()[0];
  
  // Location on the lower edge in local coordinates
  Acts::Vector2 lpos_0(local_u, vmin);   
  
  // Location on the upper edge in local coordinates
  Acts::Vector2 lpos_1(local_u, vmax);
  
  // Dummy momentum, not used
  Acts::Vector3 dummy(0.,0.,0.);
  
  // Global position of lower edge. Dummy is not used
  auto glob_0 = meas_surface->localToGlobal(geometry_context(),
                                            lpos_0,
                                            dummy);

  auto glob_1 = meas_surface->localToGlobal(geometry_context(),
                                            lpos_1,
                                            dummy);
  
  std::vector<double> jloc{glob_0(1), glob_0(2), glob_0(0), glob_1(1), glob_1(2), glob_1(0)};  // -- LINE
  
  return jloc;
}

json PhoenixEventDumper::prepareTrack(framework::Event& event,
                                      const ldmx::Track& track,
                                      const std::vector<ldmx::Measurement> measurements,
                                      const geo::TrackersTrackingGeometry&tg ) {

  //Prepare the outputs
  std::vector<std::vector<Acts::detail::Step>> propagationSteps;
  propagationSteps.reserve(1);

  //This holds the steps to be merged
  std::vector<std::vector<Acts::detail::Step>> tmpSteps;
  

  
  Acts::PropagatorOptions<ActionList, AbortList> pOptions(geometry_context(),
                                                          magnetic_field_context());
  pOptions.pathLimit = std::numeric_limits<double>::max();
  pOptions.loopProtection = false;
  auto& mInteractor = pOptions.actionList.get<Acts::MaterialInteractor>();
  mInteractor.multipleScattering = true;
  mInteractor.energyLoss         = true;
  mInteractor.recordInteractions = false;
  auto& sLogger = pOptions.actionList.get<Acts::detail::SteppingLogger>();
  sLogger.sterile = false;
  pOptions.maxStepSize = 10 * Acts::UnitConstants::mm;
  pOptions.maxSteps    = 100;
  pOptions.direction   = Acts::Direction::Forward;
  
  
  /// Using some short hands for Recorded Material
  using RecordedMaterial = Acts::MaterialInteractor::result_type;
  using RecordedMaterialTrack =
      std::pair<std::pair<Acts::Vector3, Acts::Vector3>, RecordedMaterial>;

   /// Finally the output of the propagation test
  using PropagationOutput =
      std::pair<std::vector<Acts::detail::Step>, RecordedMaterial>;

  PropagationOutput pOutput;

  // Loop over the states and the surfaces of the multi-trajectory and get
  // the arcs of helix from start to next surface

  
  std::vector<Acts::BoundTrackParameters> prop_parameters;


  for (auto& ts : track.getTrackStates()) {

    //Skip the one at the target for the moment

    if (ts.ts_type != ldmx::TrackStateType::Meas)
      continue;
    
    //Get the surface from the tracking geometry
    
    const Acts::Surface* ts_surface = tg.getSurface(ts.layerid);
    if (!ts_surface)
      std::cout<<"ERROR:: Retrieving the surface in PhoenixEventDumper"<<std::endl;

    Acts::BoundVector smoothed;
    smoothed << 
        ts.params[0],
        ts.params[1],
        ts.params[2],
        ts.params[3],
        ts.params[4],
        ts.params[5];

    Acts::BoundSymMatrix covMat = tracking::sim::utils::unpackCov(ts.cov);
    
    prop_parameters.push_back(Acts::BoundTrackParameters(ts_surface->getSharedPtr(),
                                                         smoothed,
                                                         covMat));
    
    
  }// get track states

  
  // Start from the first parameters
  // Propagate to next surface
  // Grab the next parameters
  // Propagate to the next surface..
  // The last parameters just propagate
  
  
  std::vector<Acts::detail::Step> steps;


  // Define the ECAL face surface

  double ecal_face = 247;
  auto ecal_surf = tracking::sim::utils::unboundSurface(ecal_face);
  
  //compute first the perigee to first surface:


  auto perigeeParameters = tracking::sim::utils::perigeeBoundParameters(track);
  auto result = propagator_->propagate(perigeeParameters,
                                       prop_parameters.at(0).referenceSurface(),
                                       pOptions);
  

  if (result.ok()) {
    const auto& resultValue = result.value();
    auto steppingResults =
        resultValue.template get<Acts::detail::SteppingLogger::result_type>();
    // Set the stepping result
    pOutput.first = std::move(steppingResults.steps);
    
    for (auto & step : pOutput.first)
      steps.push_back(std::move(step));
  }
  
  //follow now the trajectory

  for (int i_params = 0; i_params < prop_parameters.size(); i_params++) {
    if (i_params < prop_parameters.size() - 1) {
      auto result = propagator_->propagate(prop_parameters.at(i_params),
                                           prop_parameters.at(i_params+1).referenceSurface(),
                                           pOptions);

      if (result.ok()) {
        const auto& resultValue = result.value();
        auto steppingResults =
            resultValue.template get<Acts::detail::SteppingLogger::result_type>();
        // Set the stepping result
        pOutput.first = std::move(steppingResults.steps);
        
        for (auto & step : pOutput.first)
          steps.push_back(std::move(step));
        
        // Record the propagator steps
        //tmpSteps.push_back(std::move(pOutput.first));
      }
    }
    
    else {
      auto result = propagator_->propagate(prop_parameters.at(i_params),
                                           *(ecal_surf),
                                           pOptions);
      
      
      if (result.ok()) {
        const auto& resultValue = result.value();
        auto steppingResults =
            resultValue.template get<Acts::detail::SteppingLogger::result_type>();
        // Set the stepping result
        pOutput.first = std::move(steppingResults.steps);
        
        for (auto & step : pOutput.first)
          steps.push_back(std::move(step));
        
        // Record the propagator steps
        //tmpSteps.push_back(std::move(pOutput.first));
      }
    }
  }
  
  propagationSteps.push_back(steps);
  
  
  //This is called track by track, so only one track is coming out of here.
  
  return  prop_writer_->StepPosition(propagationSteps); 
  
} // prepare track

}
}


DECLARE_PRODUCER_NS(tracking::reco, PhoenixEventDumper)
