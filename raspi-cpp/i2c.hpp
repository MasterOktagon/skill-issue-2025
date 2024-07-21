enum Task {
  WAIT,
  FIND_VICTIM,
  FIND_CORNER,
  FIND_EXIT,
};
extern Task i2c_get_task();
