#include "CaloCell/CaloCellContainer.h"
#include "CaloCluster/CaloClusterContainer.h"
#include "CaloRings/CaloRingsContainer.h"
#include "EventInfo/EventInfoContainer.h"
#include "TruthParticle/TruthParticleContainer.h"
#include "TTree.h"
#include "CaloNtupleMaker.h"
#include "TMath.h"

using namespace SG;
using namespace CaloSampling;
using namespace Gaugi;





CaloNtupleMaker::CaloNtupleMaker( std::string name ) : 
  IMsgService(name),
  Algorithm()
{
  declareProperty( "EventKey"       , m_eventKey="EventInfo"            );
  declareProperty( "ClusterKey"     , m_clusterKey="Clusters"           );
  declareProperty( "RingerKey"      , m_ringerKey="Rings"               );
  declareProperty( "OutputLevel"    , m_outputLevel=1                   );
  declareProperty( "DeltaR"         , m_deltaR=0.15                     );
  declareProperty( "DumpCells"      , m_dumpCells=false                 );
  declareProperty( "NtupleName"     , m_ntupleName="physics"            );
}



CaloNtupleMaker::~CaloNtupleMaker()
{;}


StatusCode CaloNtupleMaker::initialize()
{
  setMsgLevel(m_outputLevel);
  return StatusCode::SUCCESS;
}


StatusCode CaloNtupleMaker::bookHistograms( SG::EventContext &ctx ) const
{
  auto store = ctx.getStoreGateSvc();

  // Create all local variables since this must be a const method
  int eventNumber         = -1;
  float avgmu             = -1;
  float seed_eta          = -1;
  float seed_phi          = -1;
  float seed_et           = -1;
  
  bool  cl_match          = false;
  float cl_eta            = -1;
  float cl_phi            = -1;
  float cl_et             = -1;
  float cl_e1             = -1;
  float cl_e2             = -1;
  float cl_e3             = -1;
  float cl_ehad1          = -1;
  float cl_ehad2          = -1;
  float cl_ehad3          = -1;
  float cl_etot           = -1;
  float cl_reta           = -1;
  float cl_rphi           = -1;
  float cl_rhad           = -1;
  float cl_eratio         = -1;
  float cl_f0             = -1;
  float cl_f1             = -1;
  float cl_f2             = -1;
  float cl_f3             = -1;
  float cl_weta2          = -1;
  float cl_e233           = -1;
  float cl_e237           = -1;
  float cl_e277           = -1;
  float cl_emaxs1         = -1;
  float cl_e2tsts1        = -1;
  bool  cl_ringer_match   =false;

  std::vector<float> cl_rings            ;
  std::vector<float> cl_cell_et          ;
  std::vector<float> cl_cell_eta         ;
  std::vector<float> cl_cell_phi         ;
  std::vector<float> cl_cell_deta        ;
  std::vector<float> cl_cell_dphi        ;
  std::vector<float> cl_cell_energy      ;
  std::vector<int>   cl_cell_layer    ;


  store->cd();
  TTree *tree = new TTree(m_ntupleName.c_str(), "");
  
  tree->Branch(  "EventNumber"        , &eventNumber        );
  tree->Branch(  "avgmu"              , &avgmu              );
  tree->Branch(  "seed_eta"           , &seed_eta           );
  tree->Branch(  "seed_phi"           , &seed_phi           );
  tree->Branch(  "seed_et"            , &seed_et            );

  tree->Branch(  "cl_match"           , &cl_match           );
  tree->Branch(  "cl_eta"             , &cl_eta             );
  tree->Branch(  "cl_phi"             , &cl_phi             );
  tree->Branch(  "cl_et"              , &cl_et              );
  tree->Branch(  "cl_e1"              , &cl_e1              );
  tree->Branch(  "cl_e2"              , &cl_e2              );
  tree->Branch(  "cl_e3"              , &cl_e3              );
  tree->Branch(  "cl_ehad1"           , &cl_ehad1           );
  tree->Branch(  "cl_ehad2"           , &cl_ehad2           );
  tree->Branch(  "cl_ehad3"           , &cl_ehad3           );
  tree->Branch(  "cl_etot"            , &cl_etot            );
  tree->Branch(  "cl_reta"            , &cl_reta            );
  tree->Branch(  "cl_rphi"            , &cl_rphi            );
  tree->Branch(  "cl_rhad"            , &cl_rhad            );
  tree->Branch(  "cl_eratio"          , &cl_eratio          );
  tree->Branch(  "cl_f0"              , &cl_f0              );
  tree->Branch(  "cl_f1"              , &cl_f1              );
  tree->Branch(  "cl_f2"              , &cl_f2              );
  tree->Branch(  "cl_f3"              , &cl_f3              );
  tree->Branch(  "cl_weta2"           , &cl_weta2           );
  tree->Branch(  "cl_e233"            , &cl_e233            );
  tree->Branch(  "cl_e237"            , &cl_e237            );
  tree->Branch(  "cl_e277"            , &cl_e277            );
  tree->Branch(  "cl_emaxs1"          , &cl_emaxs1          );
  tree->Branch(  "cl_e2tsts1"         , &cl_e2tsts1         );
  tree->Branch(  "cl_ringer_match"    , &cl_ringer_match    );
  tree->Branch(  "cl_rings"           , &cl_rings           );
  tree->Branch(  "cl_cell_et"         , &cl_cell_et         );
  tree->Branch(  "cl_cell_eta"        , &cl_cell_eta        );
  tree->Branch(  "cl_cell_phi"        , &cl_cell_phi        );
  tree->Branch(  "cl_cell_deta"       , &cl_cell_deta       );
  tree->Branch(  "cl_cell_dphi"       , &cl_cell_dphi       );
  tree->Branch(  "cl_cell_energy"     , &cl_cell_energy     );
  tree->Branch(  "cl_cell_layer"   , &cl_cell_layer   );
 

  store->add( tree );
  
  return StatusCode::SUCCESS;
}


