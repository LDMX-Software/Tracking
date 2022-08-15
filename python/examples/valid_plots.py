import os,math
from LDMX.Framework import ldmxcfg
from LDMX.Tracking import tracking_validate



p = ldmxcfg.Process("TrackerReco")
p.libraries.append("libTracking.so")
p.detector = os.environ["LDMX_BASE"] + '/ldmx-sw/Detectors/data/ldmx-det-v12-dd4hep/detector.xml'


bugger = tracking_validate.validation_processor()
bugger.dumpobj = 0

#change to True for debugging
bugger.debug = False 


p.sequence = [bugger]
print(p.sequence)


p.inputFiles = [os.environ["LDMX_BASE"]+"/ldmx-sw/single_ele_tagger.root"]
print(p.inputFiles)



p.outputFiles = ["/Users/emryspeets/sw/data_ldmx/validation_plots.root"]

p.termLogLevel=0
p.maxEvents = 50000
