#include <libgpio/funcs.hpp>

const std::string varGpioRoot = "/sys/class/gpio";

const std::map<std::string, std::string> staticModuleMap = {
    {"ROCK", "ROCK"},
    {"BOARD", "BOARD"},
    {"BCM", "BCM"},
    {"OUT", "out"},
    {"IN", "in"},
    {"RISING", "rising"},
    {"FALLING", "falling"},
    {"BOTH", "both"},
    {"VERSION", "0.6.3"}};

const std::map<std::string, std::string> rpiInfo = {
    {"PI_VERSION", "3"},
    {"RAM", "1024M"},
    {"REVISION", "a22082"},
    {"TYPE", "Pi 3 Model B"},
    {"PROCESSOR", "BCM2837"},
    {"MANUFACTURES", "Embest"}};

const std::vector<int> rock64ValidChannels = {
    27, 32, 33, 34, 35, 36, 37, 38, 60, 64, 65, 67, 68,  69,  76,  79,  80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 96, 97, 98, 100, 101, 102, 103, 104};

const std::vector<int> boardToRock64 = {
    0,   0,  0,   89, 0,  88, 0,   60, 64,  0, 65, 0,  67, 0,  0,  100,
    101, 0,  102, 97, 0,  98, 103, 96, 104, 0, 76, 68, 69, 0,  0,  0,
    38,  32, 0,   33, 37, 34, 36,  0,  35,  0, 0,  81, 82, 87, 83, 0,
    0,   80, 79,  85, 84, 27, 86,  0,  0,   0, 0,  0,  0,  89, 88};

const std::vector<int> bcmToRock64 = {
    68, 69, 89, 88, 81, 87, 83, 76, 104, 98,  97,  96,  38, 32,
    64, 65, 37, 80, 67, 33, 36, 35, 100, 101, 102, 103, 34, 82};

const std::vector<int> rockPro64ValidChannels = {
    52,  53,  152, 54,  50, 33,  48,  39, 41, 43, 155, 156, 125, 122,
    121, 148, 147, 120, 36, 149, 153, 42, 45, 44, 124, 126, 123, 127};

const std::vector<int> boardToRockPro64 = {
    0,  0,   0,  52,  0,   53,  0, 152, 148, 0,   147, 54, 120, 50,
    0,  33,  36, 0,   149, 48,  0, 39,  153, 41,  42,  0,  45,  43,
    44, 155, 0,  156, 124, 125, 0, 122, 126, 121, 123, 0,  127};

const std::vector<int> bcmToRockPro64 = {
    43,  44,  52,  53, 152, 155, 156, 45,  42, 39, 48,  41,  124, 125,
    148, 147, 124, 54, 120, 122, 123, 127, 33, 36, 149, 153, 121, 50};

std::string gpioMode = NULL;

std::string rockMode = "ROCK64";

int warningMode = 1;

void setMode(std::string mode) {
  int check = 0;

  for (auto iter : staticModuleMap) {
    if (mode == iter.second) {
      gpioMode = mode;
      check = 1;
    }
  }

  if (!check) {
    std::cout << "An invalid mode was passed to setmode(). Use one of the "
                 "following: ROCK, BOARD, BCM"
              << '\n';
  }
}

void setRock(std::string rock) {
  if (rock == "ROCK64" || rock == "ROCKPRO64") {
    rockMode = rock;
  } else {
    std::cout << "An invalid rock-mode was passed to setrock(). Use one "
                 "of the following: ROCK64, ROCKPRO64";
  }
}

std::string getMode(void) {
  for (auto iter : staticModuleMap) {
    if (gpioMode == iter.second) {
      return gpioMode;
    }
  }

  std::cout << "Error: An invalid mode is currently set";
  return NULL;
}

int getGpioNumber(int channel) {
  if (gpioMode == "ROCK" || gpioMode == "BOARD" || gpioMode == "BCM") {
    int channelNew = 0;

    if (gpioMode == "BOARD") {
      if (rockMode == "ROCK64") {
        channelNew = boardToRock64[channel];
      } else if (rockMode == "ROCKPRO64") {
        channelNew = boardToRockPro64[channel];
      }
    }
    if (gpioMode == "BCM") {
      if (rockMode == "ROCK64") {
        channelNew = bcmToRock64[channel];
      } else if (rockMode == "ROCKPRO64") {
        channelNew = bcmToRockPro64[channel];
      }
    }
    if (gpioMode == "ROCK") {
      channelNew = channel;
    }
    if (rockMode == "ROCK64") {
      for (std::vector<int>::const_iterator iter = rock64ValidChannels.begin();
           iter != rock64ValidChannels.end();
           iter++) {
        if (*iter == channelNew) {
          return channelNew;
        }
      }
    } else if (rockMode == "ROCKPRO64") {
      for (std::vector<int>::const_iterator iter =
               rockPro64ValidChannels.begin();
           iter != rockPro64ValidChannels.end();
           iter++) {
        if (*iter == channelNew) {
          return channelNew;
        }
      }
    } else {
      std::cout << "Error: GPIO not supported on {0} {1}";
      return -1;
    }
  } else {
    std::cout << "RuntimeError: Please set pin numbering mode using "
                 "GPIO.setmode(GPIO.ROCK), GPIO.setmode(GPIO.BOARD), or "
                 "GPIO.setmode(GPIO.BCM)";
  }
  return -1;
}

