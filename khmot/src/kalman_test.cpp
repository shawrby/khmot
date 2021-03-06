#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do
                           // this in one cpp file
#include "kalman.hpp"

#include "catch.hpp"
#include <cmath>

namespace khmot {

TEST_CASE("Test constant velocity movement along X", "[kalman]")
{
  const int total_steps = 100;
  const int corr_steps = 50;
  const double dx = 1.0;
  const double deviation = 1.0;
  const double total_dist = total_steps * dx;
  const double big_covariance = 5000.0;

  Kalman k;
  KalmanObservation obs;
  obs.state = Eigen::VectorXd::Zero(STATE_SIZE);
  obs.covariance = Eigen::MatrixXd::Identity(STATE_SIZE, STATE_SIZE) *
                   (deviation * deviation);
  obs.timestamp = 0.0;

  // execute predict-correct cycles
  for (int i = 0; i < total_steps; ++i) {
    obs.state(StateMemberX) += dx;
    k.predict(static_cast<double>(i));
    if (i < corr_steps) {
      k.correct(obs);  // do not correct after some time
    }
  }

  // check state is expected
  CHECK(k.state()(StateMemberX) == Approx(total_dist));
  CHECK(k.state()(StateMemberY) == Approx(0.0));
  CHECK(k.state()(StateMemberYaw) == Approx(0.0));
  CHECK(k.state()(StateMemberVx) == Approx(dx));
  CHECK(k.state()(StateMemberVy) == Approx(0.0));
  CHECK(k.state()(StateMemberVyaw) == Approx(0.0));

  // check covariance for X blowed up
  CHECK(k.covariance()(StateMemberX, StateMemberX) > big_covariance);
}

TEST_CASE("Test non-omnidirectional case movement along Y", "[kalman]")
{
  const int total_steps = 100;
  const int corr_steps = 50;
  const double dy = 1.0;
  const double deviation = 1.0;
  bool isOmnidirectional = false;

  Kalman k(isOmnidirectional);
  KalmanObservation obs;
  obs.state = Eigen::VectorXd::Zero(STATE_SIZE);
  obs.covariance = Eigen::MatrixXd::Identity(STATE_SIZE, STATE_SIZE) *
                   (deviation * deviation);
  obs.timestamp = 0.0;

  // execute predict-correct cycles
  for (int i = 0; i < total_steps; ++i) {
    obs.state(StateMemberY) += dy;
    k.predict(static_cast<double>(i));
    if (i < corr_steps) {
      k.correct(obs);  // do not correct after some time
    }
  }

  // check that last position is last same as in last correction
  CHECK(k.state()(StateMemberY) == Approx(corr_steps - 1));
  // check that velocity Y is zero
  CHECK(k.state()(StateMemberVy) == Approx(0.0));
}

TEST_CASE("Test bad input with zero covariance", "[kalman]")
{
  Kalman k;
  KalmanObservation obs;
  // set covariance to zero
  obs.covariance = Eigen::MatrixXd::Zero(STATE_SIZE, STATE_SIZE);

  k.correct(obs);
  k.correct(obs);

  // check that is normal number (not NaN)
  CHECK(std::isfinite(k.state()(StateMemberX)));
}

}  // namespace khmot
