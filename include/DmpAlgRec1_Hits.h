/*
 *  $Id: DmpAlgRec1_Hits.h, 2015-04-29 16:40:59 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 19/07/2014
*/

#ifndef DmpAlgRec1_Hits_H
#define DmpAlgRec1_Hits_H

#include "DmpLoadParameters.h"
#include "DmpEvtPsdRaw.h"
#include "DmpEvtBgoHits2.h"

#include "DmpVAlg.h"
class DmpEvtNudRaw;

class DmpAlgRec1_Hits : public DmpVAlg{
/*
 *  DmpAlgRec1_Hits
 *
 */
public:
  DmpAlgRec1_Hits();
  ~DmpAlgRec1_Hits();

  void SetPedestalFile(std::string detectorID,std::string filename);
  void SetRelationFile(std::string detectorID,std::string filename);
  void SetMipsFile(std::string detectorID,std::string filename);
  void SetCutOverflow(int v) {_fCutHigADC = v;}

  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

private:    // input
  DmpEvtBgoRaw          *fEvtBgo;
  DmpEvtPsdRaw          *fEvtPsd;
  DmpEvtNudRaw          *fEvtNud;

private:    // output
  DmpEvtBgoHits2        *fOutBgo;
  DmpEvtPsdHits         *fOutPsd;

private:
  DmpParameterHolder    fBgoRel; 
  DmpParameterHolder    fPsdRel; 

  DmpParameterHolder    fBgoPed; 
  DmpParameterHolder    fPsdPed; 

  DmpParameterHolder    fBgoMip; 
  DmpParameterHolder    fPsdMip; 

  std::map<short,double>  fTotSigmaBgo;
  std::map<short,double>  fTotSigmaPsd;

  //DmpParameterHolder    fBgoLamda;
  //DmpParameterHolder    fPsdLamda;

private:
  int   _fCutHigADC;         // dynode high range

private:
  bool _loadParametes();
  double _trans2Dy8_Bgo(short l,short b,short s,short dyID,double adcSmall);

};

#endif

