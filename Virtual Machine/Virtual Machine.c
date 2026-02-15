/*
This is a simple implementation of a virtual machine in C. It supports basic arithmetic operations, memory management, and control flow. 
The virtual machine executes a series of instructions defined in a bytecode format.

The instruction set includes:
- LOAD: Load a value into a register
- ADD: Add two registers and store the result in a register
- SUB: Subtract two registers and store the result in a register
- MUL: Multiply two registers and store the result in a register
- DIV: Divide two registers and store the result in a register
- JMP: Jump to a specific instruction
- JZ: Jump if zero
- HALT: Stop execution
- PUSH: Push a value onto the stack
- POP: Pop a value from the stack
- PRINT: Print the value of a register

The virtual machine uses a simple memory model with a fixed-size memory array and a set of registers.

Author: Zane Francis
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Virtual_Machine.h"

// Initialize the virtual machine (zero out memory and registers)
void vm_init(VirtualMachine *vm) {
    memset(vm->memory, 0, MEMORY_SIZE);
    memset(vm->registers, 0, NUM_REGISTERS);
    memset(vm->stack, 0, MEMORY_SIZE);
    vm->pc = 0;
    vm->sp = MEMORY_SIZE - 1;
    vm->running = 0;
}

// Function to load the program into the virtual machine
void vm_load_program(VirtualMachine *vm, uint8_t *program, size_t size) {
    
    // Safety check so program doesn't exceed memory size
    if (size > MEMORY_SIZE) {
        fprintf(stderr, "Error, Program size (%zu bytes) exceeds the memory siz (%d bytes)\n", 
            size, MEMORY_SIZE);
        return;
    }
    // Copy bytes to memory (can use memcpy, but wanted to hard code it)
    for (int i = 0; i < size; i++){
        vm->memory[i] = program[i];
    }
    // set running flag
    vm->running = 1;
}

// Execution function for the virtual machine
void vm_execute(VirtualMachine *vm) {
    while (vm->running) {
        // Fetch Opcode
        uint8_t opcode = vm->memory[vm->pc++];

        switch (opcode) {
            // Case for ending the virtual machine
            case OP_HALT:
                vm->running = 0;
                break;
            //Case for loading the registers and values
            case OP_LOAD: {
                // Fetch register number and value
                uint8_t reg = vm->memory[vm->pc++];
                uint8_t value = vm->memory[vm->pc++];

                // Saftey check for register
                if (reg >= NUM_REGISTERS) {
                    fprintf(stderr, "Error, Invalid register %d\n", reg);
                    vm->running = 0;
                    break;
                }
                // Add register value to register
                vm->registers[reg] = value;
                break;
            }
            // Case for printing
            case OP_PRINT: {
                // Fetch register number
                uint8_t reg = vm->memory[vm->pc++];

                // Saftey check for register
                if (reg >= NUM_REGISTERS) {
                    fprintf(stderr, "Error, Invalid register %d\n", reg);
                    vm->running = 0;
                    break;
                }
                // Print value in register
                fprintf(stderr, "Register value: %d", vm->registers[reg]);                
                break;
            }
            // Case for addition
            case OP_ADD: {
                // Fetch registers
                uint8_t fir_reg = vm->memory[vm->pc++];
                uint8_t sec_reg = vm->memory[vm->pc++];
                uint8_t des_reg = vm->memory[vm->pc++];

                // Saftey check for registers
                if (fir_reg >= NUM_REGISTERS || sec_reg >= NUM_REGISTERS || des_reg >= NUM_REGISTERS) {
                    fprintf(stderr, "Error, Invalid register %d or %d or %d\n",
                        fir_reg, sec_reg, des_reg);
                    vm->running = 0;
                    break;
                }
                // Add first and second reg and store in destintaion reg 
                vm->registers[des_reg] = vm->registers[fir_reg] + vm->registers[sec_reg];
                break;
            }
            // Case for subtraction
            case OP_SUB: {
                // Fetch registers
                uint8_t fir_reg = vm->memory[vm->pc++];
                uint8_t sec_reg = vm->memory[vm->pc++];
                uint8_t des_reg = vm->memory[vm->pc++];

                // Safety check for registers
                if (fir_reg >= NUM_REGISTERS || sec_reg >= NUM_REGISTERS || des_reg >= NUM_REGISTERS) {
                    fprintf(stderr, "Error, Invalid register %d or %d or %d\n", 
                        fir_reg, sec_reg, des_reg);
                    vm->running = 0;
                    break;
                }

                // Substract second from first reg and store in destination reg
                vm->registers[des_reg] = vm->registers[fir_reg] - vm->registers[sec_reg];
                break;
            }
            // Case for multiplication
            case OP_MUL: {
                // Fetch registers
                uint8_t fir_reg = vm->memory[vm->pc++];
                uint8_t sec_reg = vm->memory[vm->pc++];
                uint8_t des_reg = vm->memory[vm->pc++];
                
                // Safety check for registers
                if (fir_reg >= NUM_REGISTERS || sec_reg >= NUM_REGISTERS || des_reg >= NUM_REGISTERS) {
                    fprintf(stderr, "Error, invalid register %d or %d or %d\n", 
                        fir_reg, sec_reg, des_reg);
                    vm->running = 0;
                    break;
                }

                // Multiply first and second reg and store in destination reg
                vm->registers[des_reg] = vm->registers[fir_reg] * vm->registers[sec_reg];
                break;
            }
            // Case for division
            case OP_DIV: {
                // Fetch registers
                uint8_t fir_reg = vm->memory[vm->pc++];
                uint8_t sec_reg = vm->memory[vm->pc++];
                uint8_t des_reg = vm->memory[vm->pc++];

                // Safety check for registers
                if (fir_reg >= NUM_REGISTERS || sec_reg >= NUM_REGISTERS || des_reg >= NUM_REGISTERS) {
                    fprintf(stderr, "Error, Invalid register %d or %d or %d\n", 
                        fir_reg, sec_reg, des_reg);
                    vm->running = 0;
                    break;
                }
                if (vm->registers[sec_reg] == 0) {
                    fprintf(stderr, "Error, can not divide when register %d is 0.\n", sec_reg);
                    vm->running = 0;
                    break;
                }

                // Divide the second reg from the first and store in destination reg
                vm->registers[des_reg] = vm->registers[fir_reg] / vm->registers[sec_reg];
                break;
            }
            // Case for jumping to register
            case OP_JMP: {
                // Fetch the address of the register to jump to
                uint8_t high_byte = vm->memory[vm->pc++];
                uint8_t low_byte = vm->memory[vm->pc++];
                uint16_t address = (high_byte << 8) | low_byte;
                
                if (address >= MEMORY_SIZE) {
                    fprintf(stderr, "Error invalid address %d\n", address);
                    vm->running = 0;
                    break;
                }

                // Jump to register
                vm->pc = address;
                break;
            }
            // Case for jumping to zero
            case OP_JZ: {
                
            }
        }
    }
}