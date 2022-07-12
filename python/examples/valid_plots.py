import os,math
from LDMX.Framework import ldmxcfg


sw_path = '/Users/benjaminlawrence-sanderson/workdir/projects/LDMX/sw/'

p = ldmxcfg.Process("TrackerReco")
p.libraries.append("libTracking.so")
p.detector = '/Users/emryspeets/sw/ldmx-sw/Detectors/data/ldmx-det-v12-dd4hep/detector.xml'


from LDMX.Tracking import tracking_validate



bugger = tracking_validate.validation_processor()
bugger.dumpobj = 0

#change to True for debugging
bugger.debug = False





p.sequence = [bugger]
print(p.sequence)


p.inputFiles = [os.environ["LDMX_BASE"]+"/data_ldmx/single_e/single_ele_tagger.root"]
print(p.inputFiles)


p.outputFiles = [os.path.join(sw_path, "data_ldmx/validation_plots.root")]

p.termLogLevel=0
p.maxEvents = 1000
