#include "validation_processor.h"
#include "TFile.h"
#include "TCanvas.h"

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

  ////  --  Primary Track Parameter Histos  --  ////

  //d0 histograms 
  double d0min = -2;
  double d0max = 2;
  
  h_tagger_d0  = new TH1F("h_tagger_d0","h_tagger_d0",400, d0min, d0max);
  h_recoil_d0  = new TH1F("h_recoil_d0","h_recoil_d0",400, d0min, d0max);
  h_delta_d0   = new TH1F("h_delta_d0","h_delta_d0",400, d0min, d0max);
 
  //info for the histogram map
  //  histos1d_["h_tagger_d0"] = h_tagger_d0;
  //  histos1d_["h_recoil_d0"] = h_recoil_d0;
  //  histos1d_["h_delta_d0"]  = h_delta_d0; 
 
  //z0 histograms
  double z0min = -2;
  double z0max = 2;
  
  h_tagger_z0  = new TH1F("h_tagger_z0","h_tagger_z0",400, z0min, z0max);
  h_recoil_z0  = new TH1F("h_recoil_z0","h_recoil_z0",400, z0min, z0max);
  h_delta_z0   = new TH1F("h_delta_z0","h_delta_z0",400, z0min, z0max);

  //  histos1d_["h_tagger_z0"] = h_tagger_z0;
  //  histos1d_["h_recoil_z0"] = h_recoil_z0;
  //  histos1d_["h_delta_z0"]  = h_delta_z0;


  //p histograms  
  h_tagger_p = new TH1F("h_tagger_p","h_tagger_p",200,-2,5);
  h_recoil_p = new TH1F("h_recoil_p","h_recoil_p",200,1,6);
  h_delta_p  = new TH1F("h_delta_p","h_delta_p",200,-3,3);

  //  histos1d_["h_tagger_p"] = h_tagger_p;
  //  histos1d_["h_recoil_p"] = h_recoil_p;
  //  histos1d_["h_delta_p"]  = h_delta_p;

  
  //phi histograms
  h_tagger_phi   = new TH1F("h_tagger_phi","h_tagger_phi",100,-3.15,3.15);
  h_recoil_phi   = new TH1F("h_recoil_phi","h_recoil_phi",100,-3.15,3.15);
  h_delta_phi    = new TH1F("h_delta_phi","h_delta_phi",400,-0.05,0.05);
  
  //  histos1d_["h_tagger_phi"] = h_tagger_phi;
  //  histos1d_["h_recoil_phi"] = h_recoil_phi;
  //  histos1d_["h_delta_phi"]  = h_delta_phi;
  

  //theta histograms
  h_tagger_theta = new TH1F("h_tagger_theta","h_tagger_theta",100,-3.15,3.15);
  h_recoil_theta = new TH1F("h_recoil_theta","h_recoil_theta",100,-3.15,3.15);
  h_delta_theta  = new TH1F("h_delta_theta","h_delta_theta",200,-0.02,0.02);

  //  histos1d_["h_tagger_theta"] = h_tagger_theta;
  //  histos1d_["h_recoil_theta"] = h_recoil_theta;
  //  histos1d_["h_delta_theta"]  = h_delta_theta;



  //Comparison Histograms (2D) 
  h_delta_d0_vs_recoil_p = new TH2F("h_delta_d0_vs_recoil_p","h_delta_d0_vs_recoil_p",200,0,5,400,-1,1);
  h_delta_z0_vs_recoil_p = new TH2F("h_delta_z0_vs_recoil_p","h_delta_z0_vs_recoil_p",200,0,5,400,-1,1);
  h_td0_vs_rd0 = new TH2F("h_td0_vs_rd0","h_td0_vs_rd0",100,-40,40,100,-40,40);
  h_tz0_vs_rz0 = new TH2F("h_tz0_vs_rz0","h_tz0_vs_rz0",100,-40,40,100,-40,40);


  //  histos2d_["h_delta_d0_vs_recoil_p"] = h_delta_d0_vs_recoil_p;
  //  histos2d_["h_delta_z0_vs_recoil_p"] = h_delta_z0_vs_recoil_p;
  //  histos2d_["h_td0_vs_rd0"]           = h_td0_vs_rd0;
  //  histos2d_["h_tz0_vs_rz0"]           = h_tz0_vs_rz0;


  //  transverse momemtums (p_x, p_y, p_z); 
  h_tagger_px   = new TH1F("h_tagger_px","h_tagger_px",400,1,5); //expect peak centered on ~4GeV
  h_tagger_py   = new TH1F("h_tagger_py","h_tagger_py",400,-0.15, 0.15); //expected ~0 
  h_tagger_pz   = new TH1F("h_tagger_pz","h_tagger_pz",400,-0.15, 0.15); //expected ~0

  h_recoil_px   = new TH1F("h_recoil_px","h_recoil_px",400,0,6); //expect SINGLE peak centered on ~4GeV
  h_recoil_py   = new TH1F("h_recoil_py","h_recoil_py",400,-0.15,0.15); //expect 0
  h_recoil_pz   = new TH1F("h_recoil_pz","h_recoil_pz",400,-0.15,0.15); //expect 0



  //  histos1d_["h_tagger_px"] = h_tagger_px;
  //  histos1d_["h_tagger_py"] = h_tagger_py;
  //  histos1d_["h_tagger_pz"] = h_tagger_pz;
  //
  //  histos1d_["h_recoil_px"] = h_recoil_px;
  //  histos1d_["h_recoil_py"] = h_recoil_py;
  //  histos1d_["h_recoil_pz"] = h_recoil_pz;




  //  p_x^2 p_y^2; 
  h_tagger_px2   = new TH1F("h_tagger_px2","h_tagger_px2",400,-2,20);
  //  h_tagger_py2   = new TH1F("h_tagger_py2","h_tagger_py2",100,-3.15,3.15);
  //  h_recoil_px2   = new TH1F("h_recoil_px2","h_recoil_px2",100,-3.15,3.15);
  //  h_recoil_px2   = new TH1F("h_recoil_py2","h_recoil_py2",100,-3.15,3.15);

  //  histos1d_["h_tagger_px2"] = h_tagger_px2;
  //  histos1d_["h_recoil_py2"] = h_rec_theta;
  //  histos1d_["h_delta_theta"]  = h_del_theta;



  //  chi^2;
  h_tagger_chi2 = new TH1F("h_tagger_chi2","h_tagger_chi2",100,0,12);
  h_recoil_chi2 = new TH1F("h_recoil_chi2","h_recoil_chi2",100,0,12);

  //  histos1d_["h_tagger_chi2"] = h_tagger_chi2;
  //  histos1d_["h_recoil_chi2"] = h_recoil_chi2;


  //transverse momenta information
  h_tagger_t_mom = new TH1F("h_tagger_t_mom","h_tagger_t_mom",400, 0, 1);
  h_recoil_t_mom = new TH1F("h_recoil_t_mom","h_recoil_t_mom",400, -0.1, 0.1);
  h_delta_t_mom  = new TH1F("h_delta_t_mom","h_delta_t_mom",400,0,1);

  //  histos1d_["h_tagger_t_mom"] = h_tagger_t_mom;
  //  histos1d_["h_recoil_t_mom"] = h_recoil_t_mom;
  //  histos1d_["h_delta_t_mom"]  = h_delta_t_mom;



  //number of hits on track 
  h_tag_hits  = new TH1F("h_tag_hits","h_tag_hits",400, 0, 15);
  h_rec_hits  = new TH1F("h_rec_hits","h_rec_hits",400, 0, 15);

  //  histos1d_["h_tag_hits"] = h_tag_hits;
  //  histos1d_["h_rec_hits"] = h_rec_hits;