std::string gpioFunction(int channel) {
  int channelInt = getGpioNumber(channel);

  if (channelInt == -1) {
    return "";
  }

  std::string varGpioFilePath =
      varGpioRoot + "/gpio" + std::to_string(channelInt) + "/direction";
  std::ifstream file(varGpioFilePath);

  if (!file) {
    return "GPIO.UNKNOWN";
  }

  std::string direction;
  // file >> direction;
  std::getline(file, direction);
  file.close();

  if (direction == "i") {
    return " GPIO.INPUT";
  } else if (direction == "o") {
    return " GPIO.OUTPUT";
  } else {
    return " GPIO.UNKNOWN";
  }
}

void setWarning(int state) {
  if (state == 1 || state == 0) {
    warningMode = state;
  } else {
    std::cout << "Error: {} is not a valid warning mode. Use one of the "
                 "following: True, 1, False, 0";
  }
}

int validateDirection(int channel, std::string validationType) {
  int channelInt = getGpioNumber(channel);

  if (channelInt == -1) {
    return -1;
  }

  if (validationType == "both" || validationType == "input" ||
      validationType == "output") {
    std::string varGpioFilePath =
        varGpioRoot + "/gpio" + std::to_string(channelInt) + "/direction";
    std::ifstream file(varGpioFilePath);
    std::string direction;
    if (!file) {
      direction = "none";
    } else {
      // file >> direction;
      std::getline(file, direction);
    }
    file.close();
    if (validationType == "input" && direction != "i") {
      std::cout
          << "You must setup() the GPIO channel ({0} {1}) as an input first";
      return 0;
    } else if (validationType == "output" && direction != "o") {
      std::cout
          << "You must setup() the GPIO channel ({0} {1}) as an output first";
      return 0;
    } else if (
        ((validationType == "both") &&
         (direction != "i" && direction != "o")) ||
        (direction == "none")) {
      std::cout << "You must setup() the GPIO channel ({0} {1}) first";
      return 0;
    } else {
      return 1;
    }
  } else {
    std::cout << "Error: {} is not a valid direction. use one of the "
                 "following: input, output, both";
  }
  return -1;
}

void setup(
    int channel,
    std::string direction,
    std::string pull_up_down,
    std::string initial) {
  std::vector<int> channelVec;

  if (typeid(channel) == typeid(int)) {
    channelVec.push_back(channel);
  }

  for (size_t index = 0; index < channelVec.size(); index++) {
    int channelInt = getGpioNumber(channelVec[index]);
    if (channelInt == 0) {
      return;
    }
    std::string varGpioFilepath =
        varGpioRoot + "/gpio" + std::to_string(channelInt) + "value";
    bool varGpioExist = access(varGpioFilepath.c_str(), F_OK) == 0;
    if (varGpioExist) {
      if (warningMode == 1) {
        std::cout << "This channel (" << gpioMode << " " << channelVec[index]
                  << ") is already in use, continuing anyway. Use "
                     "GPIO.setwarnings(False) to disable warnings.";
      }
    } else {
      try {
        varGpioFilepath = varGpioRoot + "/export";
        std::ofstream file(varGpioFilepath);
        file << std::to_string(channelInt);
        file.close();
      } catch (...) {
        std::cout << "Error: Unable to setup GPIO";
        return;
      }
    }
    try {
      varGpioFilepath =
          varGpioRoot + "/gpio" + std::to_string(channelInt) + "/direction";
      std::ofstream file(varGpioFilepath);
      file << direction;
      file.close();
    } catch (...) {
      std::cout << "Error: Unable to setup GPIO";
      return;
    }
    if (direction == "out") {
      try {
        varGpioFilepath =
            varGpioRoot + "/gpio" + std::to_string(channelInt) + "/value";
        std::ofstream file(varGpioFilepath);
        if (initial != "") {
          if (initial.find(",") != std::string::npos) {
            std::vector<std::string> initialValues;
            char* token = strtok(const_cast<char*>(initial.c_str()), ",");
            while (token != NULL) {
              initialValues.push_back(token);
              token = strtok(NULL, ",");
            }
            if (index < initialValues.size()) {
              file << initialValues[index];
            }
          } else {
            file << initial;
          }
        }
        file.close();
      } catch (...) {
        std::cout << "Error: Unable to setup GPIO initial state";
      }
    }
    if (direction == "in") {
      try {
        varGpioFilepath =
            varGpioRoot + "/gpio" + std::to_string(channelInt) + "/active_low";
        std::ofstream file(varGpioFilepath);
        file << pull_up_down;
        file.close();
      } catch (...) {
        std::cout << "Error: Unable to set internal pullup registor state";
      }
    }
  }
}

