// Reads reduced muons from MC and real-data TableMaker outputs. The MC and
// data joins are kept separate because only MC inputs provide truth labels.

#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/Configurable.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/runDataProcessing.h"

#include "PWGDQ/Core/AnalysisCut.h"
#include "PWGDQ/Core/CutsLibrary.h"
#include "PWGDQ/Core/VarManager.h"
#include "PWGDQ/DataModel/ReducedInfoTables.h"

#include <TAxis.h>
#include <THnSparse.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>

using namespace o2;
using namespace o2::framework;

namespace o2::aod
{
namespace muonreader
{
DECLARE_SOA_COLUMN(DcaXY, dcaXY, float);
} // namespace muonreader

DECLARE_SOA_TABLE(MuonSignalInfo, "AOD", "MUSIGINF",
                  o2::soa::Index<>,
                  // Keep these reconstructed quantities in the same order as MuonDataInfo.
                  reducedmuon::FilteringFlags,
                  reducedmuon::Pt, reducedmuon::Eta, reducedmuon::Phi, reducedmuon::Sign,
                  reducedmuon::IsAmbiguous,
                  reducedmuon::Px<reducedmuon::Pt, reducedmuon::Phi>,
                  reducedmuon::Py<reducedmuon::Pt, reducedmuon::Phi>,
                  reducedmuon::Pz<reducedmuon::Pt, reducedmuon::Eta>,
                  reducedmuon::P<reducedmuon::Pt, reducedmuon::Eta>,
                  fwdtrack::NClusters, fwdtrack::PDca, fwdtrack::RAtAbsorberEnd,
                  fwdtrack::Chi2, fwdtrack::Chi2MatchMCHMID, fwdtrack::Chi2MatchMCHMFT,
                  fwdtrack::MatchScoreMCHMFT, fwdtrack::MCHBitMap, fwdtrack::MIDBitMap,
                  fwdtrack::MIDBoards, fwdtrack::TrackType,
                  reducedmuon::FwdDcaX, reducedmuon::FwdDcaY, muonreader::DcaXY,
                  fwdtrack::TrackTime, fwdtrack::TrackTimeRes,
                  fwdtrack::X, fwdtrack::Y, fwdtrack::Z, reducedmuon::RawPhi, fwdtrack::Tgl, fwdtrack::Signed1Pt,
                  fwdtrack::CXX, fwdtrack::CXY, fwdtrack::CYY,
                  fwdtrack::CPhiX, fwdtrack::CPhiY, fwdtrack::CPhiPhi,
                  fwdtrack::CTglX, fwdtrack::CTglY, fwdtrack::CTglPhi, fwdtrack::CTglTgl,
                  fwdtrack::C1PtX, fwdtrack::C1PtY, fwdtrack::C1PtPhi,
                  fwdtrack::C1PtTgl, fwdtrack::C1Pt21Pt2,
                  // Truth labels are intentionally the final columns; real data has no labels.
                  reducedmuonlabel::McMask, reducedtrackMC::McReducedFlags);

namespace muondata
{
DECLARE_SOA_COLUMN(MftPt, mftPt, float);
DECLARE_SOA_COLUMN(MftEta, mftEta, float);
DECLARE_SOA_COLUMN(MftPhi, mftPhi, float);
DECLARE_SOA_COLUMN(MftSign, mftSign, int);
DECLARE_SOA_COLUMN(MftClusterSizesAndTrackFlags, mftClusterSizesAndTrackFlags, uint64_t);
DECLARE_SOA_COLUMN(MftNClusters, mftNClusters, int);
} // namespace muondata

DECLARE_SOA_TABLE(MuonDataInfo, "AOD", "MUDATA",
                  // Keep the shared reconstructed columns in exactly the MC-table order.
                  reducedmuon::FilteringFlags,
                  reducedmuon::Pt, reducedmuon::Eta, reducedmuon::Phi, reducedmuon::Sign,
                  reducedmuon::IsAmbiguous,
                  reducedmuon::Px<reducedmuon::Pt, reducedmuon::Phi>,
                  reducedmuon::Py<reducedmuon::Pt, reducedmuon::Phi>,
                  reducedmuon::Pz<reducedmuon::Pt, reducedmuon::Eta>,
                  reducedmuon::P<reducedmuon::Pt, reducedmuon::Eta>,
                  fwdtrack::NClusters, fwdtrack::PDca, fwdtrack::RAtAbsorberEnd,
                  fwdtrack::Chi2, fwdtrack::Chi2MatchMCHMID, fwdtrack::Chi2MatchMCHMFT,
                  fwdtrack::MatchScoreMCHMFT, fwdtrack::MCHBitMap, fwdtrack::MIDBitMap,
                  fwdtrack::MIDBoards, fwdtrack::TrackType,
                  reducedmuon::FwdDcaX, reducedmuon::FwdDcaY, muonreader::DcaXY,
                  fwdtrack::TrackTime, fwdtrack::TrackTimeRes,
                  fwdtrack::X, fwdtrack::Y, fwdtrack::Z, reducedmuon::RawPhi,
                  fwdtrack::Tgl, fwdtrack::Signed1Pt,
                  fwdtrack::CXX, fwdtrack::CXY, fwdtrack::CYY,
                  fwdtrack::CPhiX, fwdtrack::CPhiY, fwdtrack::CPhiPhi,
                  fwdtrack::CTglX, fwdtrack::CTglY, fwdtrack::CTglPhi, fwdtrack::CTglTgl,
                  fwdtrack::C1PtX, fwdtrack::C1PtY, fwdtrack::C1PtPhi,
                  fwdtrack::C1PtTgl, fwdtrack::C1Pt21Pt2,
                  // MFT-match details exist only in the real-data output.
                  muondata::MftPt, muondata::MftEta, muondata::MftPhi, muondata::MftSign,
                  muondata::MftClusterSizesAndTrackFlags, muondata::MftNClusters);
} // namespace o2::aod

