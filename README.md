** Loviisa OS **

Extremely lightweight embedded operating system with minimum resource requirements, complexity and introduction period.

Loviisa is generally designed as a cross-platform easy portable operating environment for extremely low resource microcontrollers, where using of any regular OS is not applicable because of the inacceptable overhead. Loviisa provides a software developer with similar capabilities that the most of Embedded RTOS, but spends just dozens of bytes for its own structures and very few ticks to execute OS primitives.

Like most of embedded OS it is implemented as C library that can be used on almost every microcontroller and with any vendor-specific SDK or runtime. Loviisa is 100% C code without any machine-specific assembler code or high level code generation. It widely uses macros to extend regular C syntax and make OS constructions and operations native in use.

Loviisa is not a real time operating system in the commonly used meaning of real time. At least it cannot be used for hard real time tasks because of cooperative multitasking. However it can be used for multiple IoT applications with soft real time and even can be adjusted to hard real time tasks if required, but with valueable increase of the usage complexity.

The operating system is based on events concept. Event is a message that can be queued from anywhere within the program and handled by embedded scheduler. For single shot routines that don't have long loops and sequential parts, the mechanism of message subscriptions and message handlers can be used with maximum application efficiency. Regular imperative code with long algorithms needing switching in the middle need to be implemented as tasks. Loviisa tasks are lightweight threads or better say co-programs that are preempted by each other transparently for the application programmer. Yield operation shall be used in a limited number of situations.

## License

MIT — see [LICENSE](LICENSE).
