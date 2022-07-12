import os
from LDMX.Framework import ldmxcfg
from LDMX.Framework.ldmxcfg import Producer


#was from LDMX.Framework.ldmxcfg 

#This class produces validation plots from a track collection.
class validation_processor(Producer) :

    def __init__(self, instance_name = "validation_processor"):
        super().__init__(instance_name, 'tracking::reco::validation_processor','Tracking')


#This class is to produce vertices between two track collections, i.e. for tagger/recoil matching for example.

#class Vertexer(Producer) :
#    def __init__(self, instance_name = "Vertexer"):
#        super().__init__(instance_name,'tracking::reco::Vertexer','Tracking')
