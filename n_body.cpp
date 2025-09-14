#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>   // for atoi, atof, rand
#include <chrono>    // for benchmarking

const double G = 6.674e-11;   // gravitational constant
const double softening = 1e-9; // to avoid division by zero

struct Particle {
    double mass;
    double x, y, z;   // position
    double vx, vy, vz; // velocity
    double fx, fy, fz; // force accumulator
};

void reset_forces(std::vector<Particle>& particles) {
    for (auto& p : particles) {
        p.fx = p.fy = p.fz = 0.0;
    }
}

void compute_forces(std::vector<Particle>& particles) {
    int n = particles.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            double dx = particles[j].x - particles[i].x;
            double dy = particles[j].y - particles[i].y;
            double dz = particles[j].z - particles[i].z;

            double dist2 = dx*dx + dy*dy + dz*dz + softening;
            double dist = sqrt(dist2);
            double force = (G * particles[i].mass * particles[j].mass) / dist2;

            double fx = force * dx / dist;
            double fy = force * dy / dist;
            double fz = force * dz / dist;

            particles[i].fx += fx;
            particles[i].fy += fy;
            particles[i].fz += fz;

            particles[j].fx -= fx;
            particles[j].fy -= fy;
            particles[j].fz -= fz;
        }
    }
}

void update_motion(std::vector<Particle>& particles, double dt) {
    for (auto& p : particles) {
        double ax = p.fx / p.mass;
        double ay = p.fy / p.mass;
        double az = p.fz / p.mass;

        p.vx += ax * dt;
        p.vy += ay * dt;
        p.vz += az * dt;

        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

void dump_state_tsv(const std::vector<Particle>& particles, std::ofstream& out) {
    out << particles.size();
    for (const auto& p : particles) {
        out << "\t"
            << std::scientific << p.mass << "\t"
            << p.x << "\t" << p.y << "\t" << p.z << "\t"
            << p.vx << "\t" << p.vy << "\t" << p.vz << "\t"
            << p.fx << "\t" << p.fy << "\t" << p.fz;
    }
    out << "\n";
}

std::vector<Particle> load_particles_from_file(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Error: could not open input file " + filename);
    }

    std::vector<Particle> particles;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        int n;
        ss >> n;
        particles.clear();
        for (int i = 0; i < n; i++) {
            Particle p;
            ss >> p.mass >> p.x >> p.y >> p.z
               >> p.vx >> p.vy >> p.vz
               >> p.fx >> p.fy >> p.fz;
            particles.push_back(p);
        }
        break; // only first line
    }
    return particles;
}

std::vector<Particle> generate_random_particles(int n) {
    std::vector<Particle> particles;
    for (int i = 0; i < n; i++) {
        Particle p;
        p.mass = 1e22 + (rand() % 1000) * 1e20;
        p.x = (rand() % 1000 - 500) * 1e9;
        p.y = (rand() % 1000 - 500) * 1e9;
        p.z = (rand() % 1000 - 500) * 1e9;
        p.vx = (rand() % 2000 - 1000);
        p.vy = (rand() % 2000 - 1000);
        p.vz = (rand() % 2000 - 1000);
        p.fx = p.fy = p.fz = 0.0;
        particles.push_back(p);
    }
    return particles;
}

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0]
                  << " <num_particles_or_file> <dt> <steps> <dump_freq> <output_file>\n";
        return 1;
    }

    std::string input = argv[1];
    double dt = atof(argv[2]);
    int steps = atoi(argv[3]);
    int dump_freq = atoi(argv[4]);
    std::string output_file = argv[5];

    std::vector<Particle> particles;

    bool is_number = true;
    for (char c : input) {
        if (!isdigit(c)) { is_number = false; break; }
    }

    if (is_number) {
        int n = std::stoi(input);
        particles = generate_random_particles(n);
    } else {
        particles = load_particles_from_file(input);
    }

    std::ofstream out(output_file);
    if (!out) {
        std::cerr << "Error: Could not open output file.\n";
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int step = 0; step < steps; step++) {
        reset_forces(particles);
        compute_forces(particles);
        update_motion(particles, dt);

        if (step % dump_freq == 0) {
            dump_state_tsv(particles, out);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    out.close();

    std::cout << "Simulation complete.\n";
    std::cout << "Time elapsed: " << elapsed.count() << " seconds\n";
    return 0;
}
