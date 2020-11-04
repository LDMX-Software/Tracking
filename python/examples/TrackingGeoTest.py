from LDMX.Framework import ldmxcfg

p = ldmxcfg.Process("tracking")

p.libraries.append("libTracking.so")

p.detector = '/nfs/slac/g/ldmx/users/pbutti/ldmx-sw/Detectors/data/ldmx-det-v12-dd4hep/detector.xml'

from LDMX.Tracking import tracking_geo

geo  = tracking_geo.TrackingGeometryMaker()

p.sequence = [geo]

print(p.sequence)

#You need to copy this file to the LDMX_BASE area for the example to work!
#/nfs/slac/g/ldmx/data/mc20/v12/4.0GeV/v2.2.1-batch8/mc_v12-4GeV-1e-ecal_photonuclear_run1075000_t1600764279.root

p.inputFiles = ['mc_v12-4GeV-1e-ecal_photonuclear_run1075000_t1600764279.root']
p.outputFiles = ['tracker_test.root']

p.termLogLevel=0

p.maxEvents = 1