StatusCode CaloNtupleMaker::pre_execute( EventContext &/*ctx*/ ) const
{
  return StatusCode::SUCCESS;
}


StatusCode CaloNtupleMaker::execute( EventContext &/*ctx*/, const G4Step * /*step*/ ) const
{
  return StatusCode::SUCCESS;
}


StatusCode CaloNtupleMaker::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode CaloNtupleMaker::post_execute( EventContext &/*ctx*/ ) const
{
  return StatusCode::SUCCESS;
}


StatusCode CaloNtupleMaker::fillHistograms( EventContext &ctx ) const
{
  auto store = ctx.getStoreGateSvc();
  // Event info
  SG::ReadHandle<xAOD::EventInfoContainer> event(m_eventKey, ctx);

  if( !event.isValid() ){
    MSG_FATAL( "It's not possible to read the xAOD::EventInfoContainer from this Context" );
  }

  store->cd();
  TTree *tree = store->tree(m_ntupleName);
 
  for ( auto& seed : (**event.ptr()).front()->allSeeds() ){
    int eventNumber = (**event.ptr()).front()->eventNumber();
    float avgmu = (**event.ptr()).front()->avgmu();
    MSG_DEBUG( "Fill this seed into the collection tree." );
    Fill( ctx, tree, seed, eventNumber, avgmu );
  }
  
  return StatusCode::SUCCESS;
}


template <class T>
void CaloNtupleMaker::InitBranch(TTree* fChain, std::string branch_name, T* param) const
{
  std::string bname = branch_name;
  if (fChain->GetAlias(bname.c_str()))
     bname = std::string(fChain->GetAlias(bname.c_str()));

  if (!fChain->FindBranch(bname.c_str()) ) {
    MSG_WARNING( "unknown branch " << bname );
    return;
  }
  fChain->SetBranchStatus(bname.c_str(), 1.);
  fChain->SetBranchAddress(bname.c_str(), param);
}


