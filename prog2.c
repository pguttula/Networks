#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 0    /* change to 1 for bidirectional */
                           /* and write a routine called B_output */

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg {
  char data[20];
  };

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
   int seqnum;
   int acknum;
   int checksum;
   char payload[20];
    };

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
  int seqnum_A = 1;
  int acknum_A = 0;
  int expected_seqnum_B = 1;
  int SENDER_A = 0;
  int SENDER_B = 1;
  float TIME = 25.0;
  int windowsize = 8;
  int receiver_window_size = 8;
  int buffersize = 50;
  int buffered_messages_count_A = 1;
  int B_packetcount =0;
  struct pkt *sent_packets;
  struct pkt *received_packets_buffered;
  struct pkt * acked_packets_B;
  struct msg *buffer_messages_A;
  int base = 1;
  int receiver_base =1;
  int buffered_messages_count_B = 0;
  //struct pkt bufferedmessages[50];
  int A_packetcount = 0;
  int acked_at_A = 0;
  int k;
  int expected_acknum_A =1;
  starttimer(int,int, float);
/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  base =1;
  sent_packets = (struct pkt*)malloc(windowsize * sizeof(struct pkt));
  buffer_messages_A = (struct msg*)malloc(buffersize * sizeof(struct msg));
  seqnum_A =1;
  k = 1;
}
//Function to calculate & return checksum
int checksum(int seqnum,int acknum, char * payload){
  int checksum=0;
  for (int i=0; i<20;i++){
    checksum = checksum + (int)payload[i];
  }
  checksum = checksum + (int)seqnum + (int)acknum;
  return checksum;
}
//Function to print the data in packet
printpacketdata(char * payload){
  int i;
  //for (i=0; packet.payload[i]; i++){
    printf("Packet payload is :%.20s \n",payload);
}

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  struct pkt packet;
  printf("***********A_Output*********\n");
  //if loop executes when there is a window avaiable for the message
  //to be sent. All the packets sent through this loop are stored in
  //sent_packets structure that points to the packet with their
  //seqnum. Timer code in emulator is changed such that we can start a
  //timer for every packet sent.starttimer now takes an extra arg
  //seqnum.
  if(seqnum_A < base + windowsize){

    strncpy(packet.payload, message.data, 20);
    packet.seqnum = seqnum_A;
    packet.acknum = seqnum_A;;
  //printf("length of message is %d: \n",strlen(packet.payload)); 
    packet.checksum = checksum(packet.seqnum , packet.acknum , packet.payload);
    printf("--------Packet being sent from A to B------- \n");
    printpacketdata(packet.payload);
    printf("Packet Sequence Number is: %d \n", packet.seqnum);
    printf("Packet Ack Number is: %d \n", packet.seqnum);
    printf("Packet CheckSum is: %d \n", packet.checksum);
    sent_packets[seqnum_A-1] = packet;
    A_packetcount++;
    seqnum_A++;
    starttimer(SENDER_A, seqnum_A-1, TIME);
    printf("Total packets sent from A to B so far: %d \n", A_packetcount);
    tolayer3(SENDER_A, packet);
  }
  //else loop buffers the messages that fall out of window size.
  else if(seqnum_A >= base + windowsize){
    if(buffered_messages_count_A < buffersize){
      printf("Window size is full. Message is being buffered \n");
      buffer_messages_A[buffered_messages_count_A] = message;
      buffered_messages_count_A++;
    }
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
 //If we receive a valid packet, Function stops the timer for that
 //particular packet and increments the base by 1 making room or a new
 //packet to be sent to B.
  printf("**********A_input********* \n");
  if(packet.checksum == checksum(packet.seqnum, 
                                   packet.acknum, 
                                   packet.payload)){
    printf("-------A Received acknowledgement %d from B-------\n", 
               packet.acknum);
  //  if(packet.seqnum > acked_at_A){
      stoptimer(SENDER_A, packet.acknum);
      printf("Timer stopped for packet wit seqnum %d \n",packet.seqnum);
   // }
    base = base + 1;
    acked_at_A++;
 //we check for any buffered messages present
 //and push them from A to B first.
    if(buffered_messages_count_A > 1 && k<= buffered_messages_count_A){
      A_output(buffer_messages_A[k-1]);
      k++;
    }
  }
}
/* called when A's timer goes off */
void A_timerinterrupt(int seqnum)
{
  //A_timerinterrupt's emulator code is changed again to ensure we
  //start nd stop timer for every packet individually. Once the timer
  //runs off for a particular seqnum, we pick the packet stored in
  //sentpackets for that seqnum and resend it to B.
  printf("************A_timerinterrupt*********** \n");
  printf("resending packet to B with seqnum %d \n", seqnum);
  tolayer3(SENDER_A, sent_packets[seqnum-1]);
  starttimer(SENDER_A, seqnum, TIME);
}  

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  received_packets_buffered = (struct pkt*)malloc(buffersize * sizeof(struct pkt));
  acked_packets_B = (struct pkt*)malloc(buffersize * sizeof(struct pkt));
}
/* called from layer 5, passed the data to be sent to other side */
void B_output(struct msg message)
{
}
//function to print packet info
void printpacket(struct pkt packet){
  printf("--------B received packet with Seqnum: %d from A------- \n",packet.seqnum);
  printpacketdata(packet.payload);
  printf("Packet Sequence Number is: %d \n", packet.seqnum);
  printf("Packet Ack Number is: %d \n", packet.seqnum);
  printf("Packet CheckSum is: %d \n",
        checksum(packet.seqnum, packet.acknum, packet.payload));
}
//Beif explanation of variables used in B_input
//receiver base -> incremented in B_input everytime we send an
//acknowledgment for a valid packet.
//expected_seqnum_B -> incremented everytime we acknowledge a
//packet.Keeps track of what B is expecting.
//B_packet_count-> just for printing the total packets received so
//far.
//buffered_messages_count -> packets are buffered in when we recieved
//out of order packets. This variable keeps count of number of packets
//buffered.
/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    printf("**********B_input********* \n");
    int i;
    printpacket(packet);
    if(packet.seqnum < (receiver_window_size + receiver_base -1)){
      if(packet.checksum == checksum(packet.seqnum,packet.acknum,packet.payload)){
//Enter the below if loop only if we receive and in order packet. Send
//an ACK to A. Save these packets in acked_packets_B structure
//pointing to the packet using it's seqnum
        if(packet.seqnum == expected_seqnum_B){
          printf("----------B Sending Acknowledgement %d to A for packet " 
              "with Seqnum: %d---------- \n", expected_seqnum_B, packet.seqnum);
          acked_packets_B[expected_seqnum_B] = packet;
          expected_seqnum_B++;
          receiver_base++;
          B_packetcount++;
          tolayer3(SENDER_B, packet);
          tolayer5(SENDER_B, packet);
        }
//When we receive a packet that is out of order, we have to check if
//(1)The packet is previously received at B and the ACK sent by B to A got lost
//(2)A packet from A got lost and the succeeding packets are arriving.
//(3) A packet from A that got lost previously is sent by A again
        else if(packet.seqnum != expected_seqnum_B){
          printf("----------B received unexpected packet from A with " 
              "seq num:%d---------\n",packet.seqnum);
          if(packet.seqnum < expected_seqnum_B){
            //Case 1 as explained above
            if(packet.seqnum == acked_packets_B[packet.seqnum].seqnum){
              printf("Packet was acknowledged earlier. Resending the ACK to A.\n");
              tolayer3(SENDER_B, packet);
            }
            else{
//case 3 as expalined above. Look for any buffered messages and send
//them to layer 5. Reset the buffered messages count.Increment the
//receiver base.
              printf("----------Lost or Corrupted Packet got re-sent \n--------");
              printf("----------B Sending Acknowledgement %d to A for packet " 
                  "with Seqnum: %d---------- \n", packet.seqnum, packet.seqnum);
              acked_packets_B[packet.seqnum] = packet;
              B_packetcount++;
              expected_seqnum_B++;
              tolayer3(SENDER_B, packet);
              printf("---------All the messages in buffer to be sent to upper layer----------- \n");
              if(buffered_messages_count_B>0){
                int count = 0;
                for (i = packet.seqnum + 1; i < packet.seqnum + 1+ buffered_messages_count_B; i++){
                  tolayer5(SENDER_B, received_packets_buffered[i]);
                  receiver_base++;
                  count ++;
                }
                buffered_messages_count_B = buffered_messages_count_B - count;
                //expected_seqnum_B = expected_seqnum_B + count;
              }
            }
          }
          else if(packet.seqnum > expected_seqnum_B){
            printf("Packet will be buffered and sending Ack %d to A for " 
                "packet with Seqnum %d \n", packet.seqnum, packet.seqnum);
            received_packets_buffered[packet.seqnum] = packet;
            acked_packets_B[packet.seqnum] = packet;
            B_packetcount++;
            buffered_messages_count_B++;
            expected_seqnum_B++;;
            tolayer3(SENDER_B, packet);
          }
        }
          printf("-------Total number of packets sent from "
              "A to B so far: %d------- \n", B_packetcount);
      }
      else{
        printf("---------B received a corupted packet--------\n");
      }
    }
}

