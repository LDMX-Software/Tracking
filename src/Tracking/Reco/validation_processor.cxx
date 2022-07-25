#include "validation_processor.h"
#include "TFile.h"

//for acts stuffs
#include "Acts/MagneticField/ConstantBField.hpp"

#include <cmath>
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
  //Emrys to do, make "folders" for each category


////  --  Primary Track Parameter Histos  --  ////


  //d0 histograms 
  double d0min = -2;
  double d0max = 2;
  
  h_tagger_d0  = new TH1F("h_tagger_d0","h_tagger_d0",400, d0min, d0max);
  h_recoil_d0  = new TH1F("h_recoil_d0","h_recoil_d0",400, d0min, d0max);
  h_delta_d0   = new TH1F("h_delta_d0","h_delta_d0",400, d0min, d0max);
  
  //z0 histograms
  double z0min = -2;
  double z0max = 2;
  
  h_tagger_z0    = new TH1F("h_tagger_z0","h_tagger_z0",400, z0min, z0max);
  h_recoil_z0  = new TH1F("h_recoil_z0","h_recoil_z0",400, z0min, z0max);
  h_delta_z0   = new TH1F("h_delta_z0","h_delta_z0",400, z0min, z0max);

  //p histograms  
  h_tagger_p = new TH1F("h_tagger_p","h_tagger_p",200,-1,7);
  h_recoil_p = new TH1F("h_recoil_p","h_recoil_p",200,-1,7);
  h_delta_p  = new TH1F("h_delta_p","h_delta_p",200,-1,7);

  //phi histograms
  h_tagger_phi   = new TH1F("h_tagger_phi","h_tagger_phi",100,-3.15,3.15);
  h_recoil_phi   = new TH1F("h_recoil_phi","h_recoil_phi",100,-3.15,3.15);
  h_delta_phi    = new TH1F("h_delta_phi","h_delta_phi",400,-0.1,0.1);

  //theta histograms
  h_tagger_theta = new TH1F("h_tagger_theta","h_tagger_theta",100,-3.15,3.15);
  h_recoil_theta = new TH1F("h_recoil_theta","h_recoil_theta",100,-3.15,3.15);
  h_delta_theta  = new TH1F("h_delta_theta","h_delta_theta",200,-0.1,0.1);

  //Comparison Histograms 
  h_delta_d0_vs_recoil_p = new TH2F("h_delta_d0_vs_recoil_p","h_delta_d0_vs_recoil_p",200,0,5,400,-1,1);
  h_delta_z0_vs_recoil_p = new TH2F("h_delta_z0_vs_recoil_p","h_delta_z0_vs_recoil_p",200,0,5,400,-1,1);

  h_td0_vs_rd0 = new TH2F("h_td0_vs_rd0","h_td0_vs_rd0",100,-40,40,100,-40,40);
  h_tz0_vs_rz0 = new TH2F("h_tz0_vs_rz0","h_tz0_vs_rz0",100,-40,40,100,-40,40);

 


////   -- addl histograms --   ////


//  QoP; done but on Ben's version


//  transverse momemtums (p_x, p_y, p_z); 
  h_tagger_px   = new TH1F("h_tagger_px","h_tagger_px",100,1,5); //expect peak centered on ~4GeV
  h_tagger_py   = new TH1F("h_tagger_py","h_tagger_py",100,-1,1); //expected ~0 
  h_tagger_pz   = new TH1F("h_tagger_pz","h_tagger_pz",100,-1,1); //expected ~0

  h_recoil_px   = new TH1F("h_recoil_px","h_recoil_px",100,0,8); //expect SINGLE peak centered on ~4GeV
  h_recoil_py   = new TH1F("h_recoil_py","h_recoil_py",100,-3.15,3.15); //expect 0
  h_recoil_pz   = new TH1F("h_recoil_pz","h_recoil_pz",100,-3.15,3.15); //expect 0



//  deltas between p_x., p_y;
  h_delta_tpx_vs_tpy =  new TH1F("h_delta_tpx_vs_tpy","h_delta_tpx_vs_tpy",100,-2,8);
  h_delta_rpx_vs_rpy =  new TH1F("h_delta_rpx_vs_rpy","h_delta_rpx_vs_rpy",100,-2,8);



//  p_x^2 p_y^2; 
  h_tagger_px2   = new TH1F("h_tagger_px2","h_tagger_px2",100,-2,20);
//  h_tagger_py2   = new TH1F("h_tagger_py2","h_tagger_py2",100,-3.15,3.15);
//  h_recoil_px2   = new TH1F("h_recoil_px2","h_recoil_px2",100,-3.15,3.15);
//  h_recoil_px2   = new TH1F("h_recoil_py2","h_recoil_py2",100,-3.15,3.15);