void CaloNtupleMaker::Fill( EventContext &ctx , TTree *tree, xAOD::seed_t seed, int evt, float mu ) const
{
  // Create all local variables since this must be a const method
  int   eventNumber    ;
  float avgmu          ;
  float seed_eta       ;
  float seed_phi       ;
  float seed_et        ;
  bool  cl_match       ;
  float cl_eta         ;
  float cl_phi         ;
  float cl_et          ;
  float cl_e1          ;
  float cl_e2          ;
  float cl_e3          ;
  float cl_ehad1       ;
  float cl_ehad2       ;
  float cl_ehad3       ;
  float cl_etot        ;
  float cl_reta        ;
  float cl_rphi        ;
  float cl_rhad        ;
  float cl_eratio      ;
  float cl_f0          ;
  float cl_f1          ;
  float cl_f2          ;
  float cl_f3          ;
  float cl_weta2       ;
  float cl_e233        ;
  float cl_e237        ;
  float cl_e277        ;
  float cl_emaxs1      ;
  float cl_e2tsts1     ;
  bool  cl_ringer_match;

  std::vector<float> *cl_rings         = nullptr;
  std::vector<float> *cl_cell_et       = nullptr;
  std::vector<float> *cl_cell_eta      = nullptr;
  std::vector<float> *cl_cell_phi      = nullptr;
  std::vector<float> *cl_cell_deta     = nullptr;
  std::vector<float> *cl_cell_dphi     = nullptr;
  std::vector<float> *cl_cell_energy   = nullptr;
  std::vector<int>   *cl_cell_layer    = nullptr;
  

  InitBranch( tree,  "EventNumber"            , &eventNumber            );
  InitBranch( tree,  "avgmu"                  , &avgmu                  );
  InitBranch( tree,  "seed_eta"               , &seed_eta               );
  InitBranch( tree,  "seed_phi"               , &seed_phi               );
  InitBranch( tree,  "seed_et"                , &seed_et                );
  InitBranch( tree,  "cl_match"               , &cl_match               );
  InitBranch( tree,  "cl_eta"                 , &cl_eta                 );
  InitBranch( tree,  "cl_phi"                 , &cl_phi                 );
  InitBranch( tree,  "cl_et"                  , &cl_et                  );
  InitBranch( tree,  "cl_e1"                  , &cl_e1                  );
  InitBranch( tree,  "cl_e2"                  , &cl_e2                  );
  InitBranch( tree,  "cl_e3"                  , &cl_e3                  );
  InitBranch( tree,  "cl_ehad1"               , &cl_ehad1               );
  InitBranch( tree,  "cl_ehad2"               , &cl_ehad2               );
  InitBranch( tree,  "cl_ehad3"               , &cl_ehad3               );
  InitBranch( tree,  "cl_etot"                , &cl_etot                );
  InitBranch( tree,  "cl_reta"                , &cl_reta                );
  InitBranch( tree,  "cl_rphi"                , &cl_rphi                );
  InitBranch( tree,  "cl_rhad"                , &cl_rhad                );
  InitBranch( tree,  "cl_eratio"              , &cl_eratio              );
  InitBranch( tree,  "cl_f0"                  , &cl_f0                  );
  InitBranch( tree,  "cl_f1"                  , &cl_f1                  );
  InitBranch( tree,  "cl_f2"                  , &cl_f2                  );
  InitBranch( tree,  "cl_f3"                  , &cl_f3                  );
  InitBranch( tree,  "cl_weta2"               , &cl_weta2               );
  InitBranch( tree,  "cl_e233"                , &cl_e233                );
  InitBranch( tree,  "cl_e237"                , &cl_e237                );
  InitBranch( tree,  "cl_e277"                , &cl_e277                );
  InitBranch( tree,  "cl_emaxs1"              , &cl_emaxs1              );
  InitBranch( tree,  "cl_e2tsts1"             , &cl_e2tsts1             );
  InitBranch( tree,  "cl_ringer_match"        , &cl_ringer_match        );
  InitBranch( tree,  "cl_rings"               , &cl_rings               );
  InitBranch( tree,  "cl_cell_et"             , &cl_cell_et             );
  InitBranch( tree,  "cl_cell_eta"            , &cl_cell_eta            );
  InitBranch( tree,  "cl_cell_phi"            , &cl_cell_phi            );
  InitBranch( tree,  "cl_cell_deta"           , &cl_cell_deta           );
  InitBranch( tree,  "cl_cell_dphi"           , &cl_cell_dphi           );
  InitBranch( tree,  "cl_cell_energy"         , &cl_cell_energy         );
  InitBranch( tree,  "cl_cell_layer"       , &cl_cell_layer             );

  
  MSG_DEBUG( "Link all branches..." );

  // Create all local variables since this must be a const method
  eventNumber         = -1;
  avgmu               = 0;
  seed_eta            = 0;
  seed_phi            = 0;
  seed_et             = 0;
  cl_match            = false;
  cl_eta              = 0;
  cl_phi              = 0;
  cl_et               = 0;
  cl_e1               = 0;
  cl_e2               = 0;
  cl_e3               = 0;
  cl_ehad1            = 0;
  cl_ehad2            = 0;
  cl_ehad3            = 0;
  cl_etot             = 0;
  cl_reta             = 0;
  cl_rphi             = 0;
  cl_rhad             = 0;
  cl_eratio           = 0;
  cl_f0               = 0;
  cl_f1               = 0;
  cl_f2               = 0;
  cl_f3               = 0;
  cl_weta2            = 0;
  cl_e233             = 0;
  cl_e237             = 0;
  cl_e277             = 0;
  cl_emaxs1           = 0;
  cl_e2tsts1          = 0;
  cl_ringer_match     =false;

  cl_rings->clear()        ;

  cl_cell_et->clear()         ;
  cl_cell_eta->clear()        ;
  cl_cell_phi->clear()        ;
  cl_cell_deta->clear()       ;
  cl_cell_dphi->clear()       ;
  cl_cell_energy->clear()     ;
  cl_cell_layer->clear()   ;

  eventNumber = evt;
  avgmu       = mu;
  seed_eta    = seed.eta;
  seed_phi    = seed.phi;
  seed_et     = seed.et * 1.e3; // in MeV


  { // Fill all reco values
    const xAOD::CaloCluster *clus=nullptr;
    if( match( ctx, m_clusterKey, seed, clus ) )
    {
      MSG_DEBUG( "Dump reco cluster..." );
      cl_match   =  true;
      cl_eta     =  clus->eta()    ;
      cl_phi     =  clus->phi()    ;
      cl_et      =  clus->et()     ;
      cl_e1      =  clus->e1()     ;
      cl_e2      =  clus->e2()     ;
      cl_e3      =  clus->e3()     ;
      cl_ehad1   =  clus->ehad1()  ;
      cl_ehad2   =  clus->ehad2()  ;
      cl_ehad3   =  clus->ehad3()  ;
      cl_etot    =  clus->etot()   ;
      cl_reta    =  clus->reta()   ;
      cl_rphi    =  clus->rphi()   ;
      cl_rhad    =  clus->rhad()   ;
      cl_eratio  =  clus->eratio() ;
      cl_f0      =  clus->f0()     ;
      cl_f1      =  clus->f1()     ;
      cl_f2      =  clus->f2()     ;
      cl_f3      =  clus->f3()     ;
      cl_weta2   =  clus->weta2()  ;
      cl_e233    =  clus->e233()   ;
      cl_e237    =  clus->e237()   ;
      cl_e277    =  clus->e277()   ;
      cl_emaxs1  =  clus->emaxs1() ;
      cl_e2tsts1 =  clus->e2tsts1();
      
      //fillTruthCells(clus);
      const xAOD::CaloRings *ringer=nullptr;
      if( match( ctx, m_ringerKey, clus, ringer ) )
      {
        cl_ringer_match = true;
        auto vec = ringer->rings();
        cl_rings->assign( vec.begin(), vec.end() );
      }

      if (m_dumpCells){
        for (auto &cell : clus->allCells() ){
          cl_cell_et->push_back( cell->et() );
          cl_cell_eta->push_back( cell->eta() );
          cl_cell_phi->push_back( cell->phi() );
          cl_cell_deta->push_back( cell->deltaEta() );
          cl_cell_dphi->push_back( cell->deltaPhi() );
          cl_cell_energy->push_back( cell->energy() );
          cl_cell_layer->push_back( (int)cell->layer() );
        }
      }


    }
  }

  tree->Fill();
}