/* called when B's timer goes off */
B_timerinterrupt()
{
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
  - emulates the tranmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event {
   float evtime;           /* event time */
   int evtype;             /* event type code */
   int eventity;           /* entity where event occurs */
   struct pkt *pktptr;     /* ptr to packet (if any) assoc w/ this event */
   struct event *prev;
   struct event *next;
 };
struct event *evlist = NULL;   /* the event list */

/* possible events: */
#define  TIMER_INTERRUPT 0  
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF             0
#define  ON              1
#define   A    0
#define   B    1
#define TIMER_B 999999



int TRACE = 1;             /* for my debugging */
int nsim = 0;              /* number of messages from 5 to 4 so far */ 
int nsimmax = 0;           /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;            /* probability that a packet is dropped  */
float corruptprob;         /* probability that one bit is packet is flipped */
float lambda;              /* arrival rate of messages from layer 5 */   
int   ntolayer3;           /* number sent into layer 3 */
int   nlost;               /* number lost in media */
int ncorrupt;              /* number corrupted by media*/

main()
{
   struct event *eventptr;
   struct msg  msg2give;
   struct pkt  pkt2give;
   
   int i,j;
   char c; 
  
   init();
   A_init();
   B_init();
   
   while (1) {
        eventptr = evlist;            /* get next event to simulate */
        if (eventptr==NULL)
           goto terminate;
        evlist = evlist->next;        /* remove this event from event list */
        if (evlist!=NULL)
           evlist->prev=NULL;
        if (TRACE>=2) {
           printf("\nEVENT time: %f,",eventptr->evtime);
           printf("  type: %d",eventptr->evtype);
           if (eventptr->evtype==0)
         printf(", timerinterrupt  ");
             else if (eventptr->evtype==1)
               printf(", fromlayer5 ");
             else
         printf(", fromlayer3 ");
           printf(" entity: %d\n",eventptr->eventity);
           }
        time = eventptr->evtime;        /* update time to next event time */
        if (nsim==nsimmax)
    break;                        /* all done with simulation */
        if (eventptr->evtype == FROM_LAYER5 ) {
            generate_next_arrival();   /* set up future arrival */
            /* fill in msg to give with string of same letter */    
            j = nsim % 26; 
            for (i=0; i<20; i++)  
               msg2give.data[i] = 97 + j;
            if (TRACE>2) {
               printf("          MAINLOOP: data given to student: ");
                 for (i=0; i<20; i++) 
                  printf("%c", msg2give.data[i]);
               printf("\n");
       }
            nsim++;
            if (eventptr->eventity == A)
              A_output(msg2give); 
             else
               B_output(msg2give);  
            }
          else if (eventptr->evtype ==  FROM_LAYER3) {
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i=0; i<20; i++)  
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
      if (eventptr->eventity ==A)      /* deliver packet by calling */
           A_input(pkt2give);            /* appropriate entity */
            else
           B_input(pkt2give);
      free(eventptr->pktptr);          /* free the memory for packet */
            }
          else if (eventptr->evtype ==  TIMER_INTERRUPT) {
            if (eventptr->eventity == TIMER_B) 
         B_timerinterrupt();
             else
         A_timerinterrupt(eventptr ->eventity);
             }
          else  {
       printf("INTERNAL PANIC: unknown event type \n");
             }
        free(eventptr);
        }

terminate:
   printf(" Simulator terminated at time %f\n after sending %d msgs from layer5\n",time,nsim);
}



