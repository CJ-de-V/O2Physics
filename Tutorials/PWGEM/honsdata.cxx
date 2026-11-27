// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file honsdata.cxx
/// \brief Simple task for local MC-Troubleshooting
/// \author C.J. de Villiers <cornelius.johannes.de.villiers@cern.ch>

#include "PWGDQ/Core/MCSignalLibrary.h"

#include "Common/DataModel/TrackSelectionTables.h"

#include <Framework/AnalysisDataModel.h>
#include <Framework/AnalysisTask.h>
#include <Framework/EndOfStreamContext.h>
#include <Framework/Expressions.h>
#include <Framework/Logger.h>
#include <Framework/runDataProcessing.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

std::ofstream muonTracksOut("muontracks.csv");
std::ofstream outputLog("decaylog.txt");

// struct to hold track data until final write
// struct TrackRecord {
//   float eta;
//   float pt;
//   float p;
//   float phi;
//   int motherPDG;
//   int nClusters;
//   int trackType;
//   float pDca;
//   float chi2;
//   float chi2MCHMID;
//   int isPrompt;
// };

struct myExampleTask {
  HistogramRegistry histos{"histos", {}, OutputObjHandlingPolicy::AnalysisObject};
  Configurable<std::string> fConfigMCSignals{"cfgMCsignals", "", "Comma-separated list of MC signals"};
  Configurable<std::string> fConfigMCSignalsJSON{"cfgMCsignalsJSON", "", "Additional list of MC signals via JSON"};
  std::vector<MCSignal*> mcSignals;
  // std::unordered_map<int64_t, TrackRecord> bestTracks; // MC index -> best track data

  void init(InitContext const&)
  {
    muonTracksOut << "collisionID,trackID,mcID,eta,pt,p,phi,motherPDG,nClusters,trackType,pDca,chi2,chi2MatchMCHMID,rabs,isPrompt\n";

    std::stringstream signalNames{fConfigMCSignals.value};
    std::string signalName;
    while (std::getline(signalNames, signalName, ',')) {
      auto first = signalName.find_first_not_of(" \t");
      if (first == std::string::npos) {
        continue;
      }
      auto last = signalName.find_last_not_of(" \t");
      signalName = signalName.substr(first, last - first + 1);
      auto signal = o2::aod::dqmcsignals::GetMCSignal(signalName.c_str());
      if (signal == nullptr) {
        LOG(warning) << "Unknown MC signal '" << signalName << "'";
        continue;
      }
      mcSignals.push_back(signal);
    }

    if (!fConfigMCSignalsJSON.value.empty()) {
      auto jsonSignals = o2::aod::dqmcsignals::GetMCSignalsFromJSON(fConfigMCSignalsJSON.value.c_str());
      for (auto signal : jsonSignals) {
        if (signal != nullptr) {
          mcSignals.push_back(signal);
        }
      }
    }
  }

  using muonTracks = soa::Join<aod::FwdTracks, aod::McFwdTrackLabels>; // Forward tracks only

  // Filter trackFilter = (aod::fwdtrack::chi2MatchMCHMID > -2.f) && // Valid MCHMID match //all filters set to not filter at all to test
  //                      (aod::fwdtrack::chi2 > 0.f) &&             // No nonsense values
  //                      (aod::fwdtrack::chi2MatchMCHMFT > -2.f);   // No MFT entries
  //                                                                 // no mft nonsense --- in this case Do we need the whole select lowest chi2 shebang; yes small number of actual dupes

  //  (aod::mcfwdtracklabel::mcParticleId >= 0) && // has matched MC particle
  //  (nabs(aod::mcparticle::pdgCode) == 13);    // muon PDG ---There was an attempt at filtering here instead of in the loop- unsuccesful...why?

