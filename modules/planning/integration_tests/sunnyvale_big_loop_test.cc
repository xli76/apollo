/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include <string>

#include "gtest/gtest.h"

#include "modules/common/configs/config_gflags.h"
#include "modules/common/time/time.h"
#include "modules/common/util/dropbox.h"
#include "modules/map/hdmap/hdmap_util.h"
#include "modules/planning/common/planning_gflags.h"
#include "modules/planning/integration_tests/planning_test_base.h"
#include "modules/planning/planning.h"
#include "modules/planning/tasks/traffic_decider/stop_sign.h"

namespace apollo {
namespace planning {

using apollo::common::time::Clock;
using apollo::common::util::Dropbox;
using apollo::planning::StopSign;

/**
 * @class SunnyvaleBigLoopTest
 * @brief This is an integration test that uses the sunnyvale_big_loop map.
 */

class SunnyvaleBigLoopTest : public PlanningTestBase {
 public:
  virtual void SetUp() {
    FLAGS_use_navigation_mode = false;
    FLAGS_map_dir = "modules/map/data/sunnyvale_big_loop";
    FLAGS_test_base_map_filename = "base_map.bin";
    FLAGS_test_data_dir = "modules/planning/testdata/sunnyvale_big_loop_test";
    FLAGS_planning_upper_speed_limit = 12.5;

    FLAGS_enable_stop_sign = false;
    FLAGS_enable_crosswalk = false;
    FLAGS_enable_keep_clear = false;
  }
};

/*
 * stop_sign: adc proceed
 *   adc status: null => TO_STOP
 *   decision: STOP
 */
TEST_F(SunnyvaleBigLoopTest, stop_sign_01) {
  FLAGS_enable_stop_sign = true;
  std::string seq_num = "1";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);

  // check dropbox value
  StopSign::StopSignStopStatus* status =
      Dropbox<StopSign::StopSignStopStatus>::Open()->Get(
          "kStopSignStopStatus_1017");
  StopSign::StopSignStopStatus stop_status =
      (status == nullptr) ? StopSign::StopSignStopStatus::UNKNOWN : *status;
  EXPECT_EQ(StopSign::StopSignStopStatus::TO_STOP, stop_status);
}

/*
 * stop_sign: adc stopped (speed and distance to stop_line)
 *   adc status: TO_STOP => STOPPING
 *   decision: STOP
 */
TEST_F(SunnyvaleBigLoopTest, stop_sign_02) {
  FLAGS_enable_stop_sign = true;

  // set dropbox
  Dropbox<StopSign::StopSignStopStatus>::Open()->Set(
      "kStopSignStopStatus_1017", StopSign::StopSignStopStatus::TO_STOP);

  std::string seq_num = "2";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);

  // check dropbox value
  StopSign::StopSignStopStatus* status =
      Dropbox<StopSign::StopSignStopStatus>::Open()->Get(
          "kStopSignStopStatus_1017");
  StopSign::StopSignStopStatus stop_status =
      (status == nullptr) ? StopSign::StopSignStopStatus::UNKNOWN : *status;
  EXPECT_EQ(StopSign::StopSignStopStatus::STOPPING, stop_status);
}

/*
 * stop_sign: adc stopped + wait_time < 3sec
 *   adc status: STOPPING => STOPPING
 *   decision: STOP
 */
TEST_F(SunnyvaleBigLoopTest, stop_sign_03) {
  FLAGS_enable_stop_sign = true;

  // set dropbox
  Dropbox<StopSign::StopSignStopStatus>::Open()->Set(
      "kStopSignStopStatus_1017", StopSign::StopSignStopStatus::STOPPING);
  double stop_start_time = Clock::NowInSeconds() - 2;
  Dropbox<double>::Open()->Set("kStopSignStopStarttime_1017", stop_start_time);
  std::string seq_num = "2";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);

  // check dropbox value
  StopSign::StopSignStopStatus* status =
      Dropbox<StopSign::StopSignStopStatus>::Open()->Get(
          "kStopSignStopStatus_1017");
  StopSign::StopSignStopStatus stop_status =
      (status == nullptr) ? StopSign::StopSignStopStatus::UNKNOWN : *status;
  EXPECT_EQ(StopSign::StopSignStopStatus::STOPPING, stop_status);
}

