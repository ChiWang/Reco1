/*
 *  $Id: DmpAlgRec1_Hits.cc, 2015-03-02 20:59:37 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

//#include <stdio.h>
#include <stdlib.h>     // getenv()
//#include <boost/lexical_cast.hpp>
#include "TMath.h"

#include "DmpEvtBgoRaw.h"
#include "DmpEvtNudRaw.h"
#include "DmpDataBuffer.h"
#include "DmpBgoBase.h"
#include "DmpPsdBase.h"
#include "DmpCore.h"
#include "DmpAlgRec1_Hits.h"
#define VerifiedSig  30

//-------------------------------------------------------------------
DmpAlgRec1_Hits::DmpAlgRec1_Hits()
 :DmpVAlg("DmpAlgRec1_Hits"),
  fEvtBgo(0),
  fEvtPsd(0),
  fEvtNud(0),
  fOutBgo(0),
  fOutPsd(0),
  _fCutHigADC(13000)
{
  gRootIOSvc->SetOutFileKey("Rec1");
  std::string root_path = (std::string)getenv("DMPSWSYS")+"/share/Calibration";
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal",root_path+"/Bgo.ped");
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal",root_path+"/Psd.ped");
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoRelation",root_path+"/Bgo.rel");
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdRelation",root_path+"/Psd.rel");
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoMip",root_path+"/Bgo.mip");
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdMip",root_path+"/Psd.mip");
}

//-------------------------------------------------------------------
DmpAlgRec1_Hits::~DmpAlgRec1_Hits(){
}

void DmpAlgRec1_Hits::SetRelationFile(std::string ID,std::string f)
{
        TString xx = f;
        if(not xx.Contains(".rel")){
                DmpLogWarning<<f<<"("<<ID<<") is not a relation file... will use the default one"<<DmpLogEndl;
                return;
        }
  TString Id = ID;
  Id.ToUpper();
  if(Id == "BGO"){
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoRelation",f);
  }else if(Id == "PSD"){
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdRelation",f);
  }
}

void DmpAlgRec1_Hits::SetPedestalFile(std::string ID,std::string f)
{
        TString xx = f;
        if(not xx.Contains(".ped")){
                DmpLogWarning<<f<<"("<<ID<<") is not a pedestal file... will use the defaur one"<<DmpLogEndl;
                return;
        }
  TString Id = ID;
  Id.ToUpper();
  if(Id == "BGO"){
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal",f);
  }else if(Id == "PSD"){
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal",f);
  }
}

void DmpAlgRec1_Hits::SetMipsFile(std::string ID,std::string f)
{
        TString xx = f;
        if(not xx.Contains(".mip")){
                DmpLogWarning<<f<<"("<<ID<<") is not a mips file... will use the default one"<<DmpLogEndl;
                return;
        }
  TString Id = ID;
  Id.ToUpper();
  if(Id == "BGO"){
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoMip",f);
  }else if(Id == "PSD"){
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdMip",f);
  }
}

//-------------------------------------------------------------------
bool DmpAlgRec1_Hits::Initialize()
{
  // read input data
  std::string path_i = "Event/Rec0";
  fEvtBgo = dynamic_cast<DmpEvtBgoRaw*>(gDataBuffer->ReadObject(path_i+"/Bgo"));
  if(0 == fEvtBgo){
    fEvtBgo = new DmpEvtBgoRaw();
    gDataBuffer->LinkRootFile(path_i+"/Bgo",fEvtBgo);
  }
  fEvtPsd = dynamic_cast<DmpEvtPsdRaw*>(gDataBuffer->ReadObject(path_i+"/Psd"));
  if(0 == fEvtPsd){
    fEvtPsd = new DmpEvtPsdRaw();
    gDataBuffer->LinkRootFile(path_i+"/Psd",fEvtPsd);
  }
  fEvtNud = dynamic_cast<DmpEvtNudRaw*>(gDataBuffer->ReadObject(path_i+"/Nud"));
  if(0 == fEvtNud){
    fEvtNud = new DmpEvtNudRaw();
    gDataBuffer->LinkRootFile(path_i+"/Nud",fEvtNud);
  }
  std::string path_o = "Event/Rec1";
  fOutBgo = new DmpEvtBgoHits2();
  gDataBuffer->RegisterObject(path_o+"/Bgo",fOutBgo);
  fOutPsd = new DmpEvtPsdHits();
  gDataBuffer->RegisterObject(path_o+"/Psd",fOutPsd);
  gDataBuffer->RegisterObject(path_o+"/Nud",fEvtNud);

  return _loadParametes();
}

bool DmpAlgRec1_Hits::_loadParametes()
{
    // bgo relation
  DmpParameterSteering steering;
  std::string inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/BgoRelation");
  bool loadSta = DAMPE::LoadParameters(inputFile,fBgoRel,steering);
  if(not loadSta){
    return false;
  }
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoRelation/t0",steering["StartTime"]);
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoRelation/t1",steering["StopTime"]);
    // psd relation
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/PsdRelation");
  steering.clear();
  loadSta = DAMPE::LoadParameters(inputFile,fPsdRel,steering);
  if(not loadSta){
          return false;
  }
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdRelation/t0",steering["StartTime"]);
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdRelation/t1",steering["StopTime"]);
    // bgo Mips
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/BgoMip");
  steering.clear();
  loadSta = DAMPE::LoadParameters(inputFile,fBgoMip,steering);
  if(not loadSta){
          return false;
  }
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoMip/t0",steering["StartTime"]);
  gCore->GetJobOption()->SetOption(this->Name()+"/BgoMip/t1",steering["StopTime"]);
    // psd Mips
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/PsdMip");
  steering.clear();
  loadSta = DAMPE::LoadParameters(inputFile,fPsdMip,steering);
  if(not loadSta){
          return false;
  }
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdMip/t0",steering["StartTime"]);
  gCore->GetJobOption()->SetOption(this->Name()+"/PsdMip/t1",steering["StopTime"]);
    // bgo ped
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/BgoPedestal");
  loadSta = DAMPE::LoadParameters(inputFile,fBgoPed,steering);
  if(not loadSta){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/BgoPedestal/t1",steering["StopTime"]);
  }
    // psd ped
  inputFile = gCore->GetJobOption()->GetValue(this->Name()+"/PsdPedestal");
  loadSta = DAMPE::LoadParameters(inputFile,fPsdPed,steering);
  if(not loadSta){
          return false;
  }else{
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal/t0",steering["StartTime"]);
    gCore->GetJobOption()->SetOption(this->Name()+"/PsdPedestal/t1",steering["StopTime"]);
  }

  for(short l=0;l<DmpParameterBgo::kPlaneNo*2;++l){
    for(short b=0;b<DmpParameterBgo::kBarNo;++b){
      for(short s=0;s<2;++s){
        for(short d=0;d<2;++d){  // only for dy2,5
          short gid_s = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,d*3+2);
          short gid_b = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,(d+1)*3+2);
          //std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\tl="<<l<<"\tb ="<<b<<"\ts="<<s<<"\td="<<d<<"\t\tped size ="<<fBgoPed[gid_s].size()<<"\t\t"<<fBgoPed[gid_b].size()<<"\t\t"<<fBgoRel[gid_s].size()<<std::endl;
          fTotSigmaBgo[gid_s] = TMath::Sqrt(TMath::Power(fBgoPed[gid_s].at(5),2) + TMath::Power(fBgoPed[gid_b].at(5)*fBgoRel[gid_s].at(5),2));
          //fTotSigmaBgo[gid_s] = TMath::Sqrt(TMath::Power(fBgoPed[gid_s].at(5),2) + TMath::Power(fBgoPed[gid_b].at(5)*fBgoRel[gid_s].at(5),2) + TMath::Power(fBgoRel[gid_s].at(4)/2,2));
        }
      }
    }
  }
  for(short l=0;l<DmpParameterPsd::kPlaneNo*2;++l){
    for(short b=0;b<DmpParameterPsd::kStripNo;++b){
      for(short s=0;s<2;++s){
        short gid_s = DmpPsdBase::ConstructGlobalDynodeID(l,b,s,5);
        short gid_b = DmpPsdBase::ConstructGlobalDynodeID(l,b,s,8);
          //std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\tgid = "<<gid_s<<"\tl="<<l<<"\tb ="<<b<<"\ts="<<s<<"\t\tped size ="<<fPsdPed[gid_s].size()<<"\t\t"<<fPsdPed[gid_b].size()<<"\t\t"<<fPsdRel[gid_s].size()<<std::endl;
        fTotSigmaPsd[gid_s] = TMath::Sqrt(TMath::Power(fPsdPed[gid_s].at(5),2) + TMath::Power(fPsdPed[gid_b].at(5)*fPsdRel[gid_s].at(5),2));
      }
    }
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgRec1_Hits::ProcessThisEvent()
{
  fOutBgo->Reset();
  // Bgo
  for(short l=0;l<DmpParameterBgo::kPlaneNo*2;++l){
    for(short b=0;b<DmpParameterBgo::kBarNo;++b){
      short gid[2][3]; //  side0,1;  2,5,8;
      double sign[2][3];// side0,1;  2,5,8;
      double adc8[2]={0,0};
      short usedDy[2]={0,0};
      // find both adc dy8
      for(short s=0;s<DmpParameterBgo::kSideNo;++s){
        for(short dy=0;dy<3;++dy){ // load signal
          gid[s][dy] = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,3*dy+2);
          if(fEvtBgo->fADC.find(gid[s][dy]) != fEvtBgo->fADC.end()){
            sign[s][dy] = fEvtBgo->fADC.at(gid[s][dy]);
            if(sign[s][dy] >_fCutHigADC){
              sign[s][dy] = 0;
              if(s == 1 && dy == 0){
                gCore->EventHeader()->SetTag(DmpEDetectorID::kBgo,DmpEvtHeader::tag_Overflow);
              }
            }
          }else{
            sign[s][dy] = 0;
          }
        }
        for(short dy=2;dy>0;--dy){ // use small check bigger
          if(sign[s][dy] < 10 * fBgoPed[gid[s][dy]].at(5)) continue;    // must bigger than 10 *sigma
          if(sign[s][dy-1] < 5 * fBgoPed[gid[s][dy-1]].at(5)) continue;    // must bigger than 5 *sigma
          double deltaADC_small = sign[s][dy]*fBgoRel[gid[s][dy-1]].at(5) + fBgoRel[gid[s][dy-1]].at(4)  - sign[s][dy-1];
          bool comp = TMath::Abs(deltaADC_small) > 5*fTotSigmaBgo.at(gid[s][dy-1]); // affected by last event
          if(comp){
            sign[s][dy] = 0;
            DmpLogWarning<<"Bgo pileup"<<__FILE__<<"("<<__LINE__<<")\tl="<<l<<"\tb="<<b<<"\ts="<<s<<"\td="<<dy<<"\t\tdelte = "<<deltaADC_small<<std::endl;
          }
        }
        for(short dy=2;dy>=0; --dy){
          if(sign[s][dy] != 0){
            if(dy == 2){
              adc8[s] = sign[s][2];
            }else{
              adc8[s] = _trans2Dy8_Bgo(l,b,s,dy*3+2,sign[s][dy]);
            }
            usedDy[s] = dy*3+2;
            break;
          }
        }
      }

      // get energy
      if(usedDy[0] == 0 && usedDy[1] == 0) continue;    // both side has no signals

      _dmpAHit *bar = new _dmpAHit(l,b);
      fOutBgo->fHits.push_back(bar);
      bar->fUsedDyID_s0 = usedDy[0];
      bar->fUsedDyID_s1 = usedDy[1];
      if(usedDy[0] != 0 && usedDy[1] != 0){ // both has signal
        bar->fEnergy = TMath::Sqrt(adc8[0]*adc8[1] / fBgoMip.at(DmpBgoBase::ConstructGlobalBarID(l,b)).at(7));
      }else{ // one end has signal
        for(short s=0;s<2;++s){
          if(usedDy[s] != 0){
            bar->fEnergy = adc8[s] / fBgoMip.at(DmpBgoBase::ConstructGlobalDynodeID(l,b,s,8)).at(7);
            // TODO 
            //  position correction
            break;
          }
        }
      }
      bar->fPosition = DmpBgoBase::Parameter()->BarCenter(l,b);
    }
  }

  return true;
}

//-------------------------------------------------------------------
bool DmpAlgRec1_Hits::Finalize(){
  return true;
}

double DmpAlgRec1_Hits::_trans2Dy8_Bgo(short l,short b,short s,short dyID,double sig)
{
  double v=0;
  short gid = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,dyID);
  if(dyID == 5){
    v = (sig - fBgoRel.at(gid).at(4)) / fBgoRel.at(gid).at(5);
  }else if(dyID == 2){
    v = (sig - fBgoRel.at(gid).at(4)) / fBgoRel.at(gid).at(5);
    gid = DmpBgoBase::ConstructGlobalDynodeID(l,b,s,5);
    v = (v - fBgoRel.at(gid).at(4)) / fBgoRel.at(gid).at(5);
  }else{
   DmpLogError<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\tdyID should be 2 | 5 | 8"<<std::endl;
    throw;
  }
  return v;
}

