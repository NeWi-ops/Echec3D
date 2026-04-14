#include "statisticsTest.hpp"
#include "RandomGenerator.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>

void runStatisticalTests() {
    const int N = 100000;
    

    std::cout << "         STATISTICAL TESTS (N = " << N << ")         \n";
  


    double a = 5.0;
    double b = 10.0;
    double theorUniform = (a + b) / 2.0;
    double sumUniform = 0.0;
    for (int i = 0; i < N; ++i) {
        sumUniform += RandomGenerator::generateUniformContinuous(a, b);
    }
    double empUniform = sumUniform / N;
    
    std::cout << "--- Continuous Uniform [" << a << ", " << b << "] ---\n";
    std::cout << "Theoretical Mean: " << std::fixed << std::setprecision(5) << theorUniform << "\n";
    std::cout << "Empirical Mean:   " << empUniform << "\n";
    std::cout << "Absolute Error:   " << std::abs(theorUniform - empUniform) << "\n\n";

    // 2. Exponential Distribution (lambda)
    double expLambda = 2.0;
    double theorExp = 1.0 / expLambda;
    double sumExp = 0.0;
    for (int i = 0; i < N; ++i) {
        sumExp += RandomGenerator::generateExponential(expLambda);
    }
    double empExp = sumExp / N;
    
    std::cout << "--- Exponential [lambda=" << expLambda << "] ---\n";
    std::cout << "Theoretical Mean: " << theorExp << "\n";
    std::cout << "Empirical Mean:   " << empExp << "\n";
    std::cout << "Absolute Error:   " << std::abs(theorExp - empExp) << "\n\n";

    // 3. Weibull Distribution (shapeK, scaleLambda)
    double shapeK = 1.5;
    double scaleLambda = 1.0;
    double theorWeibull = scaleLambda * std::tgamma(1.0 + 1.0 / shapeK);
    double sumWeibull = 0.0;
    for (int i = 0; i < N; ++i) {
        sumWeibull += RandomGenerator::generateWeibull(shapeK, scaleLambda);
    }
    double empWeibull = sumWeibull / N;
    
    std::cout << "--- Weibull [k=" << shapeK << ", lambda=" << scaleLambda << "] ---\n";
    std::cout << "Theoretical Mean: " << theorWeibull << "\n";
    std::cout << "Empirical Mean:   " << empWeibull << "\n";
    std::cout << "Absolute Error:   " << std::abs(theorWeibull - empWeibull) << "\n\n";

    // 4. Geometric Distribution (p)
    double p = 0.3;

    double theorGeom = 1.0 / p;
    double sumGeom = 0.0;
    for (int i = 0; i < N; ++i) {
        sumGeom += RandomGenerator::generateGeometric(p);
    }
    double empGeom = sumGeom / N;
    
    std::cout << "--- Geometric [p=" << p << "] ---\n";
    std::cout << "Theoretical Mean: " << theorGeom << "\n";
    std::cout << "Empirical Mean:   " << empGeom << "\n";
    std::cout << "Absolute Error:   " << std::abs(theorGeom - empGeom) << "\n\n";

    // 5. Discrete Uniform Distribution (n)
    int n = 6;


    double theorDiscrete = (1.0 + n) / 2.0;
    double sumDiscrete = 0.0;
    for (int i = 0; i < N; ++i) {
        sumDiscrete += RandomGenerator::generateUniformDiscrete(n);
    }
    double empDiscrete = sumDiscrete / N;
    
    std::cout << "--- Discrete Uniform [1 to " << n << "] ---\n";
    std::cout << "Theoretical Mean: " << theorDiscrete << "\n";
    std::cout << "Empirical Mean:   " << empDiscrete << "\n";
    std::cout << "Absolute Error:   " << std::abs(theorDiscrete - empDiscrete) << "\n\n";
    // 6. Binomial Distribution (n, p)
    int binomN = 10;
    double binomP = 0.25;
    double theorBinom = binomN * binomP;
    double sumBinom = 0.0;
    for (int i = 0; i < N; ++i) {
        sumBinom += RandomGenerator::generateBinomial(binomN, binomP);
    }
    double empBinom = sumBinom / N;
    
    std::cout << "--- Binomial [n=" << binomN << ", p=" << binomP << "] ---\n";
    std::cout << "Theoretical Mean: " << theorBinom << "\n";
    std::cout << "Empirical Mean:   " << empBinom << "\n";
    std::cout << "Absolute Error:   " << std::abs(theorBinom - empBinom) << "\n\n";

    // 7. Normal Distribution (mu, sigma)
    double normMu = 2.0;
    double normSigma = 0.5;
    double theorNorm = normMu;
    double sumNorm = 0.0;
    for (int i = 0; i < N; ++i) {
        sumNorm += RandomGenerator::generateNormal(normMu, normSigma);
    }
    double empNorm = sumNorm / N;
    
    std::cout << "--- Normal [mu=" << normMu << ", sigma=" << normSigma << "] ---\n";
    std::cout << "Theoretical Mean: " << theorNorm << "\n";
    std::cout << "Empirical Mean:   " << empNorm << "\n";
    std::cout << "Absolute Error:   " << std::abs(theorNorm - empNorm) << "\n\n";

    // 8. Poisson Distribution (lambda)
    double poisLambda = 4.0;
    double theorPois = poisLambda;
    double sumPois = 0.0;
    for (int i = 0; i < N; ++i) {
        sumPois += RandomGenerator::generatePoisson(poisLambda);
    }
    double empPois = sumPois / N;
    
    std::cout << "--- Poisson [lambda=" << poisLambda << "] ---\n";
    std::cout << "Theoretical Mean: " << theorPois << "\n";
    std::cout << "Empirical Mean:   " << empPois << "\n";
    std::cout << "Absolute Error:   " << std::abs(theorPois - empPois) << "\n\n";
    

}
