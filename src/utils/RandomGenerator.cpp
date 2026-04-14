#include "RandomGenerator.hpp"

namespace RandomGenerator {

   
    static std::mt19937& getGenerator() {

        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    double getRandomU() {
        std::mt19937& gen = getGenerator();
        

        double range = static_cast<double>(gen.max() - gen.min());
        double value = static_cast<double>(gen() - gen.min());
        double u = value / range;
        
        if (u == 0.0) {
            u = 0.000001; 
        }
        
        return u;
    }

    double generateUniformContinuous(double a, double b) {
        double u = getRandomU();
        return a + (b - a) * u;
    }

    double generateExponential(double lambda) {
        double u = getRandomU();
        return -std::log(u) / lambda;
    }

    double generateWeibull(double shapeK, double scaleLambda) {
        double u = getRandomU();
        return scaleLambda * std::pow(-std::log(u), 1.0 / shapeK);
    }

    int generateGeometric(double p) {
        double u = getRandomU();
        double num = std::log(u);
        double den = std::log(1.0 - p);
        return static_cast<int>(std::ceil(num / den));
    }

    int generateUniformDiscrete(int n) {
        double u = getRandomU();
        return static_cast<int>(std::floor(u * n)) + 1;
    }

    int generateBinomial(int n, double p) {
        int successes = 0;
        for (int i = 0; i < n; ++i) {
            if (getRandomU() < p) {
                successes++;
            }
        }
        return successes;
    }

    double generateNormal(double mu, double sigma) {
        double u1 = getRandomU();
        double u2 = getRandomU();

        if (u1 == 0.0) {
            u1 = 0.000001;
        }

        #ifndef M_PI
        const double M_PI = 3.14159265358979323846;
        #endif

        double z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
        return mu + sigma * z0;
    }

    int generatePoisson(double lambda) {
        double limit = std::exp(-lambda);
        double product = 1.0;
        int k = 0;

        do {
            k++;
            product *= getRandomU();
        } while (product > limit);

        return k - 1;
    }

}
