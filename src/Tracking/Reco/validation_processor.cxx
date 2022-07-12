#include "validation_processor.h"
#include "TFile.h"

//for acts stuffs
#include "Acts/MagneticField/ConstantBField.hpp"


#include <chrono>

using namespace framework;

namespace tracking {
namespace reco {

validation_processor::validation_processor(const std::string &name,
                                 framework::Process &process)
    : framework::Producer(name,process) {}

validation_processor::~validation_processor() {}

void validation_processor::onProcessStart(){

////Monitoring plots////
 //range in GeV//
 
  // defining min/max for d0 and z0d 
  double d0min = -2;
  double d0max = 2;
  double z0min = -2;
  double z0max = 2;

  //Emrys to do, make "folders" for each category

  // Ben: change d0 to QoP here (naming doesn't matter here)
  //      leave d0min/max completely alone

  //d0 histograms 
  h_tagger_qop  = new TH1F("h_tagger_qop","h_tagger_qop",400, d0min, d0max);
  h_recoil_qop  = new TH1F("h_recoil_qop","h_recoil_qop",400, d0min, d0max);
  h_delta_qop   = new TH1F("h_delta_qop","h_delta_qop",400, d0min, d0max);
  
  //z0 histograms
  h_tagger_z0    = new TH1F("h_tagger_z0","h_tagger_z0",400, z0min, z0max);
  h_recoil_z0  = new TH1F("h_recoil_z0","h_recoil_z0",400, z0min, z0max);
  h_delta_z0   = new TH1F("h_delta_z0","h_delta_z0",400, z0min, z0max);

  //p histograms  
  h_tagger_p = new TH1F("h_tagger_p","h_tagger_p",200,-1,7);
  h_recoil_p = new TH1F("h_recoil_p","h_recoil_p",200,-1,7);
  h_delta_p  = new TH1F("h_delta_p","h_delta_p",200,-1,7);

  //phi histograms
  h_tagger_phi   = new TH1F("h_tagger_phi","h_tagger_phi",400,-0.2,0.2);
  h_recoil_phi   = new TH1F("h_recoil_phi","h_recoil_phi",400,-0.2,0.2);
  h_delta_phi    = new TH1F("h_delta_phi","h_delta_phi",400,-0.2,0.2);

  //theta histograms
  h_tagger_theta = new TH1F("h_tagger_theta","h_tagger_theta",200,-0.1,0.1);
  h_recoil_theta = new TH1F("h_recoil_theta","h_recoil_theta",200,-0.1,0.1);
  h_delta_theta  = new TH1F("h_delta_theta","h_delta_theta",200,-0.1,0.1);


  //Comparison Histograms 
  h_delta_d0_vs_recoil_p = new TH2F("h_delta_d0_vs_recoil_p","h_delta_d0_vs_recoil_p",200,0,5,400,-1,1);
  h_delta_z0_vs_recoil_p = new TH2F("h_delta_z0_vs_recoil_p","h_delta_z0_vs_recoil_p",200,0,5,400,-1,1);

  h_td0_vs_rd0 = new TH2F("h_td0_vs_rd0","h_td0_vs_rd0",100,-40,40,100,-40,40);
  h_tz0_vs_rz0 = new TH2F("h_tz0_vs_rz0","h_tz0_vs_rz0",100,-40,40,100,-40,40);




  //  detector_ = &detector();
  gctx_ = Acts::GeometryContext();
  bctx_ = Acts::MagneticFieldContext();
  
  auto localToGlobalBin_xyz = [](std::array<size_t, 3> bins,
                                 std::array<size_t, 3> sizes) {
    return (bins[0] * (sizes[1] * sizes[2]) + bins[1] * sizes[2] + bins[2]);  //xyz - field space
    //return (bins[1] * (sizes[2] * sizes[0]) + bins[2] * sizes[0] + bins[0]);    //zxy  
  };
  
  InterpolatedMagneticField3 map = makeMagneticFieldMapXyzFromText(std::move(localToGlobalBin_xyz), bfieldMap_,
                                                                   1. * Acts::UnitConstants::mm, //default scale for axes length
                                                                   1000. * Acts::UnitConstants::T, //The map is in kT, so scale it to T
                                                                   false, //not symmetrical
                                                                   true //rotate the axes to tracking frame
                                                                   );

  Acts::Vector3 b_field(0.,0.,-1.5 * Acts::UnitConstants::T);
  bField_ = std::make_shared<Acts::ConstantBField>(b_field);

  sp_interpolated_bField_ = std::make_shared<InterpolatedMagneticField3>(std::move(map));;
  
  std::cout<<"Check if nullptr::"<<sp_interpolated_bField_.get()<<std::endl;
  
  auto&& stepper_const = Acts::EigenStepper<>{bField_};
  auto&& stepper       = Acts::EigenStepper<>{sp_interpolated_bField_};  




// Set up propagator with void navigator

  //propagator_ = std::make_shared<VoidPropagator>(stepper);
 
  if (debug_)
    std::cout<<"Constant field propagator.."<<std::endl;
  propagator_ = std::make_shared<VoidPropagator>(stepper_const);

  
}

void validation_processor::configure(framework::config::Parameters &parameters){

  debug_                = parameters.getParameter<bool>("debug",false);

  bfieldMap_ = parameters.getParameter<std::string>("bfieldMap_", "/Users/emryspeets/sw/data_ldmx/field_map/BmapCorrected3D_13k_unfolded_scaled_1.15384615385.dat");


  trk_c_name_1       = parameters.getParameter<std::string>("trk_c_name_1","TaggerTracks");
  trk_c_name_2       = parameters.getParameter<std::string>("trk_c_name_2","RecoilTracks");

}


void validation_processor::produce(framework::Event &event) {

  nevents_++;
  auto start = std::chrono::high_resolution_clock::now();

  auto&& stepper = Acts::EigenStepper<>{sp_interpolated_bField_};
  
  // Set up propagator with void navigator
  propagator_ = std::make_shared<VoidPropagator>(stepper);



  
  
  //Retrieve the track collections
  const std::vector<ldmx::Track> TaggerTracks = event.getCollection<ldmx::Track>(trk_c_name_1);
  const std::vector<ldmx::Track> RecoilTracks = event.getCollection<ldmx::Track>(trk_c_name_2);

  



  //multiple track debugging
  if (debug_) {
    std::cout<<"Retrieved track collections"<<std::endl;
    std::cout<<"Track 1 size:"<<TaggerTracks.size()<<std::endl;
    std::cout<<"Track 2 size:"<<RecoilTracks.size()<<std::endl;
  }
  
  //Tagger + Recoil
  if (TaggerTracks.size() < 1 || RecoilTracks.size() < 1 )
     return; 


  
  //perigee surface created using Tagger, is common for recoil by construction
  std::shared_ptr<Acts::PerigeeSurface> perigeeSurface =
      Acts::Surface::makeShared<Acts::PerigeeSurface>(Acts::Vector3(TaggerTracks.front().getPerigeeX(),
                                                                    TaggerTracks.front().getPerigeeY(),
                                                                    TaggerTracks.front().getPerigeeZ()));



 ////Monitoring of Tagger and Recoil Tracks////

  TaggerRecoilMonitoring(TaggerTracks, RecoilTracks);

  // filling Tagger and Recoil vectors (paramVector)  with the important track parameters 
  // TaggerParamVec and covMat_T
  for (unsigned int iTaggerTrack = 0; iTaggerTrack < TaggerTracks.size() ; iTaggerTrack++) {
    
    Acts::BoundVector TaggerParamVec;
    TaggerParamVec <<
        TaggerTracks.at(iTaggerTrack).getD0(),
        TaggerTracks.at(iTaggerTrack).getZ0(),
        TaggerTracks.at(iTaggerTrack).getPhi(),
        TaggerTracks.at(iTaggerTrack).getTheta(),
        TaggerTracks.at(iTaggerTrack).getQoP(),
        TaggerTracks.at(iTaggerTrack).getT();
   
    // Tagger covMat
    Acts::BoundSymMatrix covMat_T =
        tracking::sim::utils::unpackCov(TaggerTracks.at(iTaggerTrack).getPerigeeCov());
        Acts::BoundTrackParameters(perigeeSurface, TaggerParamVec, std::move(covMat_T));

 }
      

   //RecoilParamVec and covMat_R
   for (unsigned int iRecoilTrack = 0; iRecoilTrack < RecoilTracks.size() ; iRecoilTrack++) {
  
    Acts::BoundVector RecoilParamVec;
    RecoilParamVec <<
        RecoilTracks.at(iRecoilTrack).getD0(),
        RecoilTracks.at(iRecoilTrack).getZ0(),
        RecoilTracks.at(iRecoilTrack).getPhi(),
        RecoilTracks.at(iRecoilTrack).getTheta(),
        RecoilTracks.at(iRecoilTrack).getQoP(),
        RecoilTracks.at(iRecoilTrack).getT();
    
  
    Acts::BoundSymMatrix covMat_R =
        tracking::sim::utils::unpackCov(RecoilTracks.at(iRecoilTrack).getPerigeeCov());
        Acts::BoundTrackParameters(perigeeSurface, RecoilParamVec, std::move(covMat_R));

}
    
    //setting up clock info
    auto end = std::chrono::high_resolution_clock::now();
    //long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
    auto diff = end-start; 
    processing_time_ += std::chrono::duration <double, std::milli> (diff).count();
  
}

void validation_processor::onProcessEnd() {

  //creating output file
  TFile* outfile = new TFile("validation_plots.root","RECREATE");
  outfile->cd();

  // Ben: changed the d0 below to qop

  //writing to d0 histos
  h_tagger_qop->Write();
  h_recoil_qop->Write();
  h_delta_qop->Write();

  //writing to z0 histos
  h_tagger_z0->Write();
  h_recoil_z0->Write();
  h_delta_z0->Write();
 
  //writing to p histos
  h_tagger_p->Write();
  h_recoil_p->Write();
  h_delta_p->Write();

  //writing to phi histos
  h_tagger_phi->Write();
  h_recoil_phi->Write();
  h_delta_phi->Write();

  //writing to theta histos
  h_tagger_theta->Write();
  h_recoil_theta->Write();
  h_delta_theta->Write();

  //writing to the comparison histos
  h_delta_d0_vs_recoil_p->Write();
  h_delta_z0_vs_recoil_p->Write();

  h_td0_vs_rd0->Write();
  h_tz0_vs_rz0->Write();



  //closing the file
  outfile->Close();
  delete outfile;
  

  std::cout<<"MONITORING PROCESSOR:: "<<this->getName()<<"   AVG Time/Event: " <<processing_time_ / nevents_ << " ms"<<std::endl;  
}


void validation_processor::TaggerRecoilMonitoring(const std::vector<ldmx::Track>& TaggerTracks,
                                      const std::vector<ldmx::Track>& RecoilTracks) {

  if (TaggerTracks.size() != 1 || RecoilTracks.size() != 1)
    return;

  ldmx::Track t_trk = TaggerTracks.at(0);
  ldmx::Track r_trk = RecoilTracks.at(0);

  //Defining track parameters [d0, z0, p, phi, theta]
 
  //d0 
  double t_d0, r_d0;
  t_d0 = t_trk.getD0();
  r_d0 = r_trk.getD0();

  //z0
  double t_z0, r_z0;
  t_z0 = t_trk.getZ0();
  r_z0 = r_trk.getZ0();

  //p , where we must convert from QoP information first 
  double t_p, r_p;
  t_p = t_trk.q() / t_trk.getQoP();
  r_p = r_trk.q() / r_trk.getQoP();

  //phi
  double t_phi, r_phi;
  t_phi = t_trk.getPhi();
  r_phi = r_trk.getPhi();

  //theta 
  double t_theta, r_theta;
  t_theta = t_trk.getTheta(); 
  r_theta = r_trk.getTheta();

  double t_qop, r_qop;
//to solve my error I think i need to do something like t_d0 = 
   
  // t_d0 = t_trk.getD0();
  // r_d0 = r_trk.getD0();

  // Ben: added the two lines below
  t_qop = t_trk.getQoP();
  r_qop = r_trk.getQoP();

  


  //Now we fill the histograms for each parameter
  
  // Ben: changed every instance of d0 to qop in the three uncommented lines below
  //filling d0 histograms
  h_tagger_qop->Fill(t_qop);
  h_recoil_qop->Fill(r_qop);
  h_delta_qop->Fill(t_qop - r_qop);

  //filling z0 histograms 
  h_tagger_z0->Fill(t_z0);
  h_recoil_z0->Fill(r_z0);
  h_delta_z0->Fill(t_p - r_z0);

  //filling p histograms   
  h_tagger_p->Fill(t_p);
  h_recoil_p->Fill(r_p);
  h_delta_p->Fill(t_p - r_p);
  
  //filling phi histograms
  h_tagger_phi->Fill(t_phi);
  h_recoil_phi->Fill(r_phi);
  h_delta_phi->Fill(t_phi - r_phi);

  //filling theta histograms
  h_tagger_theta->Fill(t_theta);
  h_recoil_theta->Fill(r_theta);
  h_delta_theta->Fill(t_theta - r_theta);

  //filling comparison histogram

  //differential plots
  h_delta_d0_vs_recoil_p->Fill(r_p, t_d0 - r_d0);
  h_delta_z0_vs_recoil_p->Fill(r_p, t_d0 - r_z0);

  //"beamspot"
  h_td0_vs_rd0->Fill(r_z0,t_d0);
  h_tz0_vs_rz0->Fill(r_d0,t_z0);


  //"pT"
  //TODO Transverse momentum should obtained orthogonal to the B-Field direction
  //This assumes to be along Z (which is not very accurate)

  //std::vector<double> r_mom = r_trk.getMomentum();
  //std::vector<double> t_mom = t_trk.getMomentum();

  //I assume to have a single photon being emitted in the target: I use momentum conservation
  //p_photon = p_beam - p_recoil






}




}//tracking
}//reco

DECLARE_PRODUCER_NS(tracking::reco, validation_processor)
