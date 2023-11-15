/*
 * cleanCar.h
 *
 *  Created on: Mar 29, 2021
 *      Author: pemueller
 */

#ifndef SRC_CLEANCAR_H_
#define SRC_CLEANCAR_H_

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <Eigen/Core>
#include <sstream>
#include <fstream>
#include <ostream>
#include <regex>
#include <cstring>

using namespace std;
using namespace Eigen;

/*void readCar(string filename);
int cleanTheCar(string type);
void printTheCar();*/
//void writeImpCar( const string& type, const MatrixXd& theCar, const size_t& nSpins, const double& Emin, const double& Emax, const double& Ef, const vector<string>& theLabels);
vector<MatrixXd> theCar;
MatrixXd theCleanedCar;
VectorXd energy;
VectorXi awayWithYou;
double cutoff = 0;
bool imp = false;

#endif /* SRC_CLEANCAR_H_ */
