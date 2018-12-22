#pragma once

template <class ControlBlock, class Extension, class Operation, class... Args>
ControlBlock& create_operation(Extension& ext, Operation&& op, Args... args) {
  auto& control_block = static_cast<ControlBlock&>(ext);
  control_block.add(op(control_block.object(), args...));
  return control_block;
}
