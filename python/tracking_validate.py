import os
#from LDMX.Framework import ldmxcfg
from LDMX.Framework.ldmxcfg import Producer

#This class produces validation plots from a track collection.
class validation_processor(Producer) :

    def __init__(self, instance_name = "validation_processor"):
        super().__init__(instance_name, 'tracking::reco::validation_processor','Tracking')
