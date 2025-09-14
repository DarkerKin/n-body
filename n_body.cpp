using namespace std;

struct Position{
    int x;
    int y;
    int z;
};

struct velocities{
    int vx;
    int vy;
    int vz;
};

struct Force{
    int fx;
    int fy;
    int fz;
};


class N_body_state{
    private:
        struct Position n_body_position;
        struct velocities n_body_velocity;
        struct Force n_body_force;
    public:
        void init_n_body();
    
};

