#include <ParticleSystem/CellList/cellList.h>
void CellList::resetLists(){
  for (int i = 0; i < nCells*nCells; i++){
    cells[i] = NULL_INDEX;
  }
  for (int i = 0; i < list.size(); i++){
    list[i] = NULL_INDEX;
  }
}

void CellList::insert(uint64_t next, uint64_t particle){
  uint64_t i = next;
  while (list[i] != NULL_INDEX){
    i = list[i];
  }
  list[i] = particle;
}

void CellList::insert(uint64_t particle, double x, double y){
    uint64_t c = hash(x,y);
    if (cells[c] == NULL_INDEX){
        cells[c] = particle;
    }
    else{
        insert(cells[c],particle);
    }
}

void CellList::cellInteractions(
  uint64_t a1,
  uint64_t b1,
  uint64_t a2,
  uint64_t b2,
  ParticleSystem * p,
  void (*callBack)(ParticleSystem * p, uint64_t i, uint64_t j)
){
  if (a1 < 0 || a1 >= nCells || b1 < 0 || b1 >= nCells || a2 < 0 || a2 >= nCells || b2 < 0 || b2 >= nCells){
    return;
  }
  uint64_t p1 = cells[a1*nCells+b1];
  uint64_t p2 = cells[a2*nCells+b2];

  if (p1 == NULL_INDEX || p2 == NULL_INDEX){
    return;
  }

  int a2NcPlusb2 = a2*nCells+b2;

  while (p1 != NULL_INDEX){
    p2 = cells[a2NcPlusb2];
    while(p2 != NULL_INDEX){
        callBack(p,p1,p2);
        p2 = list[p2];
    }
    p1 = list[p1];
  }
}

void CellList::handleInteractions(
    ParticleSystem * p, 
    void (*callBack)(ParticleSystem * p, uint64_t i, uint64_t j)
){

    for (int a = 0; a < nCells; a++){
        for (int b = 0; b < nCells; b++){
            cellInteractions(a,b,a,b,p,callBack);
            cellInteractions(a,b,a+1,b+1,p,callBack);
            cellInteractions(a,b,a+1,b-1,p,callBack);
            cellInteractions(a,b,a+1,b,p,callBack);
            cellInteractions(a,b,a,b+1,p,callBack);
        }
    }
}