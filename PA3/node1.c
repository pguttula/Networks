#include <stdio.h>
#include<string.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
extern float clocktime;

struct distance_table 
{
  int costs[4][4];
} dt1;

static int my_id = 1;
static int min_costs[4] = {999,999,999,999};
static int link_costs[4] = {1,0,1,999};
static int neighbors[2] = {0,2};
static int n_neighbors = 2;
void printdt1(struct distance_table*);

/* students to write the following two routines, and maybe some others */

static void inform_neighbor(int n) {
  struct rtpkt pkt;
  pkt.sourceid = my_id;
  pkt.destid = n;
  memcpy(pkt.mincost, min_costs, 4*sizeof(int));
  tolayer2(pkt);
}

static void inform_neighbors() {
  int i;
  for(i=0;i<n_neighbors;i++) {
    inform_neighbor(neighbors[i]);
  }
}

/* Update min costs to all nodes. Return 1 if anything changed, 0
 * otherwise.  */
static int update_min_costs() {
  int i,j, min;
  int mins[4] = {999,999,999,999};
  for(i=0;i<4;i++) {
    min = 999;
    for(j=0;j<4;j++) {
      if(dt1.costs[i][j] < min) {
        min = dt1.costs[i][j];
      } 
    }
    mins[i] = min;
  }
  if(memcmp(mins,min_costs,4*sizeof(int)) == 0) {
    return 0;
  }
  else {
    memcpy(min_costs,mins,4*sizeof(int));
    return 1;
  }
}

void rtinit1() 
{
  printf("Node 1 initialized at time: %f\n",clocktime);
  int i=0, j=0;
  for(i=0;i<4;i++) {
    for(j=0;j<4;j++){
      if(i==j){
        dt1.costs[i][j] = link_costs[i];
      }
      else {
        dt1.costs[i][j] = 999;
      }
    }
  }
  printdt1(&dt1);
  update_min_costs();
  inform_neighbors();
}


static char* marker = "    ";
void rtupdate1(struct rtpkt *rcvdpkt)
{
  printf("Node 1 updated at time: %f\n",clocktime);
  printf("%s source id: %d %s\n", marker, rcvdpkt->sourceid, marker);
  printf("%s dest id: %d %s\n", marker, rcvdpkt->destid, marker);
  printf("%s mincost: { %d %d %d %d } %s\n", marker, 
    rcvdpkt->mincost[0], rcvdpkt->mincost[1], rcvdpkt->mincost[2], rcvdpkt->mincost[3], marker);
  int source_id = rcvdpkt->sourceid;
  int i, updated = 0;
  for (i=0;i<4;i++) {
    int to_i_via_source_id = link_costs[source_id] + rcvdpkt->mincost[i];
    if (to_i_via_source_id < 999) {
      dt1.costs[i][source_id] = to_i_via_source_id;
      updated = 1;
    }
  }
  if (updated == 1) {
    printf("%s Distance table updated %s\n",marker,marker);
  }
  else {
    printf("%s Distance table NOT updated %s\n",marker,marker);
  }
  printdt1(&dt1);
  if (update_min_costs()) {
    printdt1(&dt1);
    inform_neighbors();
  }
  return;
}


void printdt1(struct distance_table *dtptr)
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



void linkhandler1(int linkid, int newcost)    
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
	
{
  printf("NODE %d <-> NODE: %d link cost updated to %d at time: %f\n", my_id, linkid, 
      newcost, clocktime);
  int i;
  /* update link cost */
  int oldcost = link_costs[linkid];
  link_costs[linkid] = newcost;
  /* update the distance table */
  for(i=0; i<4; i++) {
    dt1.costs[i][linkid] = dt1.costs[i][linkid] - oldcost + newcost;
  }
  /* Since old min costs may no longer be valid, update min costs
   * starting from scratch */
  int old_min_costs[4];
  int inftys[4] = {999,999,999,999};
  memcpy(old_min_costs,min_costs,4*sizeof(int));
  memcpy(min_costs,inftys,4*sizeof(int));
  update_min_costs();
  /* If new min costs are not same as old, inform neighbors of new
   * min costs*/
  if(memcmp(old_min_costs,min_costs,4*sizeof(int)) != 0) {
    printf("%s Min costs updated\n",marker);
    //printf("%s Old mins: %d %d %d %d\n", marker, old_min_costs[0],
    //    old_min_costs[1], old_min_costs[2], old_min_costs[3]);
    //printf("%s New mins: %d %d %d %d\n", marker, min_costs[0],
    //    min_costs[1], min_costs[2], min_costs[3]);
    printdt1(&dt1);
    inform_neighbors();
  }
}


