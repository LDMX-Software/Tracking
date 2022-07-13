import os,math
from LDMX.Framework import ldmxcfg


p = ldmxcfg.Process("TrackerReco")
p.libraries.append("libTracking.so")
p.detector = os.environ["LDMX_BASE"] + '/ldmx-sw/Detectors/data/ldmx-det-v12-dd4hep/detector.xml'


from LDMX.Tracking import tracking_validate



bugger = tracking_validate.validation_processor()
bugger.dumpobj = 0

#change to True for debugging
bugger.debug = False





p.sequence = [bugger]
print(p.sequence)


p.inputFiles = [os.environ["LDMX_BASE"]+"/data_ldmx/single_e/single_ele_tagger.root"]
p.inputFiles = [
    # os.environ["LDMX_BASE"]+"/data_ldmx/single_e/mc_v12-4GeV-1e-inclusive_run10001_t1636673834.root",
    # os.environ["LDMX_BASE"]+"/data_ldmx/single_e/mc_v12-4GeV-1e-inclusive_run10014_t1636673703.root",
    # os.environ["LDMX_BASE"]+"/data_ldmx/single_e/mc_v12-4GeV-1e-inclusive_run10014_t1636673701.root",
    # os.environ["LDMX_BASE"]+"/data_ldmx/single_e/mc_v12-4GeV-1e-inclusive_run10001_t1636674259.root",
    # os.environ["LDMX_BASE"]+"/data_ldmx/single_e/mc_v12-4GeV-1e-inclusive_run10001_t1636673835.root",
    ]
print(p.inputFiles)


p.outputFiles = [os.environ["LDMX_BASE"] + "/data_ldmx/validation_plots.root"]

p.termLogLevel=0
p.maxEvents = 1000