//  chi^2 / [degrees of freedom] (dof = number of hits - 5? );
//  d_0, sig_d0 as function of momentum;
//  z_0, sig_z0 as function of momentum;  



  //transverse momentum vs number of hits --- 2D
  h_tag_nhits_vs_pt = new TH2F("h_tag_nhits_vs_pt","h_tag_nhits_vs_pt",400,0,15,400,0,5);
  h_rec_nhits_vs_pt = new TH2F("h_rec_nhits_vs_pt","h_rec_nhits_vs_pt",400,0,15,400,0,5);
  

  //  histos2d_["h_tag_nhits_vs_pt"] = h_tag_nhits_vs_pt;
  //  histos2d_["h_rec_nhits_vs_pt"] = h_rec_nhits_vs_pt;


  //  plot sigma QoP as functon of momenta using 1. covariance 2. momenta (Y plot error on P, X plot p) (Y sigma QoP, X plot P) 

  
}



void validation_processor::configure(framework::config::Parameters &parameters){

  debug_                = parameters.getParameter<bool>("debug",false);

  trk_c_name_1       = parameters.getParameter<std::string>("trk_c_name_1","TaggerTracks");
  trk_c_name_2       = parameters.getParameter<std::string>("trk_c_name_2","RecoilTracks");

}

