#ifndef TRACKING_VALIDATIONPROCESSOR_H_
#define TRACKING_VALIDATIONPROCESSOR_H_

//--- Framework ---//
#include "Framework/Configure/Parameters.h"
#include "Framework/EventProcessor.h"

// --- Tracking --- //
#include "Tracking/Event/Track.h" //pulling important parameter definitions form this boy
#include "Tracking/Sim/BFieldXYZUtils.h"
#include "Tracking/Sim/TrackingUtils.h"


// --- ACTS --- //

// Utils and definitions
#include "Acts/Definitions/Units.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/EventData/TrackParameters.hpp"

//// Magfield
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/MagneticField/MagneticFieldProvider.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"

//// Propagator
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"

//// Geometry
#include "Acts/Surfaces/PerigeeSurface.hpp"

//Root
#include "TH1F.h"
#include "TFile.h"
#include "TLorentzVector.h"

////Propagator with void navigator
using VoidPropagator = Acts::Propagator<Acts::EigenStepper<>>;

namespace tracking {
namespace reco {


class validation_processor : public framework::Producer {
 public:

  //Constructor  
  validation_processor(const std::string &name, framework::Process &process);
    
  //Destructor
  ~validation_processor();

  void onProcessStart() final override;
  void onProcessEnd() final override;

  //configuring the processor
  void configure(framework::config::Parameters &parameters) final override;

  //running the processor
  void produce(framework::Event &event);
  void TaggerRecoilMonitoring(const std::vector<ldmx::Track>& TaggerTracks, const std::vector<ldmx::Track>& RecoilTracks);	




 private:
  
  bool debug_{false};

  //Event counters
  int nevents_{0};
  //  int Tracker_nevents_{0};  
  //  int Recoil_nevents_{0};

  //chi2
  double chi2_{0};

  //Track collection name
  std::string trk_c_name_1{"TaggerTracks"};
  std::string trk_c_name_2{"RecoilTracks"};

  //The propagator
  std::shared_ptr<VoidPropagator> propagator_;

  //Processing time counter
  double processing_time_{0.};


  ////   -- Monitoring Histograms --   ////
 
  //Initialize histogram maps for convenient access/saving 
  //  std::map<std::string,TH1F*> histos1d_;
  //  std::map<std::string,TH2F*> histos2d_;
 
  //d0 histograms
  TH1F* h_tagger_d0;
  TH1F* h_recoil_d0;
  TH1F* h_delta_d0; 

  //z0 histograms
  TH1F* h_tagger_z0;
  TH1F* h_recoil_z0;
  TH1F* h_delta_z0;

  //p histograms
  TH1F* h_tagger_p;
  TH1F* h_recoil_p;
  TH1F* h_delta_p;

  //phi histograms 
  TH1F* h_tagger_phi;
  TH1F* h_recoil_phi;
  TH1F* h_delta_phi;

  //theta histograms
  TH1F* h_tagger_theta;
  TH1F* h_recoil_theta;
  TH1F* h_delta_theta;
  
  // Comparitive Histograms 
  TH2F* h_delta_d0_vs_recoil_p;
  TH2F* h_delta_z0_vs_recoil_p;

  TH2F* h_td0_vs_rd0;
  TH2F* h_tz0_vs_rz0;


  //addl histos
  // component wise momentum

  TH1F* h_tagger_px;
  TH1F* h_tagger_py;
  TH1F* h_tagger_pz;


  TH1F* h_recoil_px;
  TH1F* h_recoil_py;
  TH1F* h_recoil_pz;


  //squared
  TH1F* h_tagger_px2;
  // TH1F* h_tagger_py2;
  // TH1F* h_recoil_px2;
  // TH1F* h_recoil_py2;

  TH1F* h_tagger_chi2;
  TH1F* h_recoil_chi2;

  //transverse momentum information 
  TH1F*  h_tagger_t_mom; 
  TH1F*  h_recoil_t_mom;
  TH1F*  h_delta_t_mom;

  //number of hits on track
  TH1F* h_tag_hits;
  TH1F* h_rec_hits;

  //nhits vs transverse mom
  TH2F*  h_tag_nhits_vs_pt;
  TH2F*  h_rec_nhits_vs_pt;

};

                  
} // namespace reco
} // namespace tracking

#endif // TRACKING_VALIDATIONPROCESSOR_H_
