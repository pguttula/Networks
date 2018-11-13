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
} dt3;

static int my_id = 3;
static int min_costs[4] = {999,999,999,999};
static int link_costs[4] = {7,999,2,0};
static int neighbors[2] = {0,2};
static int n_neighbors = 2;
void printdt3(struct distance_table*);

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
  int updated = 0;
  int i,j;
  for(i=0;i<4;i++) {
    for(j=0;j<4;j++) {
      if(dt3.costs[i][j] < min_costs[i]){
        min_costs[i] = dt3.costs[i][j];
        updated = 1;
      } 
    }
  }
  return updated;
}


void rtinit3() 
{
  printf("Node 3 initialized at time: %f\n",clocktime);
  int i=0, j=0;
  for(i=0;i<4;i++) {
    for(j=0;j<4;j++){
      if(i==j){
        dt3.costs[i][j] = link_costs[i];
      }
      else {
        dt3.costs[i][j] = 999;
      }
    }
  }
  printdt3(&dt3);
  update_min_costs();
  inform_neighbors();
}


static char* marker = "    ";
void rtupdate3(struct rtpkt *rcvdpkt)
{
  printf("Node 3 updated at time: %f\n",clocktime);
  printf("%s source id: %d %s\n", marker, rcvdpkt->sourceid, marker);
  printf("%s dest id: %d %s\n", marker, rcvdpkt->destid, marker);
  printf("%s mincost: { %d %d %d %d } %s\n", marker, 
    rcvdpkt->mincost[0], rcvdpkt->mincost[1], rcvdpkt->mincost[2], rcvdpkt->mincost[3], marker);
  int source_id = rcvdpkt->sourceid;
  int i, updated=0;
  for (i=0;i<4;i++) {
    int to_i_via_source_id = link_costs[source_id] + rcvdpkt->mincost[i];
    if (to_i_via_source_id < 999) {
      dt3.costs[i][source_id] = to_i_via_source_id;
      updated = 1;
    }
  }
  if (updated == 1) {
    printf("%s Distance table updated %s\n",marker,marker);
  }
  else {
    printf("%s Distance table NOT updated %s\n",marker,marker);
  }
  printdt3(&dt3);
  if (update_min_costs()) {
    printdt3(&dt3);
    inform_neighbors();
  }
  return;
}


void printdt3(struct distance_table *dtptr)
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}