void output(int channel, std::vector<int> value) {
  std::vector<int> channelVec;

  if (typeid(channel) == typeid(int)) {
    channelVec.push_back(channel);
  }

  for (size_t index = 0; index < channelVec.size(); index++) {
    int channel_int = getGpioNumber(channelVec[index]);
    if (channel_int == 0) {
      return;
    }
    if (validateDirection(channelVec[index], "output") == 0) {
      return;
    }
    try {
      std::string var_gpio_filepath =
          varGpioRoot + "/gpio" + std::to_string(channel_int) + "/value";
      std::ofstream file(var_gpio_filepath);
      if (typeid(value) != typeid(int)) {
        file << std::to_string((value[index]));
      }
    } catch (...) {
      std::cout << "Error: Unable to set GPIO output state";
    }
  }
}

std::string input(int channel) {
  int channelInt = getGpioNumber(channel);

  if (channelInt == -1) {
    return "";
  }
  if (validateDirection(channel, "both") == 0) {
    return "";
  }

  try {
    std::string varGpioFilePath =
        varGpioRoot + "/gpio" + std::to_string(channelInt) + "value";
    std::ifstream file(varGpioFilePath);
    std::string value;
    std::getline(file, value);
    return value;
  } catch (...) {
    std::cout << "Error: Unable to get GPIO value";
  }
  return "";
}

bool wait_for_edge(
    int channel,
    std::string edge,
    std::string bouncetime,
    std::string timeout) {
  int channelInt = getGpioNumber(channel);

  if (channelInt == -1) {
    return false;
  }

  if (!validateDirection(channel, "input")) {
    return false;
  }

  if (edge != "RISING" && edge != "FALLING" && edge != "BOTH") {
    std::cout << "The edge must be set to \"RISING\", \"FALLING\", or \"BOTH\"";
    return false;
  }

  if (bouncetime != "none" && std::stoi(bouncetime) <= 0) {
    std::cout << "Bouncetime must be greater than 0";
    return false;
  }

  if (timeout != "none" && std::stoi(timeout) <= 0) {
    std::cout << "Timeout must be greater than 0" << std::endl;
    return false;
  }

  try {
    std::string varGpioFilepath =
        varGpioRoot + "/gpio" + std::to_string(channelInt) + "/edge";
    std::ofstream file(varGpioFilepath);
    file << edge;
    file.close();
  } catch (...) {
    std::cout << "Error: Unable to set GPIO edge state";
    return false;
  }

  std::string originalValue;
  try {
    std::string varGpioFilepath =
        varGpioRoot + "/gpio" + std::to_string(channelInt) + "/value";
    std::ifstream file(varGpioFilepath);
    std::getline(file, originalValue);
    file.close();
  } catch (...) {
    std::cout << "Error: Unable to get GPIO value";
    return false;
  }

  double timeoutSecs = (timeout != "none") ? std::stod(timeout) / 1000.0 : -1.0;
  double bouncetimeSecs =
      (bouncetime != "none") ? std::stod(bouncetime) / 1000.0 : -1.0;
  while (true) {
    std::string varGpioFilepath =
        varGpioRoot + "/gpio" + std::to_string(channelInt) + "/value";
    std::ifstream file(varGpioFilepath);
    std::string newValue;
    std::getline(file, newValue);
    file.close();

    if (newValue != originalValue) {
      if (bouncetimeSecs != -1.0) {
        usleep(bouncetimeSecs * 1000000);
      }
      return true;
    }
    usleep(10000);
    if (timeoutSecs != -1.0) {
      timeoutSecs -= 0.01;
      if (timeoutSecs <= 0.0) {
        return NULL;
      }
    }
  }
}