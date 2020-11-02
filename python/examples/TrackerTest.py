from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process("tracking")

p.libraries.append("libTracking.so")

p.detector = '/nfs/slac/g/ldmx/users/pbutti/ldmx-sw/Detectors/data/ldmx-det-v12-dd4hep/detector.xml'

from LDMX.Tracking import tracker_digi

digi = tracker_digi.TrackerDigiProducer()

p.sequence = [digi]

print(p.sequence)

#p.inputFiles = ['4gev_1e_ecal_pn_v12_ldmx-det-v12_run50384_seeds_100769_100770.root']
p.inputFiles = ['/nfs/slac/g/ldmx/data/mc/v12/4gev_1e_inclusive/v2.3.0-alpha/mc_v12-4GeV-1e-inclusive_run1310001_t1601628859_reco.root']
p.outputFiles = ['tracker_test.root']

p.termLogLevel=0

p.maxEvents = 1