using MyMuonTracksMC = soa::Join<aod::ReducedMuons, aod::ReducedMuonsExtra,
                                 aod::ReducedMuonsCov, aod::ReducedMuonsLabels>;
using MyMuonTracksRD = soa::Join<aod::ReducedMuons, aod::ReducedMuonsExtra, aod::ReducedMuonsCov>;
using MyMFTTracksRD = soa::Join<aod::ReducedMFTs, aod::ReducedMFTsExtra>;

constexpr uint32_t gkMuonFillMap = VarManager::ObjTypes::ReducedMuon |
                                   VarManager::ObjTypes::ReducedMuonExtra;

struct MuonVariables {
  float pt;
  float eta;
  float phi;
  int sign;
  uint8_t trackType;
  float chi2;
  float chi2MatchMCHMID;
  float chi2MatchMCHMFT;
  float matchScoreMCHMFT;
  uint16_t mchBitMap;
  uint8_t midBitMap;
  float dcaX;
  float dcaY;
  float dcaXY;
};

template <typename Muon>
MuonVariables extractMuonVariables(Muon const& muon)
{
  // Shared quantities used by both table writers and the template histogram.
  const auto dcaX = muon.fwdDcaX();
  const auto dcaY = muon.fwdDcaY();
  return {muon.pt(), muon.eta(), muon.phi(), muon.sign(), muon.trackType(),
          muon.chi2(), muon.chi2MatchMCHMID(), muon.chi2MatchMCHMFT(),
          muon.matchScoreMCHMFT(), muon.mchBitMap(), muon.midBitMap(),
          dcaX, dcaY, std::hypot(dcaX, dcaY)};
}

struct MuonReader {
  Produces<aod::MuonSignalInfo> muonSignalInfo;
  Produces<aod::MuonDataInfo> muonDataInfo;

  std::unique_ptr<AnalysisCut> muonQualityCut;

