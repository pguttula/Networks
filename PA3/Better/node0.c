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
} dt0;

static int my_id = 0;
static int min_costs[4] = {999,999,999,999};
static int min_nexts[4] = {999, 999, 999, 999};
static int link_costs[4] = {0,1,3,7};
static int neighbors[3] = {1,2,3};
static int n_neighbors = 3;
void printdt0(struct distance_table*);

/* students to write the following two routines, and maybe some others */

static void inform_neighbor(int n) {
  struct rtpkt pkt;
  int i, poisoned_min_costs[4];
  pkt.sourceid = my_id;
  pkt.destid = n;
  /* Define poisoned min cost to i as infinity if the next node
   * on min path is n. Else define it as min cost to i. */
  for(i=0; i<4; i++) {
    if(min_nexts[i] == n) {
      //printf("Node %d -> Node %d: poisoning cost to %d\n", my_id, n, i);
      poisoned_min_costs[i] = 999;
    }
    else {
      poisoned_min_costs[i] = min_costs[i];
    }
  }
  memcpy(pkt.mincost, poisoned_min_costs, 4*sizeof(int));
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
      if(dt0.costs[i][j] < min) {
        min = dt0.costs[i][j];
        min_nexts[i] = j;
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

void rtinit0() 
{
  printf("Node 0 initialized at time: %f\n",clocktime);
  int i=0, j=0;
  for(i=0;i<4;i++) {
    for(j=0;j<4;j++){
      if(i==j){
        dt0.costs[i][j] = link_costs[i];
      }
      else {
        dt0.costs[i][j] = 999;
      }
    }
  }
  printdt0(&dt0);
  update_min_costs();
  inform_neighbors();
}


static char* marker = "    ";
void rtupdate0(struct rtpkt *rcvdpkt)
{
  printf("Node 0 updated at time: %f\n",clocktime);
  printf("%s source id: %d %s\n", marker, rcvdpkt->sourceid, marker);
  printf("%s dest id: %d %s\n", marker, rcvdpkt->destid, marker);
  printf("%s mincost: { %d %d %d %d } %s\n", marker, 
    rcvdpkt->mincost[0], rcvdpkt->mincost[1], rcvdpkt->mincost[2], rcvdpkt->mincost[3], marker);
  int source_id = rcvdpkt->sourceid;
  int i, updated=0;
  for (i=0;i<4;i++) {
    int to_i_via_source_id = link_costs[source_id] + rcvdpkt->mincost[i];
    if (to_i_via_source_id < 999) {
      dt0.costs[i][source_id] = to_i_via_source_id;
      updated = 1;
    }
  }
  if (updated == 1) {
    printf("%s Distance table updated %s\n",marker,marker);
  }
  else {
    printf("%s Distance table NOT updated %s\n",marker,marker);
  }
  printdt0(&dt0);
  if (update_min_costs()) {
    printdt0(&dt0);
    inform_neighbors();
  }
  return;
}


void printdt0(struct distance_table *dtptr)  
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
	 dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
	 dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
	 dtptr->costs[3][2],dtptr->costs[3][3]);
}

void linkhandler0(int linkid, int newcost)   
/* called when cost from 0 to linkid changes from current value to newcost*/
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
  /* update the distance table entries routed via linkid */
  for(i=0; i<4; i++) {
    dt0.costs[i][linkid] = dt0.costs[i][linkid] - oldcost + newcost;
  }
  /* Since old min costs may no longer be valid, update min costs
   * and min_nexts starting from scratch */
  int old_min_costs[4], old_min_nexts[4];
  int inftys[4] = {999,999,999,999};
  memcpy(old_min_costs,min_costs,4*sizeof(int));
  memcpy(old_min_nexts,min_nexts,4*sizeof(int));
  memcpy(min_costs,inftys,4*sizeof(int));
  memcpy(min_nexts,inftys,4*sizeof(int));
  update_min_costs();
  /* If either min costs or min nexts change, inform neighbors of new
   * min costs*/
  if(memcmp(old_min_costs, min_costs, 4*sizeof(int)) != 0 || 
      memcmp(old_min_nexts, min_nexts, 4*sizeof(int)) !=0) {
    printf("%s Min costs updated\n",marker);
    printdt0(&dt0);
    inform_neighbors();
  }
}

