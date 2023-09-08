#pragma once
#include <sys/wait.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <typeinfo>
#include <vector>

#define HIGH 1
#define LOW 0
#define PUD_UP 0
#define PUD_DOWN 1

extern const std::string varGpioRoot;

extern const std::map<std::string, std::string> staticModuleMap;

extern const std::map<std::string, std::string> rpiInfo;

extern const std::vector<int> rock64ValidChannels;

extern const std::vector<int> boardToRock64;

extern const std::vector<int> bcmToRock64;

extern const std::vector<int> rockPro64ValidChannels;

extern const std::vector<int> boardToRockPro64;

extern const std::vector<int> bcmToRockPro64;

extern std::string gpioMode;

extern std::string rockMode;

extern int warningMode;

class PWM {
 private:
  int gpio;
  int state;
  int precision;
  double freq;
  double sleepLow;
  double sleepHigh;
  pid_t pid;
  void pwmCalc(void);
  static void pwmProcess(int channel, double sleepHigh, int precision = HIGH);

 public:
  PWM(int channel, double frequency);
  void setFrequency(double frequency);
  void start(double dutyCycle, int pwmPrecision = HIGH);
  void stop(void);
  static void pwmBussyWait(double waitTime);
  void changeFrequency(double frequency);
  void changeDutyCycle(double dutyCycle);
};

void setMode(std::string mode);

void setRock(std::string rock);

std::string getMode(void);

int getGpioNumber(int channel);

std::string gpioFunction(int channel);

void setWarning(int state = 1);

int validateDirection(int channel, std::string validationType = "both");

void output(int channel, std::vector<int> value);

// void setup(int channel, std::string direction, int pull_up_down = PUD_DOWN,
//            int initial = LOW);

std::string input(int channel);

void setup(int channel, std::string direction,
           std::string pull_up_down = "PUD_DOWN", std::string initial = "LOW");

bool wait_for_edge(int channel, std::string edge,
                   std::string bouncetime = "none",
                   std::string timeout = "none");
