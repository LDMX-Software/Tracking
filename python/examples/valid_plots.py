<<<<<<< Updated upstream
import os
from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process("tracking")

p.libraries.append("libTracking.so")

#from LDMX.Tracking.python.validate import tracking_validate

#bugger = tracking_validate.validation_processor()
#bugger.debug = False

#p.sequence = [bugger]

#print(p.sequence)
=======
import os,math
from LDMX.Framework import ldmxcfg


p = ldmxcfg.Process("TrackerReco")
#p = ldmxcfg.Process("tracking")

p.libraries.append("libTracking.so")

p.detector = '/Users/emryspeets/sw/ldmx-sw/Detectors/data/ldmx-det-v12-dd4hep/detector.xml'


from LDMX.Tracking import tracking_validate
from LDMX.Tracking import tracking_geo
from LDMX.Tracking import tracking_vtx
from LDMX.Tracking import tracking_truthseeder

#now we do the truthseeder for electrons as done in SingleElectronsReco.py
ts_ele               = tracking_truthseeder.TruthSeedProcessor()
ts_ele.debug         = False
ts_ele.trk_coll_name = "RecoilTruthSeeds"
ts_ele.pdgIDs        = [11]
ts_ele.scoring_hits  = "TargetScoringPlaneHits"
ts_ele.z_min         = 4.4
ts_ele.track_id      = 1
#ts_ele.p_cut         = 3970. # In MeV
ts_ele.p_cut         = 0. # In MeV
ts_ele.pz_cut        = 0.

#smearing in u/v directions
uSmearing = 0.006  #mm
vSmearing = 0.1    #mm



#using geo 
geo  = tracking_geo.TrackingGeometryMaker("Recoil_TrackFinder")
geo.dumpobj = 0



#General for geo
####
geo.debug = False
geo.propagator_step_size = 1.  #mm
geo.propagator_maxSteps = 2000
geo.bfield = -1.5  #in T #From looking at the BField map
geo.const_b_field = False
geo.bfieldMap_ = "/Users/emryspeets/sw/data_ldmx/field_map/BmapCorrected3D_13k_unfolded_scaled_1.15384615385.dat"
#geo.bfieldMap_ = "/Users/pbutti/sw/data_ldmx/BMapConstant_1_5T.dat"




#CKF Options
geo.hit_collection="RecoilSimHits"
geo.out_trk_collection = "RecoilTracks"

#Target location for the CKF extrapolation
geo.seed_coll_name = "RecoilTruthSeeds"

geo.use_extrapolate_location = True  #false not supported anymore
geo.extrapolate_location  = [0.,0.,0.]  #ignored if use_extrapolate_location is False
geo.use_seed_perigee = False #overrides previous options and uses the seed perigee (can be used to compare with truth)

#smear the hits used for finding/fitting
geo.do_smearing = True;
geo.sigma_u = uSmearing
geo.sigma_v = vSmearing
geo.trackID = 1
geo.pdgID = 11

#setting up geo_tagger

geo_tagger  = tracking_geo.TrackingGeometryMaker("Tagger_TrackFinder")
geo_tagger.dumpobj = 0

#general for geo_tagger
geo_tagger.debug = False
geo_tagger.propagator_step_size = 2.  #mm
geo_tagger.bfield = -1.5  #in T #From looking at the BField map
geo_tagger.const_b_field = False
geo_tagger.bfieldMap_ = "/Users/emryspeets/sw/data_ldmx/field_map/BmapCorrected3D_13k_unfolded_scaled_1.15384615385.dat"
#geo.bfieldMap_ = "/Users/pbutti/sw/data_ldmx/BMapConstant_1_5T.dat"


#CKF Options for geo tagger
geo_tagger.hit_collection="TaggerSimHits"

#Target location for the CKF extrapolation
geo_tagger.seed_coll_name = "TaggerTruthSeeds"
geo_tagger.use_extrapolate_location = True  #false not supported anymore
geo_tagger.extrapolate_location  = [0.,0.,0.]  #ignored if use_extrapolate_location is False
geo_tagger.use_seed_perigee = False
geo_tagger.out_trk_collection = "TaggerTracks"

#smear the hits used for finding/fitting
geo_tagger.do_smearing = True
geo_tagger.sigma_u = uSmearing  #mm
geo_tagger.sigma_v = vSmearing    #mm
geo_tagger.trackID = 1
geo_tagger.pdgID = 11






bugger = tracking_validate.validation_processor()
bugger.debug = False


#setting up options for vaidation processor


bugger.propagator_step_size = 2.  #mm
bugger.bfield = -1.5  #in T #From looking at the BField map
bugger.const_b_field = False
bugger.bfieldMap_ = "/Users/emryspeets/sw/data_ldmx/field_map/BmapCorrected3D_13k_unfolded_scaled_1.15384615385.dat"


#some quick other stuff
#Target location for the CKF extrapolation
bugger.seed_coll_name = "TaggerTruthSeeds"
bugger.use_extrapolate_location = True  #false not supported anymore
bugger.extrapolate_location  = [0.,0.,0.]  #ignored if use_extrapolate_location is False
bugger.use_seed_perigee = False
bugger.out_trk_collection = "TaggerTracks"

#smear the hits used for finding/fitting
bugger.do_smearing = True
bugger.sigma_u = uSmearing  #mm
bugger.sigma_v = vSmearing    #mm
bugger.trackID = 1
bugger.pdgID = 11






p.sequence = [bugger]

#p.sequence = [ts_ele, geo, geo_tagger, bugger]#, vtx]
print(p.sequence)
>>>>>>> Stashed changes

#You need to copy this file to the LDMX_BASE area for the example to work!
#/nfs/slac/g/ldmx/data/mc20/v12/4.0GeV/v2.2.1-batch8/mc_v12-4GeV-1e-ecal_photonuclear_run1075000_t1600764279.root

<<<<<<< Updated upstream
p.inputFiles = [os.environ["LDMX_BASE"]+"/data_ldmx/single_e/mc_v12-4GeV-1e-inclusive_run10001_t1636673834.root"]
p.outputFiles = ["/Users/benjaminlawrence-sanderson/workdir/projects/LDMX/sw/data_ldmx/valid_plots.root"]

p.termLogLevel=0

=======
p.inputFiles = [os.environ["LDMX_BASE"]+"/data_ldmx/single_e/single_ele_tagger.root"]
print(p.inputFiles)
#p.keep = [
#    "drop .*SimHits.*", #drop all sim hits
#    "drop .*Ecal.*", #drop all ecal (Digis are not removed)
#    "drop .*Magnet*",
#    "drop .*Hcal.*",
#    "drop .*Scoring.*",
#    "drop .*SimParticles.*",
#    "drop .*TriggerPad.*",
#    "drop .*trig.*"
#    ]


p.outputFiles = ["/Users/emryspeets/sw/data_ldmx/validation_plots.root"]

p.termLogLevel=0
>>>>>>> Stashed changes
p.maxEvents = 1000