init()                         /* initialize the simulator */
{
  int i;
  float sum, avg;
  float jimsrand();
  
  
   printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
   printf("Enter the number of messages to simulate: ");
   scanf("%d",&nsimmax);
   printf("Enter  packet loss probability [enter 0.0 for no loss]:");
   scanf("%f",&lossprob);
   printf("Enter packet corruption probability [0.0 for no corruption]:");
   scanf("%f",&corruptprob);
   printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
   scanf("%f",&lambda);
   printf("Enter TRACE:");
   scanf("%d",&TRACE);

   srand(9999);              /* init random number generator */
   sum = 0.0;                /* test random number generator for students */
   for (i=0; i<1000; i++)
      sum=sum+jimsrand();    /* jimsrand() should be uniform in [0,1] */
   avg = sum/1000.0;
   if (avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n" ); 
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
    exit(0);
    }

   ntolayer3 = 0;
   nlost = 0;
   ncorrupt = 0;

   time=0.0;                    /* initialize time to 0.0 */
   generate_next_arrival();     /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand() 
{
  double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
  float x;                   /* individual students may need to change mmm */ 
  x = rand()/mmm;            /* x should be uniform in [0,1] */
  return(x);
}  

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/
 
generate_next_arrival()
{
   double x,log(),ceil();
   struct event *evptr;
    char *malloc();
   float ttime;
   int tempint;

   if (TRACE>2)
       printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");
 
   x = lambda*jimsrand()*2;  /* x is uniform on [0,2*lambda] */
                             /* having mean of lambda        */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + x;
   evptr->evtype =  FROM_LAYER5;
   if (BIDIRECTIONAL && (jimsrand()>0.5) )
      evptr->eventity = B;
    else
      evptr->eventity = A;
   insertevent(evptr);
} 


insertevent(p)
   struct event *p;
{
   struct event *q,*qold;

   if (TRACE>2) {
      printf("            INSERTEVENT: time is %lf\n",time);
      printf("            INSERTEVENT: future time will be %lf\n",p->evtime); 
      }
   q = evlist;     /* q points to header of list in which p struct inserted */
   if (q==NULL) {   /* list is empty */
        evlist=p;
        p->next=NULL;
        p->prev=NULL;
        }
     else {
        for (qold = q; q !=NULL && p->evtime > q->evtime; q=q->next)
              qold=q; 
        if (q==NULL) {   /* end of list */
             qold->next = p;
             p->prev = qold;
             p->next = NULL;
             }
           else if (q==evlist) { /* front of list */
             p->next=evlist;
             p->prev=NULL;
             p->next->prev=p;
             evlist = p;
             }
           else {     /* middle of list */
             p->next=q;
             p->prev=q->prev;
             q->prev->next=p;
             q->prev=p;
             }
         }
}

printevlist()
{
  struct event *q;
  int i;
  printf("--------------\nEvent List Follows:\n");
  for(q = evlist; q!=NULL; q=q->next) {
    printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
    }
  printf("--------------\n");
}



/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
/* A or B is trying to stop timer */
stoptimer(int AorB, int seqnum){
 struct event *q,*qold;

 if (TRACE>2)
    printf("          STOP TIMER: stopping timer at %f\n",time);
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==seqnum) ) { 
       /* remove this event */
       if (q->next==NULL && q->prev==NULL)
             evlist=NULL;         /* remove first and only event on list */
          else if (q->next==NULL) /* end of list - there is one in front */
             q->prev->next = NULL;
          else if (q==evlist) { /* front of list - there must be event after */
             q->next->prev=NULL;
             evlist = q->next;
             }
           else {     /* middle of list */
             q->next->prev = q->prev;
             q->prev->next =  q->next;
             }
       free(q);
       return;
     }
  printf("Warning: unable to cancel your timer. It wasn't running.\n");
}


