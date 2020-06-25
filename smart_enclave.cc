/*
 *
 * Copyright 2018 Asylo authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <cstdint>

#include "absl/strings/str_cat.h"
#include "asylo/trusted_application.h"
#include "asylo/util/logging.h"
#include "asylo/util/status.h"
#include "smart_meter/smart.pb.h"

class HelloApplication : public asylo::TrustedApplication
{
public:
  HelloApplication() : counter(0.0) {}

  asylo::Status Run(const asylo::EnclaveInput &input,
                    asylo::EnclaveOutput *output) override
  {
    LOG(INFO) << "Inside enclave";
    if (!input.HasExtension(smart_meter::enclave_input_smart))
    {
      return asylo::Status(asylo::error::GoogleError::INVALID_ARGUMENT,
                           "Expected a SmartInput extension on input.");
    }

    auto kwMinutes = input.GetExtension(smart_meter::enclave_input_smart).kw_minute();
    int kwArraySize = input.GetExtension(smart_meter::enclave_input_smart).kw_minute_size();
    smart_meter::SmartInput_Period period = input.GetExtension(smart_meter::enclave_input_smart).period();
    float kwMinutesArray[kwArraySize];
    int i = 0;
    for (const auto kwMinute : kwMinutes)
    {
      kwMinutesArray[i++] = kwMinute;
      LOG(INFO)
          << "Input usage cycle in kwMinute " << kwMinute;
    }
    LOG(INFO)
        << "Input period " << period;
    this->meterUsageCycle(kwMinutesArray, kwArraySize, period);
    if (output)
    {
      output->MutableExtension(smart_meter::enclave_output_smart)
          ->set_counter(this->counter);
    }
    return asylo::Status::OkStatus();
  }

private:
  float counter;

  void increaseCounter(float usage)
  {
    if (usage > 0)
    {
      this->counter += usage;
    }
  }
  float computeUsage(float *kwMinute, int size, smart_meter::SmartInput_Period rate)
  {
    float usage = 0;
    for (int i = 0; i < size; i++)
    {
      float rating = 1.0;
      if (rate == smart_meter::SmartInput_Period::SmartInput_Period_Peak)
      {
        rating = 2.0;
      }
      else if (rate == smart_meter::SmartInput_Period::SmartInput_Period_OffPeak)
      {
        rating = 0.5;
      }
      usage += kwMinute[i] * rating;
    }
    return usage;
  }
  void meterUsageCycle(float *kwMinute, int size, smart_meter::SmartInput_Period rate)
  {
    float kwHour = 0;
    // LOG(INFO) << "kwMin:" << kwMinute << " size:" << size << " period:" << rate;
    kwHour = computeUsage(kwMinute, size, rate);
    // LOG(INFO) << "kwHour:" << kwHour;
    this->increaseCounter(kwHour);
  }
};

namespace asylo
{

  TrustedApplication *BuildTrustedApplication() { return new HelloApplication; }

} // namespace asylo
