# Port of the smart meter program (class) to Asylo framework (asylo-smart-meter)
This repository provides a sample solution for `exercise 6.2` of the Security Engineering course (summer term 2020) at the Technical University of Munich.

# Smart Meter Class
```
class SmartMeter
{
private:
    float counter;

    void increaseCounter(float usage)
    {
        if (usage > 0)
        {
            this->counter += usage;
        }
    }

public:
    enum period
    {
        Peak,
        OffPeak,
        Normal
    };

    float computeUsage(float *kwMinute, int size, enum period rate)
    {
        float usage = 0;
        for (int i = 0; i < size; i++)
        {
            float rating = 1.0;
            if (rate == Peak)
            {
                rating = 2.0;
            }
            else if (rate == OffPeak)
            {
                rating = 0.5;
            }
            usage += kwMinute[i] * rating;
        }
        return usage;
    }
    void meterUsageCycle(float *kwMinute, int size, enum period rate)
    {
        float kwHour = 0;
        kwHour = computeUsage(kwMinute, size, rate);
        this->increaseCounter(kwHour);
    }

    float getCounter()
    {
        return this->counter;
    }
};
```
# Execution 
- Setup Asylo as instructed in https://asylo.dev/docs/guides/quickstart.html 
- Clone this repo into your Asylo project path `${MY_PROJECT}`
- Compile and run the project by:
```
 docker run -it --rm     -v bazel-cache:/root/.cache/bazel     -v "${MY_PROJECT}":/opt/my-project     -w /opt/my-project     gcr.io/asylo-framework/asylo     bazel run --config=sgx-sim //smart_meter -- --usages="120,140" --period="peak"

```
## Arguments
`--usages` comma-separated list of float kwMinute usages (required argument)
`--period` usage period: peak, offpeak, normal (optional argument; the default value is normal)
