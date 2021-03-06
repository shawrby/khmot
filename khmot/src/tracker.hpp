#pragma once
#include "Hungarian.h"
#include "kalman.hpp"

#include <cmath>
#include <limits>
#include <memory>
#include <vector>

namespace khmot {

const double defaultDimsFilterAlpha = 0.1;
const double defaultTrackTimeout = 10.0;
const double defaultMahalanobisThresh = 3.0;
const int defaultProbLeft = 5;

using TrackID = unsigned int;
constexpr TrackID maxTrackID = std::numeric_limits<TrackID>::max();

struct Dims {
  double h = 0;
  double w = 0;
  double l = 0;
};

struct Observation {
  KalmanObservation kalmanObs;
  Dims dims;
};

struct Track {
  TrackID trackID;
  Kalman KF;
  Dims dims;
  int probLeft;
  bool valid;

  Track(TrackID i, bool omnidirectional = true)
      : trackID(i), KF(omnidirectional), probLeft(defaultProbLeft), valid(false)
  {
  }
};

inline TrackID genTrackID(TrackID cur)
{
  return (cur == maxTrackID) ? 0 : ++cur;
}

// Filters dimentions with Exponential Moving Average
inline double filterEMA(double x, double avg, double alpha)
{
  return alpha * x + (1 - alpha) * avg;
}

class Tracker {
 public:
  Tracker(double dimsFilterAlpha = defaultDimsFilterAlpha,
          double trackTimeout = defaultTrackTimeout,
          double mahalanobisThresh = defaultMahalanobisThresh);
  Tracker(const Tracker&) = delete;             // delete copy ctor
  Tracker& operator=(Tracker const&) = delete;  // delete assign ctor

  void update(const std::vector<Observation>& obsArr, const double timestamp);
  const auto& tracks() const { return tracks_; };

  // getters, setters
  double getDimsFilterAlpha() const { return dimsFilterAlpha_; };
  double getMahalanobisThresh() const { return mahalanobisThresh_; };
  double getTrackTimeout() const { return trackTimeout_; };
  void setDimsFilterAlpha(double v) { dimsFilterAlpha_ = v; };
  void setMahalanobisThresh(double v) { mahalanobisThresh_ = v; };
  void setTrackTimeout(double v) { trackTimeout_ = v; };

 private:
  TrackID curTrackID_;
  double dimsFilterAlpha_;  // alpha coeff used filterEMA
  double mahalanobisThresh_;
  double trackTimeout_;
  std::vector<std::unique_ptr<Track>> tracks_;

  void GC(const double timestamp);
  inline TrackID newTrackID() { return curTrackID_ = genTrackID(curTrackID_); }
};

}  // namespace khmot