  // reworked to run over tracks instead (that way we can write to file herein, since we can't get functions to execute post data extraction) aod::Collision const& collision,
  // void process(soa::Filtered<muonTracks> const& tracks, aod::McParticles const&)
  void process(aod::Collision const& collision, muonTracks const& tracks, aod::McParticles const&)
  {

    for (auto const& track : tracks) {
      if (!track.has_mcParticle()) // Have a Monte Carlo particle
        continue;
      auto mcParticle = track.mcParticle();
      if (abs(mcParticle.pdgCode()) != 13) // Is a muon
        continue;
      auto muID = mcParticle.globalIndex(); // check this later globalindex might be a bad idea

      std::string signalBitmap;
      signalBitmap.reserve(mcSignals.size());
      for (auto* signal : mcSignals) {
        signalBitmap.push_back(signal->CheckSignal(true, mcParticle) ? '1' : '0');
      }

      // look for HF mother
      // if no mother skip
      if (!mcParticle.has_mothers()) {
        std::cout << "No mother for muon with id: " << muID << std::endl;
        continue;
      }
      auto muMother = mcParticle.mothers_first_as<aod::McParticles>();
      auto muMotherPDG = abs(muMother.pdgCode()); // Muon's mother's PDG
      auto motherStatusCode = muMother.getGenStatusCode();
      auto mcPart(muMother);                  // Makes a copy of muMother and stores it in mcpart
      auto prevMcPart(muMother);              // Same as above... though god knows why we'd ever use this syntax
      auto mcPartPDG = abs(mcPart.pdgCode()); // parent
      int isPrompt = 1;
      outputLog << signalBitmap << ", 13"; //"==== Forward muon decay chain for id: " << muID << " : mu";

      // of questionable theoretical rigidity && (abs(mcPart.getGenStatusCode()) > 80 || mcPart.getGenStatusCode() == 0)
      while (mcPart.has_mothers()) {
        outputLog << ", " << mcPartPDG;
        prevMcPart = *(mcPart);
        mcPart = *(mcPart.mothers_first_as<aod::McParticles>());
        mcPartPDG = abs(mcPart.pdgCode());
      }
      outputLog << std::endl;

      // this one is actually just buggy, fails for the >1k ones (granted we do not care for this)
      if (div(abs(prevMcPart.pdgCode()), 100).quot != div(muMotherPDG, 100).quot)
        isPrompt = 0;
      // outputLog << "; isPrompt = " << isPrompt << std::endl;

      // auto eee = track.rowIndex();

      // Spit out all the information
      muonTracksOut << std::to_string(track.collisionId()) << "," << std::to_string(track.matchMCHTrackId()) << "," << muID << "," << track.eta() << "," << track.pt() << "," << track.p() << "," << track.phi() << ","
                    << muMotherPDG << "," << std::to_string(track.nClusters()) << "," << std::to_string(track.trackType()) << "," << track.pDca() << "," << track.chi2() << ","
                    << track.chi2MatchMCHMID() << "," << track.rAtAbsorberEnd() << "," << isPrompt << "\n";
      // if (bestTracks.find(muID) == bestTracks.end() || track.chi2() < bestTracks[muID].chi2) {
      //   bestTracks[muID] = {
      //     track.eta(), // We want to select the reconstructed tracks.... I think? since the table we joined has 2 eta entries we need to specify which one - we grab the forwardtrack
      //     track.pt(),
      //     track.p(),
      //     track.phi(),
      //     muMotherPDG,
      //     track.nClusters(),
      //     track.trackType(),
      //     track.pDca(),
      //     track.chi2(),
      //     track.chi2MatchMCHMID(),
      //     isPrompt};
      // }
    }

    // for (auto& [id, tr] : bestTracks) {
    //   // muonTracksOut << id << "," << tr.eta << "," << tr.pt << "," << tr.p << "," << tr.phi << ","
    //   //               << tr.motherPDG << "," << tr.nClusters << "," << tr.trackType<<","<< tr.pDca << "," << tr.chi2 << ","
    //   //               << tr.chi2MCHMID << "," << tr.isPrompt << "\n";
    //   // disabled while testing for the tracktype option
    // }
    // muonTracksOut.close();
    // outputLog.close(); //attempting to remove this since somehow we now never write???? //issue seems to have been with closing the files, worked
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<myExampleTask>(cfgc, TaskName{"hons-data"})};
}
