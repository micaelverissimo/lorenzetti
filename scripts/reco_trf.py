#!/usr/bin/env python3

from Gaugi.messenger      import LoggingLevel, Logger
from Gaugi                import GeV
from P8Kernel             import EventReader
from G4Kernel             import *

from CaloClusterBuilder   import CaloClusterMaker
from CaloRingerBuilder    import CaloRingerMaker
from TruthParticleBuilder import TruthParticleMaker
from CaloCell.CaloDefs    import CaloSampling

import numpy as np
import argparse
import sys,os


mainLogger = Logger.getModuleLogger("job")
parser = argparse.ArgumentParser(description = '', add_help = False)
parser = argparse.ArgumentParser()


parser.add_argument('-i','--inputFile', action='store', dest='inputFile', required = False,
                    help = "The event input file generated by the Pythia event generator.")

parser.add_argument('-o','--outputFile', action='store', dest='outputFile', required = False,
                    help = "The reconstructed event file generated by lzt/geant4 framework.")

parser.add_argument('-d', '--debug', action='store_true', dest='debug', required = False,
                    help = "In debug mode.")

parser.add_argument('-nt','--numberOfThreads', action='store', dest='numberOfThreads', required = False, type=int, default=1,
                    help = "The number of threads")

parser.add_argument('--evt','--numberOfEvents', action='store', dest='numberOfEvents', required = False, type=int, default=None,
                    help = "The number of events to apply the reconstruction.")

parser.add_argument('--visualization', action='store_true', dest='visualization', required = False,
                    help = "Run with Qt interface.")

parser.add_argument('-n', '--ntuple', action='store', dest='ntuple',required = False, default = 'physics',
                    help = "Choose the ntuple schemma: raw (energy estimation studies) or physics (physics studies)")

parser.add_argument('--enableMagneticField', action='store_true', dest='enableMagneticField',required = False, 
                    help = "Enable the magnetic field.")

parser.add_argument('--outputLevel', action='store', dest='outputLevel', required = False, type=int, default=3,
                    help = "The output level messenger.")



pi = np.pi

if len(sys.argv)==1:
  parser.print_help()
  sys.exit(1)

args = parser.parse_args()


# Get all output names
if not '.root' in args.outputFile:
  args.outputFile+='.root'

# Add index for each thread
outputFileList = []
for thread in range( args.numberOfThreads ):
  outputFileList.append( args.outputFile.replace( '.root', "_%d.root"%thread ) )




outputLevel = 0 if args.debug else args.outputLevel

try:

  from DetectorATLASModel import DetectorConstruction as ATLAS
  from DetectorATLASModel import CaloCellBuilder
  
  # Build the ATLAS detector
  detector = ATLAS("GenericATLASDetector", 
                   UseMagneticField = args.enableMagneticField, # Force to be false since the mag field it is not working yet
                   CutOnPhi = False
                   )

  acc = ComponentAccumulator("ComponentAccumulator", detector,
                              RunVis=args.visualization,
                              NumberOfThreads = args.numberOfThreads,
                              Seed = 512, # fixed seed since pythia will be used. The random must be in the pythia generation
                              OutputFile = args.outputFile)
  
  gun = EventReader( "PythiaGenerator",
                     EventKey   = recordable("EventInfo"),
                     FileName   = args.inputFile,
                     BunchDuration = 25.0,#ns
                     )

  particles = TruthParticleMaker( "TruthParticleMaker",
                                   HistogramPath = "Expert/Truth",
                                   OutputLevel = outputLevel)

  calorimeter = CaloCellBuilder("CaloCellBuilder",
                                HistogramPath = "Expert/Cells",
                                OutputLevel   = outputLevel,
                                )
  
  acc+= particles # truth
  gun.merge(acc)
  calorimeter.merge(acc)

  
  if args.ntuple == 'physics':

      # build cluster for all seeds
      cluster = CaloClusterMaker( "CaloClusterMaker",
                                  CellsKey        = recordable("Cells"),
                                  EventKey        = recordable("EventInfo"),
                                  ClusterKey      = recordable("Clusters"),
                                  TruthKey        = recordable("Particles"),
                                  EtaWindow       = 0.4,
                                  PhiWindow       = 0.4,
                                  MinCenterEnergy = 5*GeV, # 15GeV in the EM core 
                                  HistogramPath   = "Expert/Clusters",
                                  OutputLevel     = outputLevel)

      ringer = CaloRingerBuilder( "CaloRingerBuilder",
                                  RingerKey     = recordable("Rings"),
                                  ClusterKey    = recordable("Clusters"),
                                  DeltaEtaRings = [0.025,0.00325, 0.025, 0.050, 0.1, 0.1, 0.2 ],
                                  DeltaPhiRings = [pi/32, pi/32, pi/128, pi/128, pi/128, pi/32, pi/32, pi/32],
                                  NRings        = [8, 64, 8, 8, 4, 4, 4],
                                  LayerRings = [
                                    [CaloSampling.PSB, CaloSampling.PSE],
                                    [CaloSampling.EMB1, CaloSampling.EMEC1],
                                    [CaloSampling.EMB2, CaloSampling.EMEC2],
                                    [CaloSampling.EMB3, CaloSampling.EMEC3],
                                    [CaloSampling.HEC1, CaloSampling.TileCal1, CaloSampling.TileExt1],
                                    [CaloSampling.HEC2, CaloSampling.TileCal2, CaloSampling.TileExt2],
                                    [CaloSampling.HEC3, CaloSampling.TileCal3, CaloSampling.TileExt3],
                                  ],
                                  HistogramPath = "Expert/Rings",
                                  OutputLevel   = outputLevel)
 
  
      #from CaloNtupleBuilder import CaloNtupleMaker
      #ntuple = CaloNtupleMaker( "CaloNtupleMaker",
      #                          EventKey        = recordable("EventInfo"),
      #                          RingerKey       = recordable("Rings"),
      #                          ClusterKey      = recordable("Clusters"),
      #                          DeltaR          = 0.15,
      #                          DumpCells       = True,
      #                          OutputLevel     = outputLevel)
      
      acc+= cluster
      acc+= ringer
      #acc+= ntuple
  
  
  elif args.ntuple == 'raw':
  
      from CaloNtupleBuilder import RawNtupleMaker
      ntuple = RawNtupleMaker (  "RawNtupleMaker",
                                 EventKey        = recordable("EventInfo"),
                                 CellsKey        = recordable("Cells"),
                                 EtaWindow       = 0.4,
                                 PhiWindow       = 0.4,
                                 OutputLevel     = outputLevel)
  
      acc += ntuple
  else:
      mainLogger.debug('Invalid ntuple tuple. Choose between raw or physics.')
  
  
  acc.run(args.numberOfEvents)
  
  
  command = "hadd -f " + args.outputFile + ' '
  for fname in outputFileList:
    command+=fname + ' '
  print( command )
  os.system(command)
  
  # remove thread files
  for fname in outputFileList:
    os.system( 'rm '+ fname )
  
  
  if args.visualization:
      input("Press Enter to quit...")

  sys.exit(0)
  
except  Exception as e:
  print(e)
  sys.exit(1)
