#pragma once

#include <cmath>
#include <random>

namespace RandomGenerator {

    // genere U dans (0.0, 1.0] avec mt19937 et evite 0.0 pour les log
    double getRandomU();

    // loi uniforme continue (inverse transform) : a + (b - a) * U
    double generateUniformContinuous(double a, double b);

    // loi expo (inverse transform) : -ln(U) / lambda
    double generateExponential(double lambda);

    // loi de weibull (inverse transform) : scaleLambda * (-ln(U))^(1/shapeK)
    double generateWeibull(double shapeK, double scaleLambda);

    // loi geometrique (inverse transform) : ceil( ln(U) / ln(1 - p) )
    int generateGeometric(double p);

    // loi uniforme discrete (inverse transform) : floor(U * n) + 1
    int generateUniformDiscrete(int n);

    // loi binomiale (somme de n epreuves de bernoulli)
    int generateBinomial(int n, double p);

    // loi normale ou gauss (methode de box-muller)
    double generateNormal(double mu, double sigma);

    // loi de poisson (algorithme de knuth)
    int generatePoisson(double lambda);

} // namespace RandomGenerator