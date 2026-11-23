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

/// \file MuonMatchingMlResponse.h
/// \brief Class to compute the ML response for MFT-Muon matching
/// \author Maurice Coquet <maurice.louis.coquet@cern.ch>

#ifndef PWGDQ_CORE_MUONMATCHINGMLRESPONSE_H_
#define PWGDQ_CORE_MUONMATCHINGMLRESPONSE_H_

#include "Tools/ML/MlResponse.h"

#include <Framework/Logger.h>

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <string>
#include <vector>

// Fill the map of available input features
// the key is the feature's name (std::string)
// the value is the corresponding value in EnumInputFeatures
#define FILL_MAP_MFTMUON_MATCH(FEATURE) \
  {                                     \
    #FEATURE, static_cast<uint8_t>(InputFeaturesMFTMuonMatch::FEATURE)}

// Check if the index of mCachedIndices (index associated to a FEATURE)
// matches the entry in EnumInputFeatures associated to this FEATURE
// if so, the inputFeatures vector is filled with the FEATURE's value
// by calling the corresponding GETTER expression
#define CHECK_AND_FILL_FEATURE(FEATURE, GETTER)                    \
  case static_cast<uint8_t>(InputFeaturesMFTMuonMatch::FEATURE): { \
    inputFeature = (GETTER);                                       \
    break;                                                         \
  }

// Check if the index of mCachedIndices (index associated to a FEATURE)
// matches the entry in EnumInputFeatures associated to this FEATURE
// if so, and if OBJECT.GETTER() is a valid function invocation,
// the inputFeatures vector is filled with the FEATURE's value
// by calling the corresponding GETTER function
#define CHECK_AND_FILL_FEATURE_OPTIONAL_NO_EXPR(FEATURE, OBJECT, GETTER) \
  case static_cast<uint8_t>(InputFeaturesMFTMuonMatch::FEATURE): {       \
    if constexpr (requires(decltype(OBJECT) t) { { t.GETTER() } -> std::convertible_to<float>; }) {                    \
      inputFeature = (OBJECT.GETTER());                                  \
    } else {                                                             \
      inputFeature = 0;                                                  \
    }                                                                    \
    break;                                                               \
  }

// Check if the index of mCachedIndices (index associated to a FEATURE)
// matches the entry in EnumInputFeatures associated to this FEATURE
// if so, and if OBJECT.FUNC() is a valid function invocation,
// the inputFeatures vector is filled with the FEATURE's value
// by calling the corresponding GETTER expression
#define CHECK_AND_FILL_FEATURE_OPTIONAL_WITH_EXPR(FEATURE, OBJECT, FUNC, GETTER) \
  case static_cast<uint8_t>(InputFeaturesMFTMuonMatch::FEATURE): {               \
    if constexpr (requires(decltype(OBJECT) t) { { t.FUNC() } -> std::convertible_to<float>; }) {                            \
      inputFeature = (GETTER);                                                   \
    } else {                                                                     \
      inputFeature = 0;                                                          \
    }                                                                            \
    break;                                                                       \
  }

#define __EXPAND(x) x
#define __GET_MACRO(_1, _2, _3, _4, name, ...) name
#define CHECK_AND_FILL_FEATURE_OPTIONAL(...) __EXPAND(__GET_MACRO(__VA_ARGS__, CHECK_AND_FILL_FEATURE_OPTIONAL_WITH_EXPR, CHECK_AND_FILL_FEATURE_OPTIONAL_NO_EXPR)(__VA_ARGS__))

namespace o2::analysis
{
// possible input features for ML
enum class InputFeaturesMFTMuonMatch : uint8_t {
  zMatching,
  // MFT track parameters
  xMFT,
  yMFT,
  qOverptMFT,
  tglMFT,
  phiMFT,
  ptMFT,
  pMFT,
  etaMFT,
  timeMFT,
  timeResMFT,
  clusterSizesAndTrackFlagsMFT,
  trackTypeMFT,
  dcaXY,
  dcaZ,
  chi2MFT,
  nClustersMFT,
  // MCH track parameters
  xMCH,
  yMCH,
  qOverptMCH,
  tglMCH,
  phiMCH,
  ptMCH,
  pMCH,
  etaMCH,
  timeMCH,
  timeResMCH,
  nClustersMCH,
  chi2MCH,
  pdca,
  Rabs,
  // MFT covariances
  cXXMFT,
  cXYMFT,
  cYYMFT,
  cPhiYMFT,
  cPhiXMFT,
  cPhiPhiMFT,
  cTglYMFT,
  cTglXMFT,
  cTglPhiMFT,
  cTglTglMFT,
  c1PtYMFT,
  c1PtXMFT,
  c1PtPhiMFT,
  c1PtTglMFT,
  c1Pt21Pt2MFT,
  // MCH covariances
  cXXMCH,
  cXYMCH,
  cYYMCH,
  cPhiYMCH,
  cPhiXMCH,
  cPhiPhiMCH,
  cTglYMCH,
  cTglXMCH,
  cTglPhiMCH,
  cTglTglMCH,
  c1PtYMCH,
  c1PtXMCH,
  c1PtPhiMCH,
  c1PtTglMCH,
  c1Pt21Pt2MCH,
  // track residuals
  sameSign,
  deltaX,
  deltaY,
  deltaPhi,
  deltaTgl,
  deltaEta,
  delta1Pt,
  deltaPt,
  deltaP,
  deltaR,
  deltaDirection,
  pullX,
  pullY,
  pullPhi,
  pullTgl,
  pullEta,
  pullPt,
  pullR,
  deltaPtRel,
  deltaPRel,
  // track residuals (absolute values)
  absDeltaX,
  absDeltaY,
  absDeltaPhi,
  absDeltaTgl,
  absDeltaEta,
  absDeltaPt,
  absDeltaP,
  absDeltaR,
  absDeltaDirection,
  absPullX,
  absPullY,
  absPullPhi,
  absPullTgl,
  absPullEta,
  absPullPt,
  absPullR,
  absDeltaPtRel,
  // primary vertex parameters
  posX,
  posY,
  posZ,
  numContrib,
  trackOccupancyInTimeRange,
  ft0cOccupancyInTimeRange,
  multMFT,
  multFT0A,
  multFT0C,
  multNTracksPV,
  multNTracksPVeta1,
  multNTracksPVetaHalf,
  isInelGt0,
  isInelGt1,
  multFT0M,
  centFT0M,
  centFT0A,
  centFT0C,
  // global forward track parameters
  chi2MCHMFT,
  chi2GlobMUON,
  dcaX,
  dcaY,
  isAmbig,
  // matching parameters for second matching plane
  zMatching2,
  // MFT track parameters
  xMFT2,
  yMFT2,
  qOverptMFT2,
  tglMFT2,
  phiMFT2,
  ptMFT2,
  pMFT2,
  etaMFT2,
  // timeMFT2,
  // timeResMFT2,
  // clusterSizesAndTrackFlagsMFT2,
  // trackTypeMFT2,
  // dcaXY2,
  // dcaZ2,
  // chi2MFT2,
  // nClustersMFT2,
  // MCH track parameters
  xMCH2,
  yMCH2,
  qOverptMCH2,
  tglMCH2,
  phiMCH2,
  ptMCH2,
  pMCH2,
  etaMCH2,
  // timeMCH2,
  // timeResMCH2,
  // nClustersMCH2,
  // chi2MCH2,
  // pdca2,
  // Rabs2,
  // MFT covariances
  cXXMFT2,
  cXYMFT2,
  cYYMFT2,
  cPhiYMFT2,
  cPhiXMFT2,
  cPhiPhiMFT2,
  cTglYMFT2,
  cTglXMFT2,
  cTglPhiMFT2,
  cTglTglMFT2,
  c1PtYMFT2,
  c1PtXMFT2,
  c1PtPhiMFT2,
  c1PtTglMFT2,
  c1Pt21Pt2MFT2,
  // MCH covariances
  cXXMCH2,
  cXYMCH2,
  cYYMCH2,
  cPhiYMCH2,
  cPhiXMCH2,
  cPhiPhiMCH2,
  cTglYMCH2,
  cTglXMCH2,
  cTglPhiMCH2,
  cTglTglMCH2,
  c1PtYMCH2,
  c1PtXMCH2,
  c1PtPhiMCH2,
  c1PtTglMCH2,
  c1Pt21Pt2MCH2,
  // track residuals
  sameSign2,
  deltaX2,
  deltaY2,
  deltaPhi2,
  deltaTgl2,
  deltaEta2,
  delta1Pt2,
  deltaPt2,
  deltaP2,
  deltaR2,
  deltaDirection2,
  pullX2,
  pullY2,
  pullPhi2,
  pullTgl2,
  pullEta2,
  pullPt2,
  pullR2,
  deltaPtRel2,
  deltaPRel2,
  // track residuals (absolute values)
  absDeltaX2,
  absDeltaY2,
  absDeltaPhi2,
  absDeltaTgl2,
  absDeltaEta2,
  absDeltaPt2,
  absDeltaP2,
  absDeltaR2,
  absDeltaDirection2,
  absPullX2,
  absPullY2,
  absPullPhi2,
  absPullTgl2,
  absPullEta2,
  absPullPt2,
  absPullR2,
  absDeltaPtRel2,
  // primary vertex parameters
  // posX2,
  // posY2,
  // posZ2,
  // numContrib2,
  // trackOccupancyInTimeRange2,
  // ft0cOccupancyInTimeRange2,
  // multMFT2,
  // multFT0A2,
  // multFT0C2,
  // multNTracksPV2,
  // multNTracksPVeta12,
  // multNTracksPVetaHalf2,
  // isInelGt02,
  // isInelGt12,
  // multFT0M2,
  // centFT0M2,
  // centFT0A2,
  // centFT0C2,
  // // global forward track parameters
  // chi2MCHMFT2,
  // chi2GlobMUON2,
  // dcaX2,
  // dcaY2,
  // isAmbig2,
};

template <typename T1, typename T2>
float getDeltaR(T1 const& mftprop, T2 const& mchprop)
{
  return std::sqrt((mchprop.getX() - mftprop.getX()) * (mchprop.getX() - mftprop.getX()) + (mchprop.getY() - mftprop.getY()) * (mchprop.getY() - mftprop.getY()));
}

template <typename T1, typename T2>
float getPullR(T1 const& mftprop, T2 const& mchprop)
{
  double deltaR = getDeltaR(mftprop, mchprop);
  double err2X = mftprop.getCovariances()(0, 0) + mchprop.getCovariances()(0, 0);
  double err2Y = mftprop.getCovariances()(1, 1) + mchprop.getCovariances()(1, 1);
  double errR = std::sqrt(err2X + err2Y);
  return deltaR / errR;
}

template <typename T1, typename T2>
float getPullPt(T1 const& mftprop, T2 const& mchprop)
{
  double invPtMFT = std::abs(mftprop.getInvQPt());
  double ptMFT = (invPtMFT != 0) ? 1.0 / invPtMFT : 0;
  double invPtErr2MFT = mftprop.getCovariances()(4, 4);
  double ptErr2MFT = invPtErr2MFT * ptMFT * ptMFT * ptMFT * ptMFT;

  double invPtMCH = std::abs(mchprop.getInvQPt());
  double ptMCH = (invPtMCH != 0) ? 1.0 / invPtMCH : 0;
  double invPtErr2MCH = mchprop.getCovariances()(4, 4);
  double ptErr2MCH = invPtErr2MCH * ptMCH * ptMCH * ptMCH * ptMCH;

  float delta = ptMCH - ptMFT;
  float err = std::sqrt(ptErr2MCH + ptErr2MFT);
  return (err > 0 ? delta / err : 0);
}

template <typename T1, typename T2>
float getDeltaDirection(T1 const& mftprop, T2 const& mchprop)
{
  double cos2 = std::cos(mchprop.getPhi()) * std::cos(mftprop.getPhi());
  double sin2 = std::sin(mchprop.getPhi()) * std::sin(mftprop.getPhi());
  double tgl2 = mchprop.getTgl() * mftprop.getTgl();
  double cosDelta = (cos2 + sin2 + tgl2) / (std::sqrt(mchprop.getTgl() * mchprop.getTgl() + 1) * std::sqrt(mftprop.getTgl() * mftprop.getTgl() + 1));
  return static_cast<float>(std::acos(std::clamp(cosDelta, -1.0, 1.0)));
}

float getPull(float mftVal, float mftErr2, float mchVal, float mchErr2)
{
  float delta = mchVal - mftVal;
  float err = std::sqrt(mchErr2 + mftErr2);
  return (err > 0 ? delta / err : delta);
}

float getRelDifference(float mftVal, float mchVal)
{
  return (mchVal - mftVal) / (mchVal + mftVal);
}

template <typename TypeOutputScore = float>
class MlResponseMFTMuonMatch : public MlResponse<TypeOutputScore>
{
 public:
  /// Default constructor
  MlResponseMFTMuonMatch() = default;
  /// Default destructor
  virtual ~MlResponseMFTMuonMatch() = default;

  template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename U>
  float returnFeature(uint8_t idx, T1 const& muon, T2 const& mft, T3 const& mch, T4 const& mftprop, T5 const& mchprop, T6 const& mftprop2, T7 const& mchprop2, U const& collision)
  {
    float inputFeature = 0.;
    switch (idx) {
      // matching parameters
      CHECK_AND_FILL_FEATURE(zMatching, mftprop.getZ());
      // MFT track parameters
      CHECK_AND_FILL_FEATURE(xMFT, mftprop.getX());
      CHECK_AND_FILL_FEATURE(yMFT, mftprop.getY());
      CHECK_AND_FILL_FEATURE(qOverptMFT, mftprop.getInvQPt());
      CHECK_AND_FILL_FEATURE(tglMFT, mftprop.getTanl());
      CHECK_AND_FILL_FEATURE(phiMFT, mftprop.getPhi());
      CHECK_AND_FILL_FEATURE(chi2MFT, mft.chi2());
      CHECK_AND_FILL_FEATURE(nClustersMFT, mft.nClusters());
      /*dummy value*/ CHECK_AND_FILL_FEATURE(dcaXY, 0);
      /*dummy value*/ CHECK_AND_FILL_FEATURE(dcaZ, 0);
      CHECK_AND_FILL_FEATURE(ptMFT, mftprop.getPt());
      CHECK_AND_FILL_FEATURE(pMFT, mftprop.getP());
      CHECK_AND_FILL_FEATURE(etaMFT, mftprop.getEta());
      CHECK_AND_FILL_FEATURE(timeMFT, mft.trackTime());
      CHECK_AND_FILL_FEATURE(timeResMFT, mft.trackTimeRes());
      CHECK_AND_FILL_FEATURE(clusterSizesAndTrackFlagsMFT, mft.mftClusterSizesAndTrackFlags());
      CHECK_AND_FILL_FEATURE(trackTypeMFT, (mft.isCA() ? 1 : 0));
      // MCH track parameters
      CHECK_AND_FILL_FEATURE(xMCH, mchprop.getX());
      CHECK_AND_FILL_FEATURE(yMCH, mchprop.getY());
      CHECK_AND_FILL_FEATURE(qOverptMCH, mchprop.getInvQPt());
      CHECK_AND_FILL_FEATURE(tglMCH, mchprop.getTanl());
      CHECK_AND_FILL_FEATURE(phiMCH, mchprop.getPhi());
      CHECK_AND_FILL_FEATURE(ptMCH, mchprop.getPt());
      CHECK_AND_FILL_FEATURE(pMCH, mchprop.getP());
      CHECK_AND_FILL_FEATURE(etaMCH, mchprop.getEta());
      CHECK_AND_FILL_FEATURE(timeMCH, mch.trackTime());
      CHECK_AND_FILL_FEATURE(timeResMCH, mch.trackTimeRes());
      CHECK_AND_FILL_FEATURE(nClustersMCH, mch.nClusters());
      CHECK_AND_FILL_FEATURE(chi2MCH, mch.chi2());
      CHECK_AND_FILL_FEATURE(pdca, muon.pDca());
      CHECK_AND_FILL_FEATURE(Rabs, muon.rAtAbsorberEnd());
      // MFT covariances
      CHECK_AND_FILL_FEATURE(cXXMFT, mftprop.getCovariances()(0, 0));
      CHECK_AND_FILL_FEATURE(cXYMFT, mftprop.getCovariances()(0, 1));
      CHECK_AND_FILL_FEATURE(cYYMFT, mftprop.getCovariances()(1, 1));
      CHECK_AND_FILL_FEATURE(cPhiXMFT, mftprop.getCovariances()(0, 2));
      CHECK_AND_FILL_FEATURE(cPhiYMFT, mftprop.getCovariances()(1, 2));
      CHECK_AND_FILL_FEATURE(cPhiPhiMFT, mftprop.getCovariances()(2, 2));
      CHECK_AND_FILL_FEATURE(cTglXMFT, mftprop.getCovariances()(0, 3));
      CHECK_AND_FILL_FEATURE(cTglYMFT, mftprop.getCovariances()(1, 3));
      CHECK_AND_FILL_FEATURE(cTglPhiMFT, mftprop.getCovariances()(2, 3));
      CHECK_AND_FILL_FEATURE(cTglTglMFT, mftprop.getCovariances()(3, 3));
      CHECK_AND_FILL_FEATURE(c1PtXMFT, mftprop.getCovariances()(0, 4));
      CHECK_AND_FILL_FEATURE(c1PtYMFT, mftprop.getCovariances()(1, 4));
      CHECK_AND_FILL_FEATURE(c1PtPhiMFT, mftprop.getCovariances()(2, 4));
      CHECK_AND_FILL_FEATURE(c1PtTglMFT, mftprop.getCovariances()(3, 4));
      CHECK_AND_FILL_FEATURE(c1Pt21Pt2MFT, mftprop.getCovariances()(4, 4));
      // MCH covariances
      CHECK_AND_FILL_FEATURE(cXXMCH, mchprop.getCovariances()(0, 0));
      CHECK_AND_FILL_FEATURE(cXYMCH, mchprop.getCovariances()(0, 1));
      CHECK_AND_FILL_FEATURE(cYYMCH, mchprop.getCovariances()(1, 1));
      CHECK_AND_FILL_FEATURE(cPhiXMCH, mchprop.getCovariances()(0, 2));
      CHECK_AND_FILL_FEATURE(cPhiYMCH, mchprop.getCovariances()(1, 2));
      CHECK_AND_FILL_FEATURE(cPhiPhiMCH, mchprop.getCovariances()(2, 2));
      CHECK_AND_FILL_FEATURE(cTglXMCH, mchprop.getCovariances()(0, 3));
      CHECK_AND_FILL_FEATURE(cTglYMCH, mchprop.getCovariances()(1, 3));
      CHECK_AND_FILL_FEATURE(cTglPhiMCH, mchprop.getCovariances()(2, 3));
      CHECK_AND_FILL_FEATURE(cTglTglMCH, mchprop.getCovariances()(3, 3));
      CHECK_AND_FILL_FEATURE(c1PtXMCH, mchprop.getCovariances()(0, 4));
      CHECK_AND_FILL_FEATURE(c1PtYMCH, mchprop.getCovariances()(1, 4));
      CHECK_AND_FILL_FEATURE(c1PtPhiMCH, mchprop.getCovariances()(2, 4));
      CHECK_AND_FILL_FEATURE(c1PtTglMCH, mchprop.getCovariances()(3, 4));
      CHECK_AND_FILL_FEATURE(c1Pt21Pt2MCH, mchprop.getCovariances()(4, 4));
      // Track residuals
      CHECK_AND_FILL_FEATURE(sameSign, (mch.sign() == mft.sign()) ? 1 : 0);
      CHECK_AND_FILL_FEATURE(deltaX, mchprop.getX() - mftprop.getX());
      CHECK_AND_FILL_FEATURE(deltaY, mchprop.getY() - mftprop.getY());
      CHECK_AND_FILL_FEATURE(deltaPhi, mchprop.getPhi() - mftprop.getPhi());
      CHECK_AND_FILL_FEATURE(deltaTgl, mchprop.getTgl() - mftprop.getTgl());
      CHECK_AND_FILL_FEATURE(deltaEta, mchprop.getEta() - mftprop.getEta());
      CHECK_AND_FILL_FEATURE(delta1Pt, mchprop.getInvQPt() - mftprop.getInvQPt());
      CHECK_AND_FILL_FEATURE(deltaPt, mchprop.getPt() - mftprop.getPt());
      CHECK_AND_FILL_FEATURE(deltaP, mchprop.getP() - mftprop.getP());
      CHECK_AND_FILL_FEATURE(deltaR, getDeltaR(mftprop, mchprop));
      CHECK_AND_FILL_FEATURE(deltaDirection, getDeltaDirection(mftprop, mchprop));
      CHECK_AND_FILL_FEATURE(deltaPtRel, getRelDifference(mftprop.getPt(), mchprop.getPt()));
      CHECK_AND_FILL_FEATURE(deltaPRel, getRelDifference(mftprop.getP(), mchprop.getP()));
      CHECK_AND_FILL_FEATURE(pullX, getPull(mftprop.getX(), mftprop.getCovariances()(0, 0), mchprop.getX(), mchprop.getCovariances()(0, 0)));
      CHECK_AND_FILL_FEATURE(pullY, getPull(mftprop.getY(), mftprop.getCovariances()(1, 1), mchprop.getY(), mchprop.getCovariances()(1, 1)));
      CHECK_AND_FILL_FEATURE(pullPhi, getPull(mftprop.getPhi(), mftprop.getCovariances()(2, 2), mchprop.getPhi(), mchprop.getCovariances()(2, 2)));
      CHECK_AND_FILL_FEATURE(pullTgl, getPull(mftprop.getTgl(), mftprop.getCovariances()(3, 3), mchprop.getTgl(), mchprop.getCovariances()(3, 3)));
      /*dummy value*/ CHECK_AND_FILL_FEATURE(pullEta, 0);
      CHECK_AND_FILL_FEATURE(pullPt, getPullPt(mftprop, mchprop));
      CHECK_AND_FILL_FEATURE(pullR, getPullR(mftprop, mchprop));
      // Track residuals (absolute values)
      CHECK_AND_FILL_FEATURE(absDeltaX, std::fabs(mchprop.getX() - mftprop.getX()));
      CHECK_AND_FILL_FEATURE(absDeltaY, std::fabs(mchprop.getY() - mftprop.getY()));
      CHECK_AND_FILL_FEATURE(absDeltaPhi, std::fabs(mchprop.getPhi() - mftprop.getPhi()));
      CHECK_AND_FILL_FEATURE(absDeltaTgl, std::fabs(mchprop.getTgl() - mftprop.getTgl()));
      CHECK_AND_FILL_FEATURE(absDeltaEta, std::fabs(mchprop.getEta() - mftprop.getEta()));
      CHECK_AND_FILL_FEATURE(absDeltaPt, std::fabs(mchprop.getPt() - mftprop.getPt()));
      CHECK_AND_FILL_FEATURE(absDeltaP, std::fabs(mchprop.getP() - mftprop.getP()));
      CHECK_AND_FILL_FEATURE(absDeltaR, std::fabs(getDeltaR(mftprop, mchprop)));
      CHECK_AND_FILL_FEATURE(absDeltaDirection, std::fabs(getDeltaDirection(mftprop, mchprop)));
      CHECK_AND_FILL_FEATURE(absDeltaPtRel, std::fabs(getRelDifference(mftprop.getPt(), mchprop.getPt())));
      CHECK_AND_FILL_FEATURE(absPullX, std::fabs(getPull(mftprop.getX(), mftprop.getCovariances()(0, 0), mchprop.getX(), mchprop.getCovariances()(0, 0))));
      CHECK_AND_FILL_FEATURE(absPullY, std::fabs(getPull(mftprop.getY(), mftprop.getCovariances()(1, 1), mchprop.getY(), mchprop.getCovariances()(1, 1))));
      CHECK_AND_FILL_FEATURE(absPullPhi, std::fabs(getPull(mftprop.getPhi(), mftprop.getCovariances()(2, 2), mchprop.getPhi(), mchprop.getCovariances()(2, 2))));
      CHECK_AND_FILL_FEATURE(absPullTgl, std::fabs(getPull(mftprop.getTgl(), mftprop.getCovariances()(3, 3), mchprop.getTgl(), mchprop.getCovariances()(3, 3))));
      /*dummy value*/ CHECK_AND_FILL_FEATURE(absPullEta, 0);
      CHECK_AND_FILL_FEATURE(absPullPt, std::fabs(getPullPt(mftprop, mchprop)));
      CHECK_AND_FILL_FEATURE(absPullR, std::fabs(getPullR(mftprop, mchprop)));
      // primary vertex parameters
      CHECK_AND_FILL_FEATURE(posX, collision.posX());
      CHECK_AND_FILL_FEATURE(posY, collision.posY());
      CHECK_AND_FILL_FEATURE(posZ, collision.posZ());
      CHECK_AND_FILL_FEATURE_OPTIONAL(numContrib, collision, numContrib);
      CHECK_AND_FILL_FEATURE_OPTIONAL(trackOccupancyInTimeRange, collision, trackOccupancyInTimeRange);
      CHECK_AND_FILL_FEATURE_OPTIONAL(ft0cOccupancyInTimeRange, collision, ft0cOccupancyInTimeRange);
      CHECK_AND_FILL_FEATURE_OPTIONAL(multMFT, collision, mftNtracks);
      CHECK_AND_FILL_FEATURE_OPTIONAL(multFT0A, collision, multFT0A);
      CHECK_AND_FILL_FEATURE_OPTIONAL(multFT0C, collision, multFT0C);
      CHECK_AND_FILL_FEATURE_OPTIONAL(multNTracksPV, collision, multNTracksPV);
      CHECK_AND_FILL_FEATURE_OPTIONAL(multNTracksPVeta1, collision, multNTracksPVeta1);
      CHECK_AND_FILL_FEATURE_OPTIONAL(multNTracksPVetaHalf, collision, multNTracksPVetaHalf);
      CHECK_AND_FILL_FEATURE_OPTIONAL(isInelGt0, collision, isInelGt0);
      CHECK_AND_FILL_FEATURE_OPTIONAL(isInelGt1, collision, isInelGt1);
      CHECK_AND_FILL_FEATURE_OPTIONAL(multFT0M, collision, multFT0M);
      CHECK_AND_FILL_FEATURE_OPTIONAL(centFT0M, collision, centFT0M);
      CHECK_AND_FILL_FEATURE_OPTIONAL(centFT0A, collision, centFT0A);
      CHECK_AND_FILL_FEATURE_OPTIONAL(centFT0C, collision, centFT0C);
      // global forward track parameters
      CHECK_AND_FILL_FEATURE(chi2MCHMFT, muon.chi2MatchMCHMFT());
      CHECK_AND_FILL_FEATURE(chi2GlobMUON, muon.chi2());
      CHECK_AND_FILL_FEATURE_OPTIONAL(dcaX, muon, fwdDcaX);
      CHECK_AND_FILL_FEATURE_OPTIONAL(dcaY, muon, fwdDcaY);
      CHECK_AND_FILL_FEATURE_OPTIONAL(isAmbig, muon, compatibleCollIds, (muon.compatibleCollIds().size() == 1) ? 0 : 1)

      // plane 2
      // matching parameters
      CHECK_AND_FILL_FEATURE(zMatching2, mftprop2.getZ());
      // MFT2 track parameters
      CHECK_AND_FILL_FEATURE(xMFT2, mftprop2.getX());
      CHECK_AND_FILL_FEATURE(yMFT2, mftprop2.getY());
      CHECK_AND_FILL_FEATURE(qOverptMFT2, mftprop2.getInvQPt());
      CHECK_AND_FILL_FEATURE(tglMFT2, mftprop2.getTanl());
      CHECK_AND_FILL_FEATURE(phiMFT2, mftprop2.getPhi());
      // CHECK_AND_FILL_FEATURE(chi2MFT2, mft.chi2());
      // CHECK_AND_FILL_FEATURE(nClustersMFT2, mft.nClusters());
      // /*dummy value*/ CHECK_AND_FILL_FEATURE(dcaXY, 0);
      // /*dummy value*/ CHECK_AND_FILL_FEATURE(dcaZ, 0);
      CHECK_AND_FILL_FEATURE(ptMFT2, mftprop2.getPt());
      CHECK_AND_FILL_FEATURE(pMFT2, mftprop2.getP());
      CHECK_AND_FILL_FEATURE(etaMFT2, mftprop2.getEta());
      // CHECK_AND_FILL_FEATURE(timeMFT2, mft.trackTime());
      // CHECK_AND_FILL_FEATURE(timeResMFT2, mft.trackTimeRes());
      // CHECK_AND_FILL_FEATURE(clusterSizesAndTrackFlagsMFT2, mft.mftClusterSizesAndTrackFlags());
      // CHECK_AND_FILL_FEATURE(trackTypeMFT2, (mft.isCA() ? 1 : 0));
      // MCH2 track parameters
      CHECK_AND_FILL_FEATURE(xMCH2, mchprop2.getX());
      CHECK_AND_FILL_FEATURE(yMCH2, mchprop2.getY());
      CHECK_AND_FILL_FEATURE(qOverptMCH2, mchprop2.getInvQPt());
      CHECK_AND_FILL_FEATURE(tglMCH2, mchprop2.getTanl());
      CHECK_AND_FILL_FEATURE(phiMCH2, mchprop2.getPhi());
      CHECK_AND_FILL_FEATURE(ptMCH2, mchprop2.getPt());
      CHECK_AND_FILL_FEATURE(pMCH2, mchprop2.getP());
      // CHECK_AND_FILL_FEATURE(etaMCH2, mchprop2.getEta());
      // CHECK_AND_FILL_FEATURE(timeMCH2, mch.trackTime());
      // CHECK_AND_FILL_FEATURE(timeResMCH2, mch.trackTimeRes());
      // CHECK_AND_FILL_FEATURE(nClustersMCH2, mch.nClusters());
      // CHECK_AND_FILL_FEATURE(chi2MCH2, mch.chi2());
      // CHECK_AND_FILL_FEATURE(pdca, muon.pDca());
      // CHECK_AND_FILL_FEATURE(Rabs, muon.rAtAbsorberEnd());
      // MFT2 covariances
      CHECK_AND_FILL_FEATURE(cXXMFT2, mftprop2.getCovariances()(0, 0));
      CHECK_AND_FILL_FEATURE(cXYMFT2, mftprop2.getCovariances()(0, 1));
      CHECK_AND_FILL_FEATURE(cYYMFT2, mftprop2.getCovariances()(1, 1));
      CHECK_AND_FILL_FEATURE(cPhiXMFT2, mftprop2.getCovariances()(0, 2));
      CHECK_AND_FILL_FEATURE(cPhiYMFT2, mftprop2.getCovariances()(1, 2));
      CHECK_AND_FILL_FEATURE(cPhiPhiMFT2, mftprop2.getCovariances()(2, 2));
      CHECK_AND_FILL_FEATURE(cTglXMFT2, mftprop2.getCovariances()(0, 3));
      CHECK_AND_FILL_FEATURE(cTglYMFT2, mftprop2.getCovariances()(1, 3));
      CHECK_AND_FILL_FEATURE(cTglPhiMFT2, mftprop2.getCovariances()(2, 3));
      CHECK_AND_FILL_FEATURE(cTglTglMFT2, mftprop2.getCovariances()(3, 3));
      CHECK_AND_FILL_FEATURE(c1PtXMFT2, mftprop2.getCovariances()(0, 4));
      CHECK_AND_FILL_FEATURE(c1PtYMFT2, mftprop2.getCovariances()(1, 4));
      CHECK_AND_FILL_FEATURE(c1PtPhiMFT2, mftprop2.getCovariances()(2, 4));
      CHECK_AND_FILL_FEATURE(c1PtTglMFT2, mftprop2.getCovariances()(3, 4));
      CHECK_AND_FILL_FEATURE(c1Pt21Pt2MFT2, mftprop2.getCovariances()(4, 4));
      // MCH2 covariances
      CHECK_AND_FILL_FEATURE(cXXMCH2, mchprop2.getCovariances()(0, 0));
      CHECK_AND_FILL_FEATURE(cXYMCH2, mchprop2.getCovariances()(0, 1));
      CHECK_AND_FILL_FEATURE(cYYMCH2, mchprop2.getCovariances()(1, 1));
      CHECK_AND_FILL_FEATURE(cPhiXMCH2, mchprop2.getCovariances()(0, 2));
      CHECK_AND_FILL_FEATURE(cPhiYMCH2, mchprop2.getCovariances()(1, 2));
      CHECK_AND_FILL_FEATURE(cPhiPhiMCH2, mchprop2.getCovariances()(2, 2));
      CHECK_AND_FILL_FEATURE(cTglXMCH2, mchprop2.getCovariances()(0, 3));
      CHECK_AND_FILL_FEATURE(cTglYMCH2, mchprop2.getCovariances()(1, 3));
      CHECK_AND_FILL_FEATURE(cTglPhiMCH2, mchprop2.getCovariances()(2, 3));
      CHECK_AND_FILL_FEATURE(cTglTglMCH2, mchprop2.getCovariances()(3, 3));
      CHECK_AND_FILL_FEATURE(c1PtXMCH2, mchprop2.getCovariances()(0, 4));
      CHECK_AND_FILL_FEATURE(c1PtYMCH2, mchprop2.getCovariances()(1, 4));
      CHECK_AND_FILL_FEATURE(c1PtPhiMCH2, mchprop2.getCovariances()(2, 4));
      CHECK_AND_FILL_FEATURE(c1PtTglMCH2, mchprop2.getCovariances()(3, 4));
      CHECK_AND_FILL_FEATURE(c1Pt21Pt2MCH2, mchprop2.getCovariances()(4, 4));
      // Track residuals
      // CHECK_AND_FILL_FEATURE(sameSign, (mch.sign() == mft.sign()) ? 1 : 0);
      CHECK_AND_FILL_FEATURE(deltaX2, mchprop2.getX() - mftprop2.getX());
      CHECK_AND_FILL_FEATURE(deltaY2, mchprop2.getY() - mftprop2.getY());
      CHECK_AND_FILL_FEATURE(deltaPhi2, mchprop2.getPhi() - mftprop2.getPhi());
      CHECK_AND_FILL_FEATURE(deltaTgl2, mchprop2.getTgl() - mftprop2.getTgl());
      CHECK_AND_FILL_FEATURE(deltaEta2, mchprop2.getEta() - mftprop2.getEta());
      CHECK_AND_FILL_FEATURE(delta1Pt2, mchprop2.getInvQPt() - mftprop2.getInvQPt());
      CHECK_AND_FILL_FEATURE(deltaPt2, mchprop2.getPt() - mftprop2.getPt());
      CHECK_AND_FILL_FEATURE(deltaP2, mchprop2.getP() - mftprop2.getP());
      CHECK_AND_FILL_FEATURE(deltaR2, getDeltaR(mftprop2, mchprop2));
      CHECK_AND_FILL_FEATURE(deltaDirection2, getDeltaDirection(mftprop2, mchprop2));
      CHECK_AND_FILL_FEATURE(deltaPtRel2, getRelDifference(mftprop2.getPt(), mchprop2.getPt()));
      CHECK_AND_FILL_FEATURE(deltaPRel2, getRelDifference(mftprop2.getP(), mchprop2.getP()));
      CHECK_AND_FILL_FEATURE(pullX2, getPull(mftprop2.getX(), mftprop2.getCovariances()(0, 0), mchprop2.getX(), mchprop2.getCovariances()(0, 0)));
      CHECK_AND_FILL_FEATURE(pullY2, getPull(mftprop2.getY(), mftprop2.getCovariances()(1, 1), mchprop2.getY(), mchprop2.getCovariances()(1, 1)));
      CHECK_AND_FILL_FEATURE(pullPhi2, getPull(mftprop2.getPhi(), mftprop2.getCovariances()(2, 2), mchprop2.getPhi(), mchprop2.getCovariances()(2, 2)));
      CHECK_AND_FILL_FEATURE(pullTgl2, getPull(mftprop2.getTgl(), mftprop2.getCovariances()(3, 3), mchprop2.getTgl(), mchprop2.getCovariances()(3, 3)));
      // /*dummy value*/ CHECK_AND_FILL_FEATURE(pullEta2, 0);
      CHECK_AND_FILL_FEATURE(pullPt2, getPullPt(mftprop2, mchprop2));
      CHECK_AND_FILL_FEATURE(pullR2, getPullR(mftprop2, mchprop2));
      // Track residuals (absolute values)
      CHECK_AND_FILL_FEATURE(absDeltaX2, std::fabs(mchprop2.getX() - mftprop2.getX()));
      CHECK_AND_FILL_FEATURE(absDeltaY2, std::fabs(mchprop2.getY() - mftprop2.getY()));
      CHECK_AND_FILL_FEATURE(absDeltaPhi2, std::fabs(mchprop2.getPhi() - mftprop2.getPhi()));
      CHECK_AND_FILL_FEATURE(absDeltaTgl2, std::fabs(mchprop2.getTgl() - mftprop2.getTgl()));
      CHECK_AND_FILL_FEATURE(absDeltaEta2, std::fabs(mchprop2.getEta() - mftprop2.getEta()));
      CHECK_AND_FILL_FEATURE(absDeltaPt2, std::fabs(mchprop2.getPt() - mftprop2.getPt()));
      CHECK_AND_FILL_FEATURE(absDeltaP2, std::fabs(mchprop2.getP() - mftprop2.getP()));
      CHECK_AND_FILL_FEATURE(absDeltaR2, std::fabs(getDeltaR(mftprop2, mchprop2)));
      CHECK_AND_FILL_FEATURE(absDeltaDirection2, std::fabs(getDeltaDirection(mftprop2, mchprop2)));
      CHECK_AND_FILL_FEATURE(absDeltaPtRel2, std::fabs(getRelDifference(mftprop2.getPt(), mchprop2.getPt())));
      CHECK_AND_FILL_FEATURE(absPullX2, std::fabs(getPull(mftprop2.getX(), mftprop2.getCovariances()(0, 0), mchprop2.getX(), mchprop2.getCovariances()(0, 0))));
      CHECK_AND_FILL_FEATURE(absPullY2, std::fabs(getPull(mftprop2.getY(), mftprop2.getCovariances()(1, 1), mchprop2.getY(), mchprop2.getCovariances()(1, 1))));
      CHECK_AND_FILL_FEATURE(absPullPhi2, std::fabs(getPull(mftprop2.getPhi(), mftprop2.getCovariances()(2, 2), mchprop2.getPhi(), mchprop2.getCovariances()(2, 2))));
      CHECK_AND_FILL_FEATURE(absPullTgl2, std::fabs(getPull(mftprop2.getTgl(), mftprop2.getCovariances()(3, 3), mchprop2.getTgl(), mchprop2.getCovariances()(3, 3))));
      // /*dummy value*/ CHECK_AND_FILL_FEATURE(absPullEta2, 0);
      CHECK_AND_FILL_FEATURE(absPullPt2, std::fabs(getPullPt(mftprop2, mchprop2)));
      CHECK_AND_FILL_FEATURE(absPullR2, std::fabs(getPullR(mftprop2, mchprop2)));
      // primary vertex parameters
      // CHECK_AND_FILL_FEATURE(posX, collision.posX());
      // CHECK_AND_FILL_FEATURE(posY, collision.posY());
      // CHECK_AND_FILL_FEATURE(posZ, collision.posZ());
      // CHECK_AND_FILL_FEATURE_OPTIONAL(numContrib, collision, numContrib);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(trackOccupancyInTimeRange, collision, trackOccupancyInTimeRange);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(ft0cOccupancyInTimeRange, collision, ft0cOccupancyInTimeRange);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(multMFT2, collision, mftNtracks);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(multFT0A, collision, multFT0A);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(multFT0C, collision, multFT0C);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(multNTracksPV, collision, multNTracksPV);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(multNTracksPVeta1, collision, multNTracksPVeta1);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(multNTracksPVetaHalf, collision, multNTracksPVetaHalf);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(isInelGt0, collision, isInelGt0);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(isInelGt1, collision, isInelGt1);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(multFT0M, collision, multFT0M);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(centFT0M, collision, centFT0M);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(centFT0A, collision, centFT0A);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(centFT0C, collision, centFT0C);
      // global forward track parameters
      // CHECK_AND_FILL_FEATURE(chi2MCH2MFT2, muon.chi2MatchMCH2MFT2());
      // CHECK_AND_FILL_FEATURE(chi2GlobMUON, muon.chi2());
      // CHECK_AND_FILL_FEATURE_OPTIONAL(dcaX, muon, fwdDcaX);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(dcaY, muon, fwdDcaY);
      // CHECK_AND_FILL_FEATURE_OPTIONAL(isAmbig, muon, compatibleCollIds, (muon.compatibleCollIds().size() == 1) ? 0 : 1)

      ;
    }
    return inputFeature;
  }

  /// Method to get the input features vector needed for ML inference
  /// \param track is the single track, \param collision is the collision
  /// \return inputFeatures vector
  template <typename T1, typename T2, typename T3, typename T4, typename T5, typename U>
  std::vector<float> getInputFeatures(T1 const& muon, T2 const& mft, T3 const& mch, T4 const& mftprop, T5 const& mchprop, U const& collision)
  {
    std::vector<float> inputFeatures;
    for (const auto& idx : MlResponse<TypeOutputScore>::mCachedIndices) {
      float inputFeature = returnFeature(idx, muon, mft, mch, mftprop, mchprop, mftprop, mchprop, collision);
      inputFeatures.emplace_back(inputFeature);
    }
    return inputFeatures;
  }

  /// Method to get the input features vector needed for ML inference with 2 matching planes
  /// \param track is the single track, \param collision is the collision
  /// \return inputFeatures vector
  template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename U>
  std::vector<float> getInputFeatures(T1 const& muon, T2 const& mft, T3 const& mch, T4 const& mftprop, T5 const& mchprop, T6 const& mftprop2, T7 const& mchprop2, U const& collision)
  {
    std::vector<float> inputFeatures;
    for (const auto& idx : MlResponse<TypeOutputScore>::mCachedIndices) {
      float inputFeature = returnFeature(idx, muon, mft, mch, mftprop, mchprop, mftprop2, mchprop2, collision);
      inputFeatures.emplace_back(inputFeature);
    }
    return inputFeatures;
  }

  /// Method to get the value of variable chosen for binning
  /// \param track is the single track, \param collision is the collision
  /// \return binning variable
  template <typename T1, typename T2, typename C1, typename C2, typename U>
  float getBinningFeature(T1 const& muon, T2 const& mft, C1 const& muoncov, C2 const& mftcov, U const& collision)
  {
    return returnFeature(mCachedIndexBinning, muon, mft, muoncov, mftcov, collision);
  }

  void cacheBinningIndex(std::string const& cfgBinningFeature)
  {
    setAvailableInputFeatures();
    if (MlResponse<TypeOutputScore>::mAvailableInputFeatures.count(cfgBinningFeature)) {
      mCachedIndexBinning = MlResponse<TypeOutputScore>::mAvailableInputFeatures[cfgBinningFeature];
    } else {
      LOG(fatal) << "Binning feature " << cfgBinningFeature << " not available! Please check your configurables.";
    }
  }

 protected:
  /// Method to fill the map of available input features
  void setAvailableInputFeatures()
  {
    MlResponse<TypeOutputScore>::mAvailableInputFeatures = {
      // matching parameters
      FILL_MAP_MFTMUON_MATCH(zMatching),
      // MFT track parameters
      FILL_MAP_MFTMUON_MATCH(xMFT),
      FILL_MAP_MFTMUON_MATCH(yMFT),
      FILL_MAP_MFTMUON_MATCH(qOverptMFT),
      FILL_MAP_MFTMUON_MATCH(tglMFT),
      FILL_MAP_MFTMUON_MATCH(phiMFT),
      FILL_MAP_MFTMUON_MATCH(ptMFT),
      FILL_MAP_MFTMUON_MATCH(pMFT),
      FILL_MAP_MFTMUON_MATCH(etaMFT),
      FILL_MAP_MFTMUON_MATCH(timeMFT),
      FILL_MAP_MFTMUON_MATCH(timeResMFT),
      FILL_MAP_MFTMUON_MATCH(dcaXY),
      FILL_MAP_MFTMUON_MATCH(dcaZ),
      FILL_MAP_MFTMUON_MATCH(chi2MFT),
      FILL_MAP_MFTMUON_MATCH(clusterSizesAndTrackFlagsMFT),
      FILL_MAP_MFTMUON_MATCH(trackTypeMFT),
      FILL_MAP_MFTMUON_MATCH(nClustersMFT),
      // MCH track parameters
      FILL_MAP_MFTMUON_MATCH(xMCH),
      FILL_MAP_MFTMUON_MATCH(yMCH),
      FILL_MAP_MFTMUON_MATCH(qOverptMCH),
      FILL_MAP_MFTMUON_MATCH(tglMCH),
      FILL_MAP_MFTMUON_MATCH(phiMCH),
      FILL_MAP_MFTMUON_MATCH(nClustersMCH),
      FILL_MAP_MFTMUON_MATCH(ptMCH),
      FILL_MAP_MFTMUON_MATCH(pMCH),
      FILL_MAP_MFTMUON_MATCH(etaMCH),
      FILL_MAP_MFTMUON_MATCH(timeMCH),
      FILL_MAP_MFTMUON_MATCH(timeResMCH),
      FILL_MAP_MFTMUON_MATCH(chi2MCH),
      FILL_MAP_MFTMUON_MATCH(pdca),
      FILL_MAP_MFTMUON_MATCH(Rabs),
      // MFT covariances
      FILL_MAP_MFTMUON_MATCH(cXXMFT),
      FILL_MAP_MFTMUON_MATCH(cXYMFT),
      FILL_MAP_MFTMUON_MATCH(cYYMFT),
      FILL_MAP_MFTMUON_MATCH(cPhiYMFT),
      FILL_MAP_MFTMUON_MATCH(cPhiXMFT),
      FILL_MAP_MFTMUON_MATCH(cPhiPhiMFT),
      FILL_MAP_MFTMUON_MATCH(cTglYMFT),
      FILL_MAP_MFTMUON_MATCH(cTglXMFT),
      FILL_MAP_MFTMUON_MATCH(cTglPhiMFT),
      FILL_MAP_MFTMUON_MATCH(cTglTglMFT),
      FILL_MAP_MFTMUON_MATCH(c1PtYMFT),
      FILL_MAP_MFTMUON_MATCH(c1PtXMFT),
      FILL_MAP_MFTMUON_MATCH(c1PtPhiMFT),
      FILL_MAP_MFTMUON_MATCH(c1PtTglMFT),
      FILL_MAP_MFTMUON_MATCH(c1Pt21Pt2MFT),
      // MCH covariances
      FILL_MAP_MFTMUON_MATCH(cXXMCH),
      FILL_MAP_MFTMUON_MATCH(cXYMCH),
      FILL_MAP_MFTMUON_MATCH(cYYMCH),
      FILL_MAP_MFTMUON_MATCH(cPhiYMCH),
      FILL_MAP_MFTMUON_MATCH(cPhiXMCH),
      FILL_MAP_MFTMUON_MATCH(cPhiPhiMCH),
      FILL_MAP_MFTMUON_MATCH(cTglYMCH),
      FILL_MAP_MFTMUON_MATCH(cTglXMCH),
      FILL_MAP_MFTMUON_MATCH(cTglPhiMCH),
      FILL_MAP_MFTMUON_MATCH(cTglTglMCH),
      FILL_MAP_MFTMUON_MATCH(c1PtYMCH),
      FILL_MAP_MFTMUON_MATCH(c1PtXMCH),
      FILL_MAP_MFTMUON_MATCH(c1PtPhiMCH),
      FILL_MAP_MFTMUON_MATCH(c1PtTglMCH),
      FILL_MAP_MFTMUON_MATCH(c1Pt21Pt2MCH),
      // track residuals
      FILL_MAP_MFTMUON_MATCH(sameSign),
      FILL_MAP_MFTMUON_MATCH(deltaX),
      FILL_MAP_MFTMUON_MATCH(deltaY),
      FILL_MAP_MFTMUON_MATCH(deltaPhi),
      FILL_MAP_MFTMUON_MATCH(deltaTgl),
      FILL_MAP_MFTMUON_MATCH(deltaEta),
      FILL_MAP_MFTMUON_MATCH(deltaPt),
      FILL_MAP_MFTMUON_MATCH(delta1Pt),
      FILL_MAP_MFTMUON_MATCH(deltaP),
      FILL_MAP_MFTMUON_MATCH(deltaR),
      FILL_MAP_MFTMUON_MATCH(deltaDirection),
      FILL_MAP_MFTMUON_MATCH(pullX),
      FILL_MAP_MFTMUON_MATCH(pullY),
      FILL_MAP_MFTMUON_MATCH(pullPhi),
      FILL_MAP_MFTMUON_MATCH(pullTgl),
      FILL_MAP_MFTMUON_MATCH(pullEta),
      FILL_MAP_MFTMUON_MATCH(pullPt),
      FILL_MAP_MFTMUON_MATCH(pullR),
      FILL_MAP_MFTMUON_MATCH(deltaPtRel),
      FILL_MAP_MFTMUON_MATCH(deltaPRel),
      // track residuals (absolute values)
      FILL_MAP_MFTMUON_MATCH(absDeltaX),
      FILL_MAP_MFTMUON_MATCH(absDeltaY),
      FILL_MAP_MFTMUON_MATCH(absDeltaPhi),
      FILL_MAP_MFTMUON_MATCH(absDeltaTgl),
      FILL_MAP_MFTMUON_MATCH(absDeltaEta),
      FILL_MAP_MFTMUON_MATCH(absDeltaPt),
      FILL_MAP_MFTMUON_MATCH(absDeltaP),
      FILL_MAP_MFTMUON_MATCH(absDeltaR),
      FILL_MAP_MFTMUON_MATCH(absDeltaDirection),
      FILL_MAP_MFTMUON_MATCH(absPullX),
      FILL_MAP_MFTMUON_MATCH(absPullY),
      FILL_MAP_MFTMUON_MATCH(absPullPhi),
      FILL_MAP_MFTMUON_MATCH(absPullTgl),
      FILL_MAP_MFTMUON_MATCH(absPullEta),
      FILL_MAP_MFTMUON_MATCH(absPullPt),
      FILL_MAP_MFTMUON_MATCH(absPullR),
      FILL_MAP_MFTMUON_MATCH(absDeltaPtRel),
      // primary vertex parameters
      FILL_MAP_MFTMUON_MATCH(posX),
      FILL_MAP_MFTMUON_MATCH(posY),
      FILL_MAP_MFTMUON_MATCH(posZ),
      FILL_MAP_MFTMUON_MATCH(numContrib),
      FILL_MAP_MFTMUON_MATCH(trackOccupancyInTimeRange),
      FILL_MAP_MFTMUON_MATCH(ft0cOccupancyInTimeRange),
      FILL_MAP_MFTMUON_MATCH(multMFT),
      FILL_MAP_MFTMUON_MATCH(multFT0A),
      FILL_MAP_MFTMUON_MATCH(multFT0C),
      FILL_MAP_MFTMUON_MATCH(multNTracksPV),
      FILL_MAP_MFTMUON_MATCH(multNTracksPVeta1),
      FILL_MAP_MFTMUON_MATCH(multNTracksPVetaHalf),
      FILL_MAP_MFTMUON_MATCH(isInelGt0),
      FILL_MAP_MFTMUON_MATCH(isInelGt1),
      FILL_MAP_MFTMUON_MATCH(multFT0M),
      FILL_MAP_MFTMUON_MATCH(centFT0M),
      FILL_MAP_MFTMUON_MATCH(centFT0A),
      FILL_MAP_MFTMUON_MATCH(centFT0C),
      // global forward track parameters
      FILL_MAP_MFTMUON_MATCH(chi2MCHMFT),
      FILL_MAP_MFTMUON_MATCH(chi2GlobMUON),
      FILL_MAP_MFTMUON_MATCH(dcaX),
      FILL_MAP_MFTMUON_MATCH(dcaY),
      FILL_MAP_MFTMUON_MATCH(isAmbig),
      // Matching plane 2 parameters
      FILL_MAP_MFTMUON_MATCH(zMatching2),
      // MFT track parameters
      FILL_MAP_MFTMUON_MATCH(xMFT2),
      FILL_MAP_MFTMUON_MATCH(yMFT2),
      FILL_MAP_MFTMUON_MATCH(qOverptMFT2),
      FILL_MAP_MFTMUON_MATCH(tglMFT2),
      FILL_MAP_MFTMUON_MATCH(phiMFT2),
      FILL_MAP_MFTMUON_MATCH(ptMFT2),
      FILL_MAP_MFTMUON_MATCH(pMFT2),
      FILL_MAP_MFTMUON_MATCH(etaMFT2),
      // FILL_MAP_MFTMUON_MATCH(timeMFT),
      // FILL_MAP_MFTMUON_MATCH(timeResMFT),
      // FILL_MAP_MFTMUON_MATCH(dcaXY),
      // FILL_MAP_MFTMUON_MATCH(dcaZ),
      // FILL_MAP_MFTMUON_MATCH(chi2MFT),
      // FILL_MAP_MFTMUON_MATCH(clusterSizesAndTrackFlagsMFT),
      // FILL_MAP_MFTMUON_MATCH(trackTypeMFT),
      // FILL_MAP_MFTMUON_MATCH(nClustersMFT),
      // MCH track parameters
      FILL_MAP_MFTMUON_MATCH(xMCH2),
      FILL_MAP_MFTMUON_MATCH(yMCH2),
      FILL_MAP_MFTMUON_MATCH(qOverptMCH2),
      FILL_MAP_MFTMUON_MATCH(tglMCH2),
      FILL_MAP_MFTMUON_MATCH(phiMCH2),
      // FILL_MAP_MFTMUON_MATCH(nClustersMCH),
      FILL_MAP_MFTMUON_MATCH(ptMCH2),
      FILL_MAP_MFTMUON_MATCH(pMCH2),
      FILL_MAP_MFTMUON_MATCH(etaMCH2),
      // FILL_MAP_MFTMUON_MATCH(timeMCH),
      // FILL_MAP_MFTMUON_MATCH(timeResMCH),
      // FILL_MAP_MFTMUON_MATCH(chi2MCH),
      // FILL_MAP_MFTMUON_MATCH(pdca),
      // FILL_MAP_MFTMUON_MATCH(Rabs),
      // MFT covariances
      FILL_MAP_MFTMUON_MATCH(cXXMFT2),
      FILL_MAP_MFTMUON_MATCH(cXYMFT2),
      FILL_MAP_MFTMUON_MATCH(cYYMFT2),
      FILL_MAP_MFTMUON_MATCH(cPhiYMFT2),
      FILL_MAP_MFTMUON_MATCH(cPhiXMFT2),
      FILL_MAP_MFTMUON_MATCH(cPhiPhiMFT2),
      FILL_MAP_MFTMUON_MATCH(cTglYMFT2),
      FILL_MAP_MFTMUON_MATCH(cTglXMFT2),
      FILL_MAP_MFTMUON_MATCH(cTglPhiMFT2),
      FILL_MAP_MFTMUON_MATCH(cTglTglMFT2),
      FILL_MAP_MFTMUON_MATCH(c1PtYMFT2),
      FILL_MAP_MFTMUON_MATCH(c1PtXMFT2),
      FILL_MAP_MFTMUON_MATCH(c1PtPhiMFT2),
      FILL_MAP_MFTMUON_MATCH(c1PtTglMFT2),
      FILL_MAP_MFTMUON_MATCH(c1Pt21Pt2MFT2),
      // MCH covariances
      FILL_MAP_MFTMUON_MATCH(cXXMCH2),
      FILL_MAP_MFTMUON_MATCH(cXYMCH2),
      FILL_MAP_MFTMUON_MATCH(cYYMCH2),
      FILL_MAP_MFTMUON_MATCH(cPhiYMCH2),
      FILL_MAP_MFTMUON_MATCH(cPhiXMCH2),
      FILL_MAP_MFTMUON_MATCH(cPhiPhiMCH2),
      FILL_MAP_MFTMUON_MATCH(cTglYMCH2),
      FILL_MAP_MFTMUON_MATCH(cTglXMCH2),
      FILL_MAP_MFTMUON_MATCH(cTglPhiMCH2),
      FILL_MAP_MFTMUON_MATCH(cTglTglMCH2),
      FILL_MAP_MFTMUON_MATCH(c1PtYMCH2),
      FILL_MAP_MFTMUON_MATCH(c1PtXMCH2),
      FILL_MAP_MFTMUON_MATCH(c1PtPhiMCH2),
      FILL_MAP_MFTMUON_MATCH(c1PtTglMCH2),
      FILL_MAP_MFTMUON_MATCH(c1Pt21Pt2MCH2),
      // track residuals
      // FILL_MAP_MFTMUON_MATCH(sameSign),
      FILL_MAP_MFTMUON_MATCH(deltaX2),
      FILL_MAP_MFTMUON_MATCH(deltaY2),
      FILL_MAP_MFTMUON_MATCH(deltaPhi2),
      FILL_MAP_MFTMUON_MATCH(deltaTgl2),
      FILL_MAP_MFTMUON_MATCH(deltaEta2),
      FILL_MAP_MFTMUON_MATCH(deltaPt2),
      FILL_MAP_MFTMUON_MATCH(delta1Pt2),
      FILL_MAP_MFTMUON_MATCH(deltaP2),
      FILL_MAP_MFTMUON_MATCH(deltaR2),
      FILL_MAP_MFTMUON_MATCH(deltaDirection2),
      FILL_MAP_MFTMUON_MATCH(pullX2),
      FILL_MAP_MFTMUON_MATCH(pullY2),
      FILL_MAP_MFTMUON_MATCH(pullPhi2),
      FILL_MAP_MFTMUON_MATCH(pullTgl2),
      FILL_MAP_MFTMUON_MATCH(pullEta2),
      FILL_MAP_MFTMUON_MATCH(pullPt2),
      FILL_MAP_MFTMUON_MATCH(pullR2),
      FILL_MAP_MFTMUON_MATCH(deltaPtRel2),
      FILL_MAP_MFTMUON_MATCH(deltaPRel2),
      // track residuals (absolute values)
      FILL_MAP_MFTMUON_MATCH(absDeltaX2),
      FILL_MAP_MFTMUON_MATCH(absDeltaY2),
      FILL_MAP_MFTMUON_MATCH(absDeltaPhi2),
      FILL_MAP_MFTMUON_MATCH(absDeltaTgl2),
      FILL_MAP_MFTMUON_MATCH(absDeltaEta2),
      FILL_MAP_MFTMUON_MATCH(absDeltaPt2),
      FILL_MAP_MFTMUON_MATCH(absDeltaP2),
      FILL_MAP_MFTMUON_MATCH(absDeltaR2),
      FILL_MAP_MFTMUON_MATCH(absDeltaDirection2),
      FILL_MAP_MFTMUON_MATCH(absPullX2),
      FILL_MAP_MFTMUON_MATCH(absPullY2),
      FILL_MAP_MFTMUON_MATCH(absPullPhi2),
      FILL_MAP_MFTMUON_MATCH(absPullTgl2),
      FILL_MAP_MFTMUON_MATCH(absPullEta2),
      FILL_MAP_MFTMUON_MATCH(absPullPt2),
      FILL_MAP_MFTMUON_MATCH(absPullR2),
      FILL_MAP_MFTMUON_MATCH(absDeltaPtRel2),
      // primary vertex parameters
      // FILL_MAP_MFTMUON_MATCH(posX),
      // FILL_MAP_MFTMUON_MATCH(posY),
      // FILL_MAP_MFTMUON_MATCH(posZ),
      // FILL_MAP_MFTMUON_MATCH(numContrib),
      // FILL_MAP_MFTMUON_MATCH(trackOccupancyInTimeRange),
      // FILL_MAP_MFTMUON_MATCH(ft0cOccupancyInTimeRange),
      // FILL_MAP_MFTMUON_MATCH(multMFT),
      // FILL_MAP_MFTMUON_MATCH(multFT0A),
      // FILL_MAP_MFTMUON_MATCH(multFT0C),
      // FILL_MAP_MFTMUON_MATCH(multNTracksPV),
      // FILL_MAP_MFTMUON_MATCH(multNTracksPVeta1),
      // FILL_MAP_MFTMUON_MATCH(multNTracksPVetaHalf),
      // FILL_MAP_MFTMUON_MATCH(isInelGt0),
      // FILL_MAP_MFTMUON_MATCH(isInelGt1),
      // FILL_MAP_MFTMUON_MATCH(multFT0M),
      // FILL_MAP_MFTMUON_MATCH(centFT0M),
      // FILL_MAP_MFTMUON_MATCH(centFT0A),
      // FILL_MAP_MFTMUON_MATCH(centFT0C),
      // // global forward track parameters
      // FILL_MAP_MFTMUON_MATCH(chi2MCHMFT),
      // FILL_MAP_MFTMUON_MATCH(chi2GlobMUON),
      // FILL_MAP_MFTMUON_MATCH(dcaX),
      // FILL_MAP_MFTMUON_MATCH(dcaY),
      // FILL_MAP_MFTMUON_MATCH(isAmbig)
    };
  }

  uint8_t mCachedIndexBinning; // index correspondance between configurable and available input features
};

} // namespace o2::analysis

#undef FILL_MAP_MFTMUON_MAP
#undef CHECK_AND_FILL_MUON_TRACK
#undef CHECK_AND_FILL_MFT_TRACK
#undef CHECK_AND_FILL_MUON_COV
#undef CHECK_AND_FILL_MFT_COV
#undef CHECK_AND_FILL_MFTMUON_DIFF
#undef CHECK_AND_FILL_MFTMUON_COLLISION

#endif // PWGDQ_CORE_MUONMATCHINGMLRESPONSE_H_