  ConfigurableAxis axisPt{"axisPt", {80, 0.0, 20.0}, "p_{T} (GeV/c)"};
  ConfigurableAxis axisDCAxy{"axisDCAxy", {1000, 0.0, 5.0}, "DCA_{XY} (cm)"};
  ConfigurableAxis axisMcReducedFlags{"axisMcReducedFlags", {65537, -1.5, 65.5},
                                      "MC reduced flags (-1 for real data)"};

  HistogramRegistry histos{"histos", {}, OutputObjHandlingPolicy::AnalysisObject};

  void init(InitContext const&)
  {
    // Use the shared DQ cut definition and tell VarManager which inputs it needs.
    muonQualityCut.reset(o2::aod::dqcuts::GetAnalysisCut("matchedQualityCutsMFTeta"));
    // FillTrack below must populate every variable used by this CutsLibrary cut.
    VarManager::SetUseVars(AnalysisCut::fgUsedVars);

    histos.add("hMuonTemplate", "Muon template; p_{T} (GeV/c); DCA_{XY} (cm); MC reduced flags",
               HistType::kTHnSparseF, {axisPt, axisDCAxy, axisMcReducedFlags});
    auto histogram = histos.get<THnSparse>(HIST("hMuonTemplate"));
    histogram->GetAxis(2)->SetBinLabel(1, "real data");
  }

  template <typename Muon>
  bool passesMuonQualityCut(Muon const& muon)
  {
    VarManager::FillTrack<gkMuonFillMap>(muon);
    return muonQualityCut->IsSelected(VarManager::fgValues);
  }

  template <typename Muon>
  bool isGlobalMuon(Muon const& muon) const
  {
    return muon.trackType() == 0;
  }

  std::unordered_map<int64_t, int> countMuonAssociations(aod::ReducedMuonsAssoc const& associations) const
  {
    std::unordered_map<int64_t, std::unordered_set<int64_t>> eventIdsByMuon;
    for (auto const& association : associations) {
      eventIdsByMuon[association.reducedmuonId()].insert(association.reducedeventId());
    }

    std::unordered_map<int64_t, int> countsByMuon;
    countsByMuon.reserve(eventIdsByMuon.size());
    for (auto const& [muonId, eventIds] : eventIdsByMuon) {
      countsByMuon.emplace(muonId, static_cast<int>(eventIds.size()));
    }
    return countsByMuon;
  }

  int getMuonAssociationCount(std::unordered_map<int64_t, int> const& countsByMuon, int64_t muonId) const
  {
    const auto it = countsByMuon.find(muonId);
    return it == countsByMuon.end() ? 0 : it->second;
  }

  template <typename Muon>
  void fillHistogram(Muon const& muon, int mcReducedFlags)
  {
    // Only global muons enter the template; the table paths retain the original track coverage.
    if (!isGlobalMuon(muon) || !passesMuonQualityCut(muon)) {
      return;
    }
    const auto variables = extractMuonVariables(muon);
    histos.fill(HIST("hMuonTemplate"), variables.pt, variables.dcaXY, mcReducedFlags);
  }

  void processMCTable(MyMuonTracksMC const& muons, aod::ReducedMuonsAssoc const& associations)
  {
    const auto associationCounts = countMuonAssociations(associations);
    for (auto const& muon : muons) {
      if (!passesMuonQualityCut(muon)) {
        continue;
      }
      const auto v = extractMuonVariables(muon);
      muonSignalInfo(muon.filteringFlags(), v.pt, v.eta, v.phi, v.sign,
                     getMuonAssociationCount(associationCounts, muon.globalIndex()),
                     muon.nClusters(), muon.pDca(), muon.rAtAbsorberEnd(),
                     v.chi2, v.chi2MatchMCHMID, v.chi2MatchMCHMFT, v.matchScoreMCHMFT,
                     v.mchBitMap, v.midBitMap, muon.midBoards(), v.trackType, v.dcaX, v.dcaY, v.dcaXY,
                     muon.trackTime(), muon.trackTimeRes(), muon.x(), muon.y(), muon.z(),
                     muon.rawPhi(), muon.tgl(), muon.signed1Pt(), muon.cXX(), muon.cXY(),
                     muon.cYY(), muon.cPhiX(), muon.cPhiY(), muon.cPhiPhi(), muon.cTglX(),
                     muon.cTglY(), muon.cTglPhi(), muon.cTglTgl(), muon.c1PtX(), muon.c1PtY(),
                     muon.c1PtPhi(), muon.c1PtTgl(), muon.c1Pt21Pt2(), muon.mcMask(),
                     muon.mcReducedFlags());
    }
  }

