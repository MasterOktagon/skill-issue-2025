/*
File that handles all the communication with the ESP32
*/

namespace i2c {

  enum Task {
  /*
  Represents different tasks/states given by the ESP32
  */
    WAIT,
    FIND_VICTIM,
    FIND_CORNER,
    FIND_EXIT,
    END
  };

  extern Task get_task();
  /*
  [return] get the task given by the ESP32
  */


}
