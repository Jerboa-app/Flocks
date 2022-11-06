#ifndef CELLLIST_H
#define CELLLIST_H

class ParticleSystem;

class CellList{
public:
    CellList(double radius, double Lx = 1.0, double Ly = 0.90, int sizeHint=10000)
    : cutOff(radius), Lx(Lx), Ly(Ly)
    {

        N = sizeHint;

        nCells = std::ceil(1.0/(4.0*cutOff));
        deltax = Lx / nCells;
        deltay = Ly / nCells;
        
        for (int c = 0; c < nCells*nCells; c++){
            cells.push_back(NULL_INDEX);
        }

        for (int i = 0; i < N; i++){
            list.push_back(NULL_INDEX);
        }
    }

    void resetLists();
    void insert(uint64_t particle, double x, double y);
    void handleInteractions(ParticleSystem * p, void (*callBack)(ParticleSystem * p, uint64_t i, uint64_t j));

private:
    uint64_t nCells;
    uint64_t N;
    double deltax;
    double deltay;
    double cutOff;
    double Lx;
    double Ly;

    std::vector<uint64_t> cells;
    std::vector<uint64_t> list;

    uint64_t hash(float x, float y){
        return uint64_t(floor(x/deltax))*nCells + uint64_t(floor(y/deltay));
    }

    void insert(uint64_t next, uint64_t particle);

    void cellInteractions(
        uint64_t a1,
        uint64_t b1,
        uint64_t a2,
        uint64_t b2,
        ParticleSystem * p,
        void (*callBack)(ParticleSystem * p, uint64_t i, uint64_t j)
    );
};

#endif