starttimer(int AorB, int seqnum, float increment){

 struct event *q;
 struct event *evptr;
 char *malloc();

 if (TRACE>2)
    printf("          START TIMER: starting timer at %f\n",time);
 /* be nice: check to see if timer is already started, if so, then  warn */
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
   for (q=evlist; q!=NULL ; q = q->next)  
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
      printf("Warning: attempt to start a timer that is already started\n");
      return;
      }
 
/* create future event for when timer goes off */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + increment;
   evptr->evtype =  TIMER_INTERRUPT;
   evptr->eventity = seqnum;
   insertevent(evptr);
} 


/************************** TOLAYER3 ***************/
tolayer3(int AorB, struct pkt packet){
 struct pkt *mypktptr;
 struct event *evptr,*q;
 char *malloc();
 float lastime, x, jimsrand();
 int i;


 ntolayer3++;

 /* simulate losses: */
 if (jimsrand() < lossprob)  {
      nlost++;
      if (TRACE>0)    
  printf("          TOLAYER3: packet being lost\n");
      return;
    }  

/* make a copy of the packet student just gave me since he/she may decide */
/* to do something with the packet after we return back to him/her */ 
 mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
 mypktptr->seqnum = packet.seqnum;
 mypktptr->acknum = packet.acknum;
 mypktptr->checksum = packet.checksum;
 for (i=0; i<20; i++)
    mypktptr->payload[i] = packet.payload[i];
 if (TRACE>2)  {
   printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
    mypktptr->acknum,  mypktptr->checksum);
    for (i=0; i<20; i++)
        printf("%c",mypktptr->payload[i]);
    printf("\n");
   }

