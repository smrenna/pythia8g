// main45.cc is a part of the PYTHIA event generator.
// Copyright (C) 2015 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Author: Stephen Mrenna, mrenna@fnal.gov
// This program illustrates how to call Rivet directly
// from Pythia8 without introducing fifos or hepmc files.

#include "Pythia8/Pythia.h"
//#include "Pythia8Plugins/HepMC2.h"
//#include "Rivet/AnalysisHandler.hh"
//#include "HepMC/IO_GenEvent.h"

using namespace Pythia8;

int main(int argc, char* argv[]) {

  // Check that correct number of command-line arguments
  if (argc != 2) {
    cerr << " Unexpected number of command-line arguments. \n You are"
         << " expected to provide one input (parameter) file name. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  // Check that the provided input name corresponds to an existing file.
  ifstream is(argv[1]);
  if (!is) {
    cerr << " Command-line file " << argv[1] << " was not found. \n"
         << " Program stopped! " << endl;
    return 1;
  }

  // Interface for conversion from Pythia8::Event to HepMC event.
//  HepMC::Pythia8ToHepMC ToHepMC;

  // Specify file where HepMC events will be stored.
  //  HepMC::IO_GenEvent ascii_io(argv[2], std::ios::out);


  // Generator.
  Pythia pythia;

  pythia.settings.addWVec("UncertaintyVariations", vector<string>(1," "));
  // Read in commands from external file.
  pythia.readFile(argv[1]);

  // Extract settings to be used in the main program.
  int    nEvent    = pythia.mode("Main:numberOfEvents");
  int    nAbort    = pythia.mode("Main:timesAllowErrors");

/*  Rivet::AnalysisHandler ah;
  string rivetAnalysis=pythia.word("Main:spareWord1");
  bool doRivet = (rivetAnalysis != "");
  if( doRivet ) {
    ah.addAnalysis(rivetAnalysis);
    rivetAnalysis=pythia.word("Main:spareWord2");
    if( rivetAnalysis != "" ) ah.addAnalysis(rivetAnalysis);
  }
  cout << " doRivet = " << doRivet << endl;*/
  int wtType = pythia.mode("Main:spareMode1"); 

  // Initialization.
  pythia.init();
//  pythia.rndm.readState("weight0.rndm"); 
  Hist weight1("wt1",100,0.0,2.0);
  Hist weight2("wt2",150,0.0,3.0);

  cout << "Weight Type = " << wtType << endl;

  vector<string> uncert = pythia.settings.wvec("UncertaintyVariations");
  for (int i = 0; i < int(uncert.size()); ++i) 
    cout << " part " << i << " is \"" << uncert[i] << "\"" << endl; 

  
  // Begin event loop.
  int iAbort = 0;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

/*    if( iEvent == 8706 ) {
//      pythia.rndm.dumpState("weight0.rndm"); 
      cout << iEvent << endl;
      break;
    } */
    // Generate event.
    if (!pythia.next()) {

      // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) {
        cout << " Aborted since reached end of Les Houches Event File\n";
        break;
      }

      // First few failures write off as "acceptable" errors, then quit.
      if (++iAbort < nAbort) continue;
      cout << " Event generation aborted prematurely, owing to error!\n";
      break;
    }

    // Construct new empty HepMC event and fill it.
    // Units will be as chosen for HepMC build, but can be changed
    // by arguments, e.g. GenEvt( HepMC::Units::GEV, HepMC::Units::MM)
    if( iEvent < 100 ) cout << setprecision(10) << pythia.info.weight(1) << 
    " " << pythia.info.weight(2) << " " << pythia.info.weight(3) << " " << 
           pythia.info.weight(4) << endl;
/*    HepMC::GenEvent* hepmcevt = new HepMC::GenEvent();
    ToHepMC.fill_next_event( pythia, hepmcevt );
    if( wtType > 0 ) {
        double tempWeight = pythia.info.weight(wtType);
        if( tempWeight > 10.0 ) cout << tempWeight << " " << iEvent << endl;
        hepmcevt->weights()[0] = tempWeight;
    } */

//    if( doRivet ) ah.analyze( *hepmcevt );
    // Write the HepMC event to file. Done with it.
    //    ascii_io << hepmcevt;
 //   delete hepmcevt;

    // print out event weights
//    cout << "weights " << pythia.info.getWeightSave(1) << " "
//	 << pythia.info.getWeightSave(2) << endl;
//    weight1.fill(pythia.info.getWeightSave(1));
//    weight2.fill(pythia.info.getWeightSave(2));

//    if( pythia.info.getWeightSave(1) == 0.0 ) break;

  // End of event loop. Statistics.
  }
  pythia.stat();
/*  if( doRivet ) {
   ah.setCrossSection(pythia.info.sigmaGen() * 1.0E9);  
   ah.finalize();
   string yodaName = pythia.word("Main:spareWord3");
   ah.writeData(yodaName);
  } */
  cout << weight1 << endl;
  cout << weight2 << endl;
  // Done.
  return 0;
}