/*
 * stop_sign: adc stopped + wait time > 3
 *   adc status: STOPPING => STOP_DONE
 *   decision: CRUISE
 */
TEST_F(SunnyvaleBigLoopTest, stop_sign_04) {
  FLAGS_enable_stop_sign = true;

  // set dropbox
  Dropbox<StopSign::StopSignStopStatus>::Open()->Set(
      "kStopSignStopStatus_1017", StopSign::StopSignStopStatus::STOPPING);
  double stop_start_time = Clock::NowInSeconds() - 4;
  Dropbox<double>::Open()->Set("kStopSignStopStarttime_1017", stop_start_time);
  std::string seq_num = "2";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);

  // check dropbox value
  StopSign::StopSignStopStatus* status =
      Dropbox<StopSign::StopSignStopStatus>::Open()->Get(
          "kStopSignStopStatus_1017");
  StopSign::StopSignStopStatus stop_status =
      (status == nullptr) ? StopSign::StopSignStopStatus::UNKNOWN : *status;
  EXPECT_EQ(StopSign::StopSignStopStatus::STOP_DONE, stop_status);
}

/*
 * stop_sign:
 * bag: 2018-01-24-11-32-28/2018-01-24-11-32-30_0.bag
 * step 1:
 *   adc decision: STOP
 * step 2:
 *   wait_time = 4, other vehicles arrived at other stop sign later than adc
 *   adc status: STOPPING => STOP_DONE
 *   decision: CRUISE
 */
TEST_F(SunnyvaleBigLoopTest, stop_sign_05) {
  FLAGS_enable_stop_sign = true;

  std::string seq_num = "3";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);

  // set dropbox
  double stop_start_time = Clock::NowInSeconds() - 4;
  Dropbox<double>::Open()->Set("kStopSignStopStarttime_9762", stop_start_time);

  seq_num = "4";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(1);
}

/*
 * stop_sign:
 * bag: 2018-01-24-11-36-55/2018-01-24-11-36-55
 * step 1:
 *   adc decision: STOP
 * step 2:
 *   wait_time = 4, other vehicles arrived at other stop sign earlier than adc
 *   adc status: STOPPING => STOPPING (i.e. waiting)
 *   decision: STOP
 * step 3:
 *   wait_time = 4,
 *     and other vehicles arrived at other stop sign earlier than adc GONE
 *   adc status: STOPPING => STOPPING => STOP_DONE
 *   decision: CRUISE
 */
TEST_F(SunnyvaleBigLoopTest, stop_sign_06) {
  FLAGS_enable_stop_sign = true;

  std::string seq_num = "5";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);

  // step 2:
  // wait time is enough
  // but vehicles are still there (use the same data as previous test)

  // set dropbox
  double stop_start_time = Clock::NowInSeconds() - 4;
  Dropbox<double>::Open()->Set("kStopSignStopStarttime_1022", stop_start_time);

  seq_num = "6";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(1);

  // check dropbox value on watch vehicles
  // waiting for vehicle 4059 on lane 868_1_-1
  std::string db_key_watch_vehicle = "kStopSignWatchVehicle_868_1_-1";
  std::vector<std::string>* value =
      Dropbox<std::vector<std::string>>::Open()->Get(db_key_watch_vehicle);
  EXPECT_TRUE(value != nullptr && (*value)[0] == "4059");

  // step 3:
  // wait time is enough
  // previously watch vehicles are gone

  // set dropbox
  stop_start_time = Clock::NowInSeconds() - 4;
  Dropbox<double>::Open()->Set("kStopSignStopStarttime_1022", stop_start_time);

  seq_num = "7";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(2);
}

/*
 * stop_sign:
 * bag:
 *    step 1/3: 22018-02-15-16-37-45/2018-02-15-16-40-46_3.bag
 *    step2:    22018-02-15-16-37-45/2018-02-15-16-41-46_4.bag
 * step 1:
 *   adc decision: STOP
 * step 2:
 *   pass stop sign
 * step 3:
 *   come back to the same stop sign 2nd time
 *   adc decision: STOP
 */