/* create future event for arrival of packet at the other side */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtype =  FROM_LAYER3;   /* packet will pop out from layer3 */
  evptr->eventity = (AorB+1) % 2; /* event occurs at other entity */
  evptr->pktptr = mypktptr;       /* save ptr to my copy of packet */
/* finally, compute the arrival time of packet at the other end.
   medium can not reorder, so make sure packet arrives between 1 and 10
   time units after the latest arrival time of packets
   currently in the medium on their way to the destination */
 lastime = time;
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==FROM_LAYER3  && q->eventity==evptr->eventity) ) 
      lastime = q->evtime;
 evptr->evtime =  lastime + 1 + 9*jimsrand();
 


 /* simulate corruption: */
 if (jimsrand() < corruptprob)  {
    ncorrupt++;
    if ( (x = jimsrand()) < .75)
       mypktptr->payload[0]='Z';   /* corrupt payload */
      else if (x < .875)
       mypktptr->seqnum = 999999;
      else
       mypktptr->acknum = 999999;
    if (TRACE>0)    
  printf("          TOLAYER3: packet being corrupted\n");
    }  

  if (TRACE>2)  
     printf("          TOLAYER3: scheduling arrival on other side\n");
  insertevent(evptr);
} 

tolayer5(int AorB, char datasent[20])
{
  int i;  
  if (TRACE>2) {
     printf("          TOLAYER5: data received: ");
     for (i=0; i<20; i++)  
        printf("%c",datasent[i]);
     printf("\n");
   }
  
}
