#include "Tracking/Sim/SeedFinderProcessor.hpp"


/*~~~~~~~~~~*/
/*   ACTS   */
/*~~~~~~~~~~*/


//#include "Acts/Seeding/SeedFilter.hpp"
//#include "Acts/Seeding/Seed.hpp"
//#include "Acts/Seeding/BinFinder.hpp"
//#include "Acts/Seeding/BinnedSPGroup.hpp"

using namespace ldmx;

namespace tracking{
    namespace sim {
        
SeedFinderProcessor::SeedFinderProcessor(const std::string &name,
                                             ldmx::Process &process)
    : ldmx::Producer(name, process) {}

SeedFinderProcessor::~SeedFinderProcessor() {}

void SeedFinderProcessor::onProcessStart() {
    
    //m_def_random_engine = std::make_shared<std::default_random_engine>(1234);
    //m_def_random_engine = new std::default_random_engine(1234);
    distN = std::make_shared<std::normal_distribution<double> >(0.,1.);
    
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
    
}
        
        void SeedFinderProcessor::produce(ldmx::Event &event) {
            
            //Read in the Sim hits -- TODO choose which collection from config
            
            const std::vector<ldmx::SimTrackerHit> simHits = event.getCollection<ldmx::SimTrackerHit>("TaggerSimHits");

            //std::vector<ldmx::LdmxSpacePoint> ldmxsps;
            
            for (auto& simHit : simHits) {
                
                simHit.Print();
                convertSimHitToLdmxSpacePoint(simHit);
                //ldmxsps.push_back(ldmxsp);
                              
            }    
            
        }//produce
        
        
        //This method converts a SimHit in a LdmxSpacePoint for the Acts seeder. It smears the SimHit according to a gaussian distribution. The seed is fixed.
        // (1) Rotate the coordinates into acts::seedFinder coordinates defined by B-Field along z axis [Z_ldmx -> X_acts, X_ldmx->Y_acts, Y_ldmx->Z_acts]
        // (2) Smear  the location of the simulated hits and store the smeared hits into LdmxSpacepoints, saving the error information.
        

        //sigma_u and sigma_v are more sensitive and less sensitive directions.
        
        std::shared_ptr<ldmx::LdmxSpacePoint> SeedFinderProcessor::convertSimHitToLdmxSpacePoint(const ldmx::SimTrackerHit& hit) {
            
            std::vector<float> sim_hit_pos = hit.getPosition();

            float sigma_u = 0.05; //50um
            float sigma_v = 0.25; //250um

            float ldmxsp_x = sim_hit_pos[2];
            float ldmxsp_y = sim_hit_pos[0];
            float ldmxsp_z = sim_hit_pos[1];

            std::cout<<"PF:: DEBUG:: Pre-smearing::(x,y,z) "<<ldmxsp_x<<" " <<ldmxsp_y<<" "<<ldmxsp_z<<std::endl;

            std::default_random_engine generator;
            std::normal_distribution<float> distribution(0.0, 1.0);

            float smear_factor = distribution(generator);
            
            
            //double smear_factor = distN(m_def_random_engine);
            ldmxsp_y = ldmxsp_y + smear_factor*sigma_v;
            
            smear_factor = distribution(generator);
            ldmxsp_z = ldmxsp_z + smear_factor*sigma_u;

            std::cout<<"PF:: DEBUG:: After-smearing::(x,y,z) "<<ldmxsp_x<<" " <<ldmxsp_y<<" "<<ldmxsp_z<<std::endl;
            
            
        }
        
    

        
    }// namespace sim
} //namespace tracking


DECLARE_PRODUCER_NS(tracking::sim, SeedFinderProcessor)
