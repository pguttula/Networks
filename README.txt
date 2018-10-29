Alternating Bit Protocol- Bidirectional:  
* Use cc -w prog2_arq.c to compile code
* Default mode is Biderectional. Please change flag #define
  BIDIRECTIONAL from 1 to 0 for Unidirectional mode.
*Protocol uses:
  1) Sequence numbers and Acknowledgment numbers 0,1 to handle messages sent from A to B
  2) Sequence numbers and Acknowledgement numbers 2,3 to handle messages sent from B to A
* output_arq.pdf has the corruption, loss and recovery scenarios
  highlighted for first 10 messages recived correctly.

Go-Back N Protocol:
* Use cc -w prog2_gbn.c to compile code
* output_gbn.pdf has the corruption, loss and recovery scenarios 
  highlighted for first 20 messages recived correctly.

Selective- Repeat Protocol:
* Use cc -w prog2_sr.c to compile code
* output_sr.pdf has the corruption, loss and recovery scenarios
  highlighted for first 20 messages recived correctly.
* Modified simulator code to set multiple timers. 
  Modified the functions A_timerinterrupt, starttimer, stoptimer
