
from LDMX.Framework.ldmxcfg import Producer

class TrackerDigiProducer(Producer) : 

  def __init__(self, instance_name = 'TrackerDigi'): 
    super().__init__(instance_name, 'tracking::sim::TrackerDigiProducer', 'Tracking')
