from LDMX.Framework.ldmxcfg import Producer


class DigitizationProcessor(Producer):
    """ Producer that smears simulated tracker hits.

    Parameters
    ----------
    instance_name : str
        Unique name for this instance.


    Attributes
    ----------
    mergeHits : bool
        Activate merging of all hits that have the same trackID on the same layer.
    do_smearing : bool
        Activate the smearing.
    sigma_u : float
        Smearing sigma in the sensitive direction
    sigma_v : float
        Smearing sigma in the un-sensitive direction
    trackID : int
        If trackID>0, retain only hits with that particular trackID and discard the rest.
    minEdep : float
        Minimum energy deposited by G4 to consider the hit
    debug : bool
        Activate debug print-outs
    hit_collection : string
        Input hit collection to be smeared
    out_collection : string
        Output hit collection to be stored
    """

    def __init__(self, instance_name="DigitizationProcessor"):
        super().__init__(instance_name, 'tracking::reco::DigitizationProcessor', 'Tracking')
        self.mergeHits = True
        self.do_smearing = True
        self.sigma_u = 0.06
        self.sigma_v = 0.0
        self.trackID = -1
        self.debug = False
        self.minEdep = 0.05


class SeedFinderProcessor(Producer):
    """ Producer to find Seeds for the KF-based track finding.

    Parameters
    ----------
    instance_name : str
        Unique name for this instance.
    """

    def __init__(self, instance_name="SeedFinderProcessor"):
        super().__init__(instance_name, 'tracking::sim::SeedFinderProcessor', 'Tracking')


class CKFProcessor(Producer):
    """ Producer that runs the Combinatorial Kalman Filter for track finding and fitting.

    Parameters
    ----------
    instance_name : str
        Unique name for this instance.
    """
  def __init__(self, instance_name = 'CKFProcessor'): 
    super().__init__(instance_name, 'tracking::sim::CKFProcessor', 'Tracking')

class TruthSeedProcessor(Producer) :
    """ Producer that returns truth seeds to feed the KF based track finding.
    Seeds are not smeared, so the fits will be too optimistic, especially the residuals of the
    estimated locations w.r.t. simulated hits on each surface.

    Parameters
    ----------
    instance_name : str
        Unique name for this instance.
    """
    def __init__(self, instance_name = "TruthSeedProcessor"):
        super().__init__(instance_name, 'tracking::reco::TruthSeedProcessor','Tracking')




