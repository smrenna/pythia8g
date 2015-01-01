// LHAPDF6.h is a part of the PYTHIA event generator.
// Copyright (C) 2015 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This file contains the LHAPDF6 PDF plugin class.

#ifndef Pythia8_LHAPDF6_H
#define Pythia8_LHAPDF6_H

#include "Pythia8/PartonDistributions.h"
#include "LHAPDF/LHAPDF.h"

namespace Pythia8 {

//==========================================================================

// Global tracking of opened PDF sets.

//--------------------------------------------------------------------------

namespace LHAPDF6Interface {

  // Define opened PDF sets global variable.
  map< int, pair< ::LHAPDF::PDF*, int > > initializedSets;

}

//==========================================================================

// Provide interface to the LHAPDF6 library of parton densities.

class LHAPDF6 : public PDF {

public:

  // Constructor.
  LHAPDF6(int idBeamIn, string setName, int member,  int,
          Info* infoPtr) : PDF(idBeamIn), id (-1), pdf(0), extrapol(false)
    {init(setName, member, infoPtr);}

  // Destructor.
  ~LHAPDF6();

  // Allow extrapolation beyond boundaries (not implemented).
  void setExtrapolate(bool extrapolIn) {extrapol = extrapolIn;}

private:

  // The LHAPDF objects.
  int id;
  ::LHAPDF::PDF *pdf;
  ::LHAPDF::Extrapolator *ext;
  bool extrapol;

  // Initialization of PDF set.
  void init(string setName, int member, Info* infoPtr);

  // Update parton densities.
  void xfUpdate(int id, double x, double Q2);

};

//--------------------------------------------------------------------------

// Destructor.

LHAPDF6::~LHAPDF6() {
  map< int, pair< ::LHAPDF::PDF*, int > >::iterator set =
    LHAPDF6Interface::initializedSets.find(id);
  if (set == LHAPDF6Interface::initializedSets.end()) return;
  set->second.second--;
  if (set->second.second == 0) {
    delete set->second.first;
    LHAPDF6Interface::initializedSets.erase(set);
  }

}

//--------------------------------------------------------------------------

// Initialize a parton density function from LHAPDF6.

void LHAPDF6::init(string setName, int member, Info *info) {
  isSet = false;

  // Initialize the set if not initialized.
  id = ::LHAPDF::lookupLHAPDFID(setName, member);
  if (id < 0) {
    info->errorMsg("Error in LHAPDF6::init: unknown PDF " + setName);
    return;
  } else if (LHAPDF6Interface::initializedSets.find(id) ==
             LHAPDF6Interface::initializedSets.end()) {
    pdf = ::LHAPDF::mkPDF(id);
    if (!pdf) {
      info->errorMsg("Error in LHAPDF6::init: could not initilaize PDF "
                     + setName);
      return;
    } else LHAPDF6Interface::initializedSets[id] =
             pair< ::LHAPDF::PDF*, int>(pdf, 0);
  } else {
    pair< ::LHAPDF::PDF*, int > &set = LHAPDF6Interface::initializedSets[id];
    pdf = set.first;
    set.second++;
  }
  isSet = true;

}

//--------------------------------------------------------------------------

// Give the parton distribution function set from LHAPDF6.

void LHAPDF6::xfUpdate(int, double x, double Q2) {

  // Update values.
  xg     = pdf->xfxQ2(21, x, Q2);
  xu     = pdf->xfxQ2(2,  x, Q2);
  xd     = pdf->xfxQ2(1,  x, Q2);
  xs     = pdf->xfxQ2(3,  x, Q2);
  xubar  = pdf->xfxQ2(-2, x, Q2);
  xdbar  = pdf->xfxQ2(-1, x, Q2);
  xsbar  = pdf->xfxQ2(-3, x, Q2);
  xc     = pdf->xfxQ2(4,  x, Q2);
  xb     = pdf->xfxQ2(5,  x, Q2);
  xgamma = pdf->xfxQ2(22, x, Q2);

  // Subdivision of valence and sea.
  xuVal  = xu - xubar;
  xuSea  = xubar;
  xdVal  = xd - xdbar;
  xdSea  = xdbar;

  // idSav = 9 to indicate that all flavours reset.
  idSav = 9;

}

//--------------------------------------------------------------------------

// Define external handles to the plugin for dynamic loading.

extern "C" LHAPDF6* newLHAPDF(int idBeamIn, string setName, int member,
                               Info* infoPtr) {
  return new LHAPDF6(idBeamIn, setName, member, 1, infoPtr);

}

extern "C" void deleteLHAPDF(LHAPDF6* pdf) {
  delete pdf;

}

//==========================================================================

} // end namespace Pythia8

#endif // end Pythia8_LHAPDF6_H
