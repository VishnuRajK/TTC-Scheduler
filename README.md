# TTC-Scheduler
Time Triggered Cooperative scheduler with overrun protection.
This coursework implements an offline, static, co-operative time-triggered scheduler which is trivially optimal, and performs 8 short periodic tasks out of which four are executed during all clock intervals, 
two during odd clock intervals, and the remaining two during even clock intervals. 
This scheduler must be able to detect if any task is faulty, which takes up more time causing other tasks to miss the deadline. 
Hence the scheduler must be implemented with overrun protection. 
Each task is to control the brightness of one LED using Pulse Width Modulation (PWM).