  void processMCHist(MyMuonTracksMC const& muons)
  {
    for (auto const& muon : muons) {
      // The label table stores the configured MC-signal decisions as a bitmask.
      fillHistogram(muon, muon.mcReducedFlags());
    }
  }

  void processRDTable(MyMuonTracksRD const& muons, MyMFTTracksRD const& mftTracks,
                      aod::ReducedMuonsAssoc const& associations)
  {
    const auto associationCounts = countMuonAssociations(associations);
    for (auto const& muon : muons) {
      if (!passesMuonQualityCut(muon)) {
        continue;
      }
      float mftPt = -999.f;
      float mftEta = -999.f;
      float mftPhi = -999.f;
      int mftSign = 0;
      uint64_t mftClusterSizesAndTrackFlags = 0;
      int mftNClusters = 0;
      if (muon.has_matchMFTTrack() && muon.matchMFTTrackId() >= 0 &&
          muon.matchMFTTrackId() < static_cast<int>(mftTracks.size())) {
        auto mftTrack = muon.template matchMFTTrack_as<MyMFTTracksRD>();
        mftPt = mftTrack.pt();
        mftEta = mftTrack.eta();
        mftPhi = mftTrack.phi();
        mftSign = mftTrack.sign();
        mftClusterSizesAndTrackFlags = mftTrack.mftClusterSizesAndTrackFlags();
        mftNClusters = mftTrack.mftNClusters();
      }
      const auto v = extractMuonVariables(muon);
      // The dynamic momentum columns are derived from pT/eta/phi, so the writer receives only stored columns.
      muonDataInfo(muon.filteringFlags(), v.pt, v.eta, v.phi, v.sign,
                   getMuonAssociationCount(associationCounts, muon.globalIndex()),
                   muon.nClusters(), muon.pDca(), muon.rAtAbsorberEnd(),
                   v.chi2, v.chi2MatchMCHMID, v.chi2MatchMCHMFT, v.matchScoreMCHMFT,
                   v.mchBitMap, v.midBitMap, muon.midBoards(), v.trackType,
                   v.dcaX, v.dcaY, v.dcaXY,
                   muon.trackTime(), muon.trackTimeRes(),
                   muon.x(), muon.y(), muon.z(), muon.rawPhi(), muon.tgl(), muon.signed1Pt(),
                   muon.cXX(), muon.cXY(), muon.cYY(), muon.cPhiX(), muon.cPhiY(), muon.cPhiPhi(),
                   muon.cTglX(), muon.cTglY(), muon.cTglPhi(), muon.cTglTgl(),
                   muon.c1PtX(), muon.c1PtY(), muon.c1PtPhi(), muon.c1PtTgl(), muon.c1Pt21Pt2(),
                   mftPt, mftEta, mftPhi, mftSign,
                   mftClusterSizesAndTrackFlags, mftNClusters);
    }
  }

  void processRDHist(MyMuonTracksRD const& muons)
  {
    for (auto const& muon : muons) {
      fillHistogram(muon, -1);
    }
  }

  // Each switch can be enabled independently or combined with any of the others.
  PROCESS_SWITCH(MuonReader, processMCTable, "Produce MC training table", false);
  PROCESS_SWITCH(MuonReader, processMCHist, "Produce MC template histogram", false);
  PROCESS_SWITCH(MuonReader, processRDTable, "Produce real-data table", true);
  PROCESS_SWITCH(MuonReader, processRDHist, "Produce real-data template histogram", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<MuonReader>(cfgc)};
}
