/* Copyright (c) 2017, United States Government, as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * 
 * All rights reserved.
 * 
 * The Astrobee platform is licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License. You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "dds_ros_bridge/ros_access_control.h"

ff::RosAccessControlStateToRapid::RosAccessControlStateToRapid(
                                            const std::string& subscribe_topic,
                                            const std::string& pub_topic,
                                            const ros::NodeHandle &nh,
                                            const unsigned int queue_size)
  : RosSubRapidPub(subscribe_topic, pub_topic, nh, queue_size) {
  state_supplier_.reset(new ff::RosAccessControlStateToRapid::StateSupplier(
                                  rapid::ACCESSCONTROL_STATE_TOPIC + pub_topic,
                                  "", "RapidAccessControlStateProfile", ""));

  sub_ = nh_.subscribe(subscribe_topic,
                       queue_size,
                       &RosAccessControlStateToRapid::Callback,
                       this);

  rapid::RapidHelper::initHeader(state_supplier_->event().hdr);
}

void ff::RosAccessControlStateToRapid::Callback(
    const ff_msgs::AccessControlStateStamped::ConstPtr& acs) {

  rapid::AccessControlState &msg = state_supplier_->event();
  msg.hdr.timeStamp = util::RosTime2RapidTime(acs->header.stamp);

  std::strncpy(msg.controller, acs->controller.data(), 32);

  msg.requestors.length(1);
  std::strncpy(msg.requestors[0], acs->cookie.data(), 32);

  state_supplier_->sendEvent();
}

