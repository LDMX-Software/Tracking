#include "Tracking/Sim/SeedFinderProcessor.hpp"


/*~~~~~~~~~~*/
/*   ROOT   */
/*~~~~~~~~~~*/

#include "TFile.h"
#include "TH2F.h"

using namespace ldmx;

namespace tracking{
    namespace sim {
        
        SeedFinderProcessor::SeedFinderProcessor(const std::string &name,
                                                 ldmx::Process &process)
            : 
            ldmx::Producer(name, process),
            m_randomEngine{1234},
            distN{0,1} 
{
    
    bottomBinFinder = std::make_shared<Acts::BinFinder<ldmx::LdmxSpacePoint> > (
        Acts::BinFinder<ldmx::LdmxSpacePoint>());

    topBinFinder = std::make_shared<Acts::BinFinder<ldmx::LdmxSpacePoint> > (
        Acts::BinFinder<ldmx::LdmxSpacePoint>());

}
        
        SeedFinderProcessor::~SeedFinderProcessor() {}
        
        void SeedFinderProcessor::onProcessStart() {
            
            h_SimHit_edep_vs_mom = std::make_shared<TH2F>("h_SimHit_edep_vs_mom","",100,0,4200,100,0,1);
            m_outFile = std::make_shared<TFile>("seederOutFile.root","RECREATE");
            
            
        }
        
        void SeedFinderProcessor::configure(ldmx::Parameters &parameters) {
        
            //Default configuration

            //Tagger r max
            m_config.rMax = 1000.;
            m_config.deltaRMin = 3.; 
            m_config.deltaRMax = 220.; 
            m_config.collisionRegionMin = -50; 
            m_config.collisionRegionMax =  50; 
            m_config.zMin = -300;
            m_config.zMax = 300.;
            m_config.maxSeedsPerSpM = 5;
    
            //More or less the max angle is something of the order of 50 / 600 (assuming that the track hits all the layers)
            //Theta for the seeder is like ATLAS eta, so it's 90-lambda.
            //Max lamba is of the order of ~0.1 so cotThetaMax will be 1./tan(pi/2 - 0.1) ~ 1.4. 
            m_config.cotThetaMax = 1.5;

            //cotThetaMax and deltaRMax matter to choose the binning in z. The bin size is given by cotThetaMax*deltaRMax
    
            m_config.sigmaScattering = 2.25;
            m_config.minPt = 500.;
            m_config.bFieldInZ = 1.5e-3;  // in kT
            m_config.beamPos = {0, 0}; // units?
            m_config.impactMax = 20.;
    
    
            m_gridConf.bFieldInZ = m_config.bFieldInZ;
            m_gridConf.minPt = m_config.minPt;
            m_gridConf.rMax = m_config.rMax;
            m_gridConf.zMax = m_config.zMax;
            m_gridConf.zMin = m_config.zMin;
            m_gridConf.deltaRMax = m_config.deltaRMax;
            m_gridConf.cotThetaMax = m_config.cotThetaMax;


            //The seed finder needs a seed filter instance
            //In the seed finder there is the correction for the beam axis, which you could ignore if you set the penalty for high impact parameters. So removing that in the seeder config.

            //Seed filter configuration
            m_seedFilter_cfg.impactWeightFactor = 0.;
    
            //For the moment no experiment dependent cuts are assigned to the filter
            m_config.seedFilter = std::make_unique<Acts::SeedFilter<LdmxSpacePoint>>(
                Acts::SeedFilter<LdmxSpacePoint>(m_seedFilter_cfg));
            
            m_seedFinder = std::make_shared<Acts::Seedfinder<ldmx::LdmxSpacePoint> >(m_config);
            
        }
        