//  chi^2;
  h_tagger_chi2 = new TH1F("h_tagger_chi2","h_tagger_chi2",100,-1,6);
  h_recoil_chi2 = new TH1F("h_recoil_chi2","h_recoil_chi2",100,-1,6);


//transverse momenta information
  h_tagger_t_mom = new TH1F("h_tagger_t_mom","h_tagger_t_mom",100,-2,6);
  h_recoil_t_mom = new TH1F("h_recoil_t_mom","h_recoil_t_mom",100,-2,6);
  h_delta_t_mom  = new TH1F("h_delta_t_mom","h_delta_t_mom",100,-2,3);










//  chi^2 / [degrees of freedom] (dof = number of hits - 5? );
//  d_0, sig_d0 as function of momentum;
//  z_0, sig_z0 as function of momentum;  
//  p_t as function of number of hits 
//  plot sigma QoP as functon of momenta using 1. covariance 2. momenta (Y plot error on P, X plot p) (Y sigma QoP, X plot P) 


  
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

 
  //writing to d0 histos
  h_tagger_d0->Write();
  h_recoil_d0->Write();
  h_delta_d0->Write();

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

  


//adtnl plots 
  //px,py,pz
  h_tagger_px->Write();
  h_tagger_py->Write();
  h_tagger_pz->Write();

  h_recoil_px->Write();
  h_recoil_py->Write();
  h_recoil_pz->Write();

  //  deltas between p_x., p_y;
  h_delta_tpx_vs_tpy->Write();
  h_delta_rpx_vs_rpy->Write();
  
  //px^2 and py^2
  h_tagger_px2->Write();
//  h_tagger_py2->Write();
//  h_recoil_px2->Write();
//  h_recoil_py2->Write();

  //chi2
  h_tagger_chi2->Write();
  h_recoil_chi2->Write();

  //transverse momenta information


  h_tagger_t_mom->Write();
  h_recoil_t_mom->Write();
  h_delta_t_mom->Write();;








  //closing the file
  outfile->Close();
  delete outfile;
  

  std::cout<<"PROCESSOR:: "<<this->getName()<<"   AVG Time/Event: " <<processing_time_ / nevents_ << " ms"<<std::endl;  
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


  //Now we fill the histograms for each parameter
  
  //filling d0 histograms
  h_tagger_d0->Fill(t_d0);
  h_recoil_d0->Fill(r_d0);
  h_delta_d0->Fill(t_d0 - r_d0);

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





  



//addl plots 

//tagger and recoil component wise momentum
double t_px, t_py, t_pz, r_px, r_py, r_pz;


//where .getMomentum comes from Track.h
t_px = t_trk.getMomentum()[0]; 
t_py = t_trk.getMomentum()[1];
t_pz = t_trk.getMomentum()[2];

r_px = r_trk.getMomentum()[0];
r_py = r_trk.getMomentum()[1];
r_pz = r_trk.getMomentum()[2];


//Transverse Momentum --> Tp ~ sqrt(px^2 +py^2)
  double t_px2, t_py2, r_px2, r_py2;
  double tag_t_mom, rec_t_mom;

  t_px2 = pow(t_px,2);
  t_py2 = pow(t_py,2);
  r_px2 = pow(r_px,2);
  r_py2 = pow(r_py,2);

  tag_t_mom = sqrt(t_px2 + t_py2);
  rec_t_mom = sqrt(r_px2 + r_py2);


  //px,py,pz
  h_tagger_px->Fill(t_px);
  h_tagger_py->Fill(t_py);
  h_tagger_pz->Fill(t_pz);

  h_recoil_px->Fill(r_px);
  h_recoil_py->Fill(r_py);
  h_recoil_pz->Fill(r_pz);

  //  deltas between p_x., p_y;
  h_delta_tpx_vs_tpy->Fill(t_px - t_py);
  h_delta_rpx_vs_rpy->Fill(r_px - r_py);

  //px^2 and py^2
  h_tagger_px2->Fill(t_px2);
//  h_tagger_py2->Fill(pow(t_py,2));
//  h_recoil_px2->Fill(pow(r_px,2));
//  h_recoil_py2->Fill(pow(r_py,2));


  //transverse momentum
  h_tagger_t_mom->Fill(tag_t_mom);
  h_recoil_t_mom->Fill(rec_t_mom);

  h_delta_t_mom->Fill(sqrt(tag_t_mom - rec_t_mom));

  //chi2
  double t_chi2, r_chi2;
  t_chi2 = t_trk.getChi2();
  r_chi2 = r_trk.getChi2();
  h_tagger_chi2->Fill(t_chi2);
  h_recoil_chi2->Fill(r_chi2);



  //  std::cout<<"chi2sum="<<trajState.chi2Sum<<std::endl;



}




}//tracking
}//reco

DECLARE_PRODUCER_NS(tracking::reco, validation_processor)
