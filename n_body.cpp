#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>

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

            // normalized direction
            double fx = force * dx / dist;
            double fy = force * dy / dist;
            double fz = force * dz / dist;

            // apply equal and opposite forces
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
        // acceleration
        double ax = p.fx / p.mass;
        double ay = p.fy / p.mass;
        double az = p.fz / p.mass;

        // update velocity
        p.vx += ax * dt;
        p.vy += ay * dt;
        p.vz += az * dt;

        // update position
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

int main() {
    // Initial state from your example output (10 bodies)
    std::vector<Particle> particles = {
        {1.9891e30, 0, 0, 0, 0, 0, 0},                // Sun
        {5.972e24, 1.496e11, 0, 0, 0, 29780, 0},     // Earth
        {7.342e22, 1.49984e11, 0, 0, 0, 30802, 0}    // Moon
    };

    double dt = 60.0; // timestep = 1 minute
    int steps = 1000;  // simulate 1000 steps

    std::ofstream out("simulation.tsv");
    if (!out) {
        std::cerr << "Error: Could not open output file.\n";
        return 1;
    }

    for (int step = 0; step < steps; step++) {
        reset_forces(particles);
        compute_forces(particles);
        update_motion(particles, dt);
        dump_state_tsv(particles, out);
    }

    out.close();
    std::cout << "Simulation complete. Output written to simulation.tsv\n";
    return 0;
}