float CaloNtupleMaker::dR( float eta1, float phi1, float eta2, float phi2 ) const
{
  float deta = fabs(eta1-eta2);
  float dphi = fabs(phi1-phi2)  < TMath::Pi() ? fabs(phi1-phi2) : 2*TMath::Pi() - fabs(phi1-phi2) ;
  return sqrt( deta*deta + dphi*dphi);
}


bool CaloNtupleMaker::match( EventContext &ctx , std::string key, xAOD::seed_t seed, const xAOD::CaloCluster *&cluster )  const
{
  SG::ReadHandle<xAOD::CaloClusterContainer> container( key, ctx );
  
  if( !container.isValid() )
  {
    MSG_WARNING("It's not possible to read the xAOD::CaloClusterContainer from this Context using this key " << key );
    return false;
  }

  for (const auto clus : **container.ptr() ){
    if( dR( seed.eta, seed.phi, clus->eta(), clus->phi() ) < m_deltaR ){
      cluster=clus;
      return true;
    }
  }
  return false;
}


bool CaloNtupleMaker::match( EventContext &ctx , std::string key, const xAOD::CaloCluster *cluster, const xAOD::CaloRings *&ringer ) const
{
  SG::ReadHandle<xAOD::CaloRingsContainer> container( key, ctx );
  
  if( !container.isValid() )
  {
    MSG_WARNING("It's not possible to read the xAOD::CaloRingsContainer from this Context using this key " << key );
    return false;
  }

  for (const auto rings : **container.ptr() ){ 
    if ( rings->caloCluster() == cluster ){
      ringer= rings;
      return true;
    }
  }
  return false;
}