        void SeedFinderProcessor::produce(ldmx::Event &event) {
            
            //Read in the Sim hits -- TODO choose which collection from config
            
            const std::vector<ldmx::SimTrackerHit> simHits = event.getCollection<ldmx::SimTrackerHit>("TaggerSimHits");

            std::vector<ldmx::LdmxSpacePoint* > ldmxsps;
            
            //Only convert simHits that have at least 0.05 edep
                        
            for (auto& simHit : simHits) {
                
                //simHit.Print();
                
                std::vector<double> hit_mom = simHit.getMomentum();
                float mom = std::sqrt(hit_mom[0]*hit_mom[0] + hit_mom[1]*hit_mom[1]+hit_mom[2]*hit_mom[2]);
                                
                h_SimHit_edep_vs_mom->Fill(mom,simHit.getEdep());
                
                if (simHit.getEdep() >  0.05) {
                    ldmxsps.push_back(convertSimHitToLdmxSpacePoint(simHit));
                }
                              
            }    

            //TODO:: I think here is not necessary to only select 3 points. 
            //Select the ones to be used for seeding
            //I'll use layer 3,5,7. I also filter out here the space points that are not matched with electrons (using the pID)
            
                        
            std::vector<const LdmxSpacePoint*> spVec;

            for (size_t isp = 0; isp < ldmxsps.size(); isp++) {
        
                if (ldmxsps[isp]->layer()==3 || ldmxsps[isp]->layer()==7 || ldmxsps[isp]->layer()==9) 
                    spVec.push_back(ldmxsps[isp]);
            }

            // create grid with bin sizes according to the configured geometry
            std::unique_ptr<Acts::SpacePointGrid<LdmxSpacePoint>> grid =
                Acts::SpacePointGridCreator::createGrid<LdmxSpacePoint>(m_gridConf);

            // covariance tool, sets covariances per spacepoint as required  --- for the moment 0 covariance
            auto ct = [=](const LdmxSpacePoint& sp, float, float,
                          float) -> Acts::Vector2D {
                return {sp.varianceR(), sp.varianceZ()};
            };

            // create the space point group
            auto spGroup = Acts::BinnedSPGroup<LdmxSpacePoint>(
                spVec.begin(), spVec.end(), ct, bottomBinFinder, topBinFinder,
                std::move(grid), m_config);
            

            // seed vector
            std::vector<std::vector<Acts::Seed<LdmxSpacePoint>>> seedVector;
            
            // find the seeds
            auto groupIt = spGroup.begin();
            auto endOfGroups = spGroup.end();
            
            for (; !(groupIt == endOfGroups); ++groupIt) {
                
                seedVector.push_back(m_seedFinder->createSeedsForGroup(
                                         groupIt.bottom(), groupIt.middle(), groupIt.top()));
            }
            
            int numSeeds = 0;
            for (auto& outVec : seedVector) {
                numSeeds += outVec.size();
            }
            
            /*
            std::cout<<spVec.size() << " hits, " << seedVector.size() << " regions, "
                     << numSeeds << " seeds"<<std::endl;
            */
                        
        }//produce


        void SeedFinderProcessor::onProcessEnd() { 
            
            m_outFile->cd();
            h_SimHit_edep_vs_mom->Write();
            m_outFile->Close();
                        
        }

        
        
        //This method converts a SimHit in a LdmxSpacePoint for the Acts seeder. It smears the SimHit according to a gaussian distribution. The seed is fixed.
        // (1) Rotate the coordinates into acts::seedFinder coordinates defined by B-Field along z axis [Z_ldmx -> X_acts, X_ldmx->Y_acts, Y_ldmx->Z_acts]
        // (2) Smear  the location of the simulated hits and store the smeared hits into LdmxSpacepoints, saving the error information.
        

        //sigma_u and sigma_v are more sensitive and less sensitive directions.
        //TODO::Move to shared pointers?!
        
        ldmx::LdmxSpacePoint* SeedFinderProcessor::convertSimHitToLdmxSpacePoint(const ldmx::SimTrackerHit& hit) {
            
            std::vector<float> sim_hit_pos = hit.getPosition();

            float sigma_u = 0.05; //50um
            float sigma_v = 0.25; //250um

            float ldmxsp_x = sim_hit_pos[2];
            float ldmxsp_y = sim_hit_pos[0];
            float ldmxsp_z = sim_hit_pos[1];

            //std::cout<<"PF:: DEBUG:: Pre-smearing::(x,y,z) "<<ldmxsp_x<<" " <<ldmxsp_y<<" "<<ldmxsp_z<<std::endl;

            
            double smear_factor = distN((m_randomEngine));
            ldmxsp_y = ldmxsp_y + smear_factor*sigma_v;
            
            smear_factor = distN((m_randomEngine));
            ldmxsp_z = ldmxsp_z + smear_factor*sigma_u;

            //std::cout<<"PF:: DEBUG:: After-smearing::(x,y,z) "<<ldmxsp_x<<" " <<ldmxsp_y<<" "<<ldmxsp_z<<std::endl;
            
            return new ldmx::LdmxSpacePoint(ldmxsp_x, ldmxsp_y,ldmxsp_z,
                                            hit.getTime(), hit.getLayerID(), sigma_u*sigma_u, sigma_v*sigma_v,hit.getID());
            
        }
                
    }// namespace sim
} //namespace tracking


DECLARE_PRODUCER_NS(tracking::sim, SeedFinderProcessor)