TEST_F(SunnyvaleBigLoopTest, stop_sign_07) {
  FLAGS_enable_stop_sign = true;

  std::string seq_num = "12";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);

  // check dropbox value to make sure they are set
  std::string db_key_stop_status = "kStopSignStopStatus_9762";
  StopSign::StopSignStopStatus* status =
      Dropbox<StopSign::StopSignStopStatus>::Open()->Get(db_key_stop_status);
  EXPECT_TRUE(status != nullptr);

  std::string db_key_stop_starttime = "kStopSignStopStarttime_9762";
  double* start_time = Dropbox<double>::Open()->Get(db_key_stop_starttime);
  EXPECT_TRUE(start_time != nullptr);

  std::string db_key_associated_lanes = "kStopSignAssociateLane_9762";
  std::vector<std::string>* associated_lanes =
      Dropbox<std::vector<std::string>>::Open()->Get(db_key_associated_lanes);
  EXPECT_TRUE(associated_lanes != nullptr);

  // step 2: pass stop sign
  seq_num = "13";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(1);

  // check dropbox value to make sure everything is cleared for that stop sign
  db_key_stop_status = "kStopSignStopStatus_9762";
  status = Dropbox<StopSign::StopSignStopStatus>::Open()->Get(
      db_key_stop_status);
  EXPECT_TRUE(status == nullptr);

  db_key_stop_starttime = "kStopSignStopStarttime_9762";
  start_time = Dropbox<double>::Open()->Get(db_key_stop_starttime);
  EXPECT_TRUE(start_time == nullptr);

  std::string db_key_watch_vehicle = "kStopSignWatchVehicle_743_1_-2";
  std::vector<std::string>* value =
      Dropbox<std::vector<std::string>>::Open()->Get(db_key_watch_vehicle);
  EXPECT_TRUE(value == nullptr);
  db_key_watch_vehicle = "kStopSignWatchVehicle_743_1_-1";
  value = Dropbox<std::vector<std::string>>::Open()->Get(db_key_watch_vehicle);
  EXPECT_TRUE(value == nullptr);
  db_key_watch_vehicle = "kStopSignWatchVehicle_868_1_-1";
  value = Dropbox<std::vector<std::string>>::Open()->Get(db_key_watch_vehicle);
  EXPECT_TRUE(value == nullptr);

  db_key_associated_lanes = "kStopSignAssociateLane_9762";
  associated_lanes =
      Dropbox<std::vector<std::string>>::Open()->Get(db_key_associated_lanes);
  EXPECT_TRUE(associated_lanes == nullptr);

  // step 3: 2nd round
  seq_num = "12";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(2);
}

/*
 * crosswalk: pedestrian on crosswalk
 * bag: 2018-01-29-17-22-46/2018-01-29-17-31-47_9.bag
 * decision: STOP
 */
TEST_F(SunnyvaleBigLoopTest, crosswalk_01) {
  FLAGS_enable_crosswalk = true;
  FLAGS_enable_traffic_light = false;

  std::string seq_num = "8";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);
  FLAGS_enable_traffic_light = true;
}

/*
 * kee_clear: not blocking, KEEP_CLEAR static obstacle built
 * bag: 2018-01-29-17-22-46/2018-01-29-17-22-47_0.bag
 * decision: CRUISE
 */
TEST_F(SunnyvaleBigLoopTest, keep_clear_01) {
  FLAGS_enable_keep_clear = true;
  FLAGS_enable_traffic_light = false;

  std::string seq_num = "9";
  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);
}

TEST_F(SunnyvaleBigLoopTest, traffic_light_green) {
  std::string seq_num = "10";
  FLAGS_enable_traffic_light = true;
  FLAGS_enable_prediction = false;
  FLAGS_enable_keep_clear = false;

  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  FLAGS_test_traffic_light_file = seq_num + "_traffic_light.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);
  FLAGS_enable_prediction = true;
}

TEST_F(SunnyvaleBigLoopTest, abort_change_lane_for_fast_back_vehicle) {
  std::string seq_num = "11";
  FLAGS_enable_traffic_light = true;
  FLAGS_enable_keep_clear = false;

  FLAGS_test_routing_response_file = seq_num + "_routing.pb.txt";
  FLAGS_test_localization_file = seq_num + "_localization.pb.txt";
  FLAGS_test_chassis_file = seq_num + "_chassis.pb.txt";
  FLAGS_test_prediction_file = seq_num + "_prediction.pb.txt";
  PlanningTestBase::SetUp();
  RUN_GOLDEN_TEST(0);
}

}  // namespace planning
}  // namespace apollo

TMAIN;
