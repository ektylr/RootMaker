from CRABClient.UserUtilities import config
config = config()

# taskname
config.General.requestName = 'newrootmakertest_try1'
#config.General.workArea = ''

config.JobType.pluginName = 'Analysis'

config.JobType.psetName = 'RootTree.py'

# input and JSON
config.Data.inputDataset = '/SingleMuon/Run2015C_25ns-16Dec2015-v1/MINIAOD'
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Reprocessing/Cert_13TeV_16Dec2015ReReco_Collisions15_25ns_JSON_v2.txt'

# job splitting options
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 3

# storage options
config.Data.outLFNDirBase = '/store/user/ekennedy/smh2mu/data/test'
config.Site.storageSite = 'T2_CH_CERN'

config.Data.publication = False
