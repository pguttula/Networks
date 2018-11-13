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
} dt2;

static int my_id = 2;
static int min_costs[4] = {999,999,999,999};
static int link_costs[4] = {3,1,0,2};
static int neighbors[3] = {0,1,3};
static int n_neighbors = 3;
void printdt2(struct distance_table*);

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
      if(dt2.costs[i][j] < min) {
        min = dt2.costs[i][j];
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


void rtinit2() 
{
  printf("Node 2 initialized at time: %f\n",clocktime);
  int i=0, j=0;
  for(i=0;i<4;i++) {
    for(j=0;j<4;j++){
      if(i==j){
        dt2.costs[i][j] = link_costs[i];
      }
      else {
        dt2.costs[i][j] = 999;
      }
    }
  }
  printdt2(&dt2);
  update_min_costs();
  inform_neighbors();
}


static char* marker = "    ";
void rtupdate2(struct rtpkt *rcvdpkt)
{
  printf("Node 2 updated at time: %f\n",clocktime);
  printf("%s source id: %d %s\n", marker, rcvdpkt->sourceid, marker);
  printf("%s dest id: %d %s\n", marker, rcvdpkt->destid, marker);
  printf("%s mincost: { %d %d %d %d } %s\n", marker, 
    rcvdpkt->mincost[0], rcvdpkt->mincost[1], rcvdpkt->mincost[2], rcvdpkt->mincost[3], marker);
  int source_id = rcvdpkt->sourceid;
  int i, updated = 0;
  for (i=0;i<4;i++) {
    int to_i_via_source_id = link_costs[source_id] + rcvdpkt->mincost[i];
    //printf("From %d to %d via %d: %d\n", my_id, i, source_id, to_i_via_source_id);
    if (to_i_via_source_id < 999) {
      dt2.costs[i][source_id] = to_i_via_source_id;
      updated = 1;
    }
  }
  if (updated == 1) {
    printf("%s Distance table updated %s\n",marker,marker);
  }
  else {
    printf("%s Distance table NOT updated %s\n",marker,marker);
  }
  printdt2(&dt2);
  if (update_min_costs() == 1) {
    printdt2(&dt2);
    inform_neighbors();
  }
  return;
}


void printdt2(struct distance_table *dtptr)
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
	 dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
	 dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
	 dtptr->costs[3][1],dtptr->costs[3][3]);
}