void validation_processor::produce(framework::Event &event) {

  nevents_++;
  auto start = std::chrono::high_resolution_clock::now();

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
// std::shared_ptr<Acts::PerigeeSurface> perigeeSurface =
//      Acts::Surface::makeShared<Acts::PerigeeSurface>(Acts::Vector3(TaggerTracks.front().getPerigeeX(),
//                                                                    TaggerTracks.front().getPerigeeY(),
//                                                                    TaggerTracks.front().getPerigeeZ()));


  ////Monitoring of Tagger and Recoil Tracks////
  TaggerRecoilMonitoring(TaggerTracks, RecoilTracks);

      
    
   //setting up clock info
   auto end = std::chrono::high_resolution_clock::now();
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
  
  //px,py,pz
  h_tagger_px->Write();
  h_tagger_py->Write();
  h_tagger_pz->Write();

  h_recoil_px->Write();
  h_recoil_py->Write();
  h_recoil_pz->Write();
  
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
  h_delta_t_mom->Write();

  //number of hits on track
  h_tag_hits->Write();
  h_rec_hits->Write();

  //pt as function of nhits
  h_tag_nhits_vs_pt->Write();
  h_rec_nhits_vs_pt->Write();

  //closing the file
  outfile->Close();
  delete outfile;
  
  //print update
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

  //differential plots
  h_delta_d0_vs_recoil_p->Fill(r_p, t_d0 - r_d0);
  h_delta_z0_vs_recoil_p->Fill(r_p, t_d0 - r_z0);

  //"beamspot"
  h_td0_vs_rd0->Fill(r_z0,t_d0);
  h_tz0_vs_rz0->Fill(r_d0,t_z0);

   
  //tagger and recoil component wise momentum
  double t_px, t_py, t_pz, r_px, r_py, r_pz;

  //where .getMomentum comes from Track.h
  t_px = t_trk.getMomentum()[0]; 
  t_py = t_trk.getMomentum()[1];
  t_pz = t_trk.getMomentum()[2];

  r_px = r_trk.getMomentum()[0];
  r_py = r_trk.getMomentum()[1];
  r_pz = r_trk.getMomentum()[2];

  //Transverse Momentum --> Tp ~ sqrt(py^2 +pz^2)
  double t_px2, t_py2, t_pz2,  r_px2, r_py2, r_pz2;
  double tag_t_mom, rec_t_mom;

  t_px2 = pow(t_px,2);
  t_py2 = pow(t_py,2);
  t_pz2 = pow(t_pz,2);
  r_px2 = pow(r_px,2);
  r_py2 = pow(r_py,2);
  r_py2 = pow(r_pz,2);
  tag_t_mom = sqrt(t_pz2 + t_py2);
  rec_t_mom = sqrt(r_pz2 + r_py2);

  //px,py,pz
  h_tagger_px->Fill(t_px);
  h_tagger_py->Fill(t_py);
  h_tagger_pz->Fill(t_pz);

  h_recoil_px->Fill(r_px);
  h_recoil_py->Fill(r_py);
  h_recoil_pz->Fill(r_pz);


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


  // nhits on track for tag and recoil 
  double tag_hit, rec_hit;
  tag_hit = t_trk.getNhits();
  rec_hit = r_trk.getNhits();

  h_tag_hits->Fill(tag_hit);
  h_rec_hits->Fill(rec_hit);  

  // hits on track vs t momentum
  h_tag_nhits_vs_pt->Fill(tag_hit, tag_t_mom);
  h_rec_nhits_vs_pt->Fill(rec_hit, rec_t_mom);

}

}//tracking
}//reco

DECLARE_PRODUCER_NS(tracking::reco, validation_processor)
