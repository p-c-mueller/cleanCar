/*
 * cleanCar.cpp
 *
 *  Created on: Mar 29, 2021
 *      Author: pemueller
 */

#include "cleanCar.h"

void clearCars()
{
	theCar.resize(0);
	theCleanedCar.resize(0,0);
	energy.resize(0);
	awayWithYou.resize(0);
}


bool isNumber( const string& token )
{
    return regex_match( token, regex( ( "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?" ) ) );
}

bool checkForCar(const string& type)
{
	fstream controlFile;
	const string filename = type + ".lobster";
	controlFile.open(filename);

	if( controlFile.fail() )
	{
		cout << "It seems like you don't have any " << type << " to clean. \nPlease come back once you remember where you parked it." << endl << endl;
		return 1;
	}

	return 0;
}

int isIn( const string& refString, const vector<string>& stringVector )
{
	for ( int i = 0; i < stringVector.size(); ++i )
	{
		if ( stringVector[i] == refString )
			return i;
	}

	return -1;
}

void printTheCar()
{
	 cout << " _____     _            _        _____" 		<< endl <<
	 "|  __ \\   | |          ( )      / ____|           " 	<< endl <<
	 "| |__) |__| |_ ___ _ __|/ ___  | |     __ _ _ __" 	<< endl <<
	 "|  ___/ _ \\ __/ _ \\ '__| / __| | |    / _` | '__| " << endl <<
	 "| |  |  __/ ||  __/ |    \\__ \\ | |___| (_| | |" 	<< endl <<
	 "|_|___\\___|\\__\\___|_|    |___/  \\_____\\__,_|_|" 	<< endl <<
	 "  _____ _                  _                  "	    << "           _________" << endl <<
	 " / ____| |                (_)                 "		<< "	        //  || \\  \\ " << endl <<
	 "| |    | | ___  __ _ _ __  _ _ __   __ _" 			<< "	 ______//___||__\\  \\___ " <<endl <<
	 "| |    | |/ _ \\/ _` | '_ \\| | '_ \\ / _` |" 		<< " 	 )  __ COBI & CO __    \\ " << endl <<
	 "| |____| |  __/ (_| | | | | | | | | (_| |"			<< " 	 |_/  \\_________/  \\___| " << endl <<
	 " \\_____|_|\\___|\\__,_|_| |_|_|_| |_|\\__, |" 		<< "	___\\__/_________\\__/______" << endl <<
	 "                                    __/ |" 			<< endl <<
	 "                                   |___/" 			<< endl;
}


string getAtomLabels( const string& carLabel )
{
	if ( carLabel == "Average" )
		return carLabel;

	string result, result_prev;
	char char_array[ carLabel.length() + 1 ];
	strcpy(char_array, carLabel.c_str());

#ifdef DEBUG
	cout << endl << endl << carLabel << " -> " << char_array << " -> ";
#endif

	for ( size_t i = 3; i < carLabel.length(); ++i )
	{
		if ( isalpha( char_array[i] ) )
		{
			if ( isupper(char_array[i]) )
				result_prev +="-";

			result_prev += char_array[i];
#ifdef DEBUG
			cout << char_array[i];
#endif
		}
	}

	result = result_prev.substr(1,result_prev.length());

#ifdef DEBUG
	cout << endl << endl;
#endif
	return result;
}

void writeSumCar( const string& type, const vector<MatrixXd>& theCar, const MatrixXd& theCleanedCar, const vector<string>& theLabels, const size_t& nSpins, const double& Emin, const double& Emax, const double& Ef)
{
#ifdef DEBUG
	cout << endl << "Summing now..." << flush;
#endif
	const size_t numCars = theLabels.size();
	VectorXi assignCar = VectorXi::Zero(numCars);	// Contains the final car that the current car will be assigned to.
	vector<string> assignLabels;

	// Categorize the cars according to their labels
	for ( size_t i = 0; i < numCars; ++i )
	{
#ifdef DEBUG
		cout << endl << theLabels[i] << flush;
		cout << endl << getAtomLabels(theLabels[i]) << flush;
#endif
		if ( isIn( getAtomLabels(theLabels[i]), assignLabels ) < 0 )
		{
			assignLabels.push_back( getAtomLabels(theLabels[i]) );
		}
		assignCar(i) = isIn( getAtomLabels(theLabels[i]), assignLabels );
	}

	// Write the new sum car
#ifdef DEBUG
	cout << endl << assignCar << endl << flush;
	for ( size_t i = 0; i < assignLabels.size(); ++i )
		cout << assignLabels[i] << endl << flush;
#endif

	MatrixXd theSumCar = MatrixXd::Zero( theCleanedCar.rows(), 2*assignLabels.size() + 3 );

	// Set energy scale
	theSumCar.block(0,0,theSumCar.rows(),1) = theCleanedCar.block(0,0,theCleanedCar.rows(),1);

#ifdef DEBUG
	cout << endl << "Entering the sum..." << flush;
#endif
#ifdef LOTS_O_DEBUG
	cout << endl << theSumCar << endl << flush;
#endif
	// Summarize over all interaction kinds
	for ( size_t i = 0; i < numCars; ++i  )
	{
		theSumCar.block(0,2*assignCar(i)+3,theSumCar.rows(),2) += theCar[i];
		theSumCar.block(0,1,theSumCar.rows(),2) += theCar[i];
#ifdef LOTS_O_DEBUG
		cout << endl << theSumCar << endl << flush;
#endif
	}

#ifdef DEBUG
	cout << endl << "Writing sum file..." << flush;
#endif
	ofstream outfile;
	outfile.open(type+".sum");
	const string headerString = "ESCALE ; " + type + " file generated by LOBSTER and cleaned by Peter's Car Cleaning. Energy is shifted such that the Fermi level lies at 0 eV.";
	outfile << headerString << endl << "  " << (theSumCar.cols() -1) / 2 << "  " << nSpins << "  "  << theSumCar.rows() << "  " << Emin << "  " << Emax << "  " << Ef << endl;
	outfile << "Everything" << endl;
	for ( size_t i = 0; i < assignLabels.size(); ++i )
		outfile << assignLabels[i] << endl;

	outfile << fixed << setprecision(5) << theSumCar.format(StreamPrecision);


	outfile.close();
}


void writeImpCar( const string& type, const MatrixXd& theCar, const size_t& nSpins, const double& Emin, const double& Emax, const double& Ef)
{
#ifdef DEBUG
	cout << "Hallo " << type << endl;
#endif
	MatrixXd theImpCar = MatrixXd::Zero(theCar.rows(),5);

	// set energy scale and find the Fermi level
	theImpCar.block(0,0,theImpCar.rows(),1) = theCar.block(0,0,theCar.rows(),1);

//	cout << theImpCar << endl;

	size_t fermiLevel;
	theImpCar.col(0).array().abs().minCoeff(&fermiLevel);

	VectorXd weights = VectorXd::Zero(theCar.cols()/2-1);

#ifdef DEBUG
	cout << "Hallo 1 " << type << endl;
#endif

	for ( int i = 0; i < weights.size(); ++i )
		weights(i) = theCar(fermiLevel, 2*i + 2);	// Write the ICOHP of each interaction to the weights vector

	weights /= weights.sum();

#ifdef DEBUG
	cout << "Hallo 1.5 " << type << endl;
#endif


	for ( int i = 0; i < weights.size(); ++i )
	{
		theImpCar.col(1) += theCar.col(2*i + 1) * weights(i);
		theImpCar.col(2) += theCar.col(2*i + 2) * weights(i);
		theImpCar.col(3) += theCar.col(2*i + 1) / weights.size() * 2;// todo: Zahl der Atome, nicht Zahl der Interaktionen!
		theImpCar.col(4) += theCar.col(2*i + 2) / weights.size() * 2;
	}

#ifdef DEBUG
	cout << "Hallo 2 " << type << endl;
#endif

	ofstream outfile;
	outfile.open(type+".imp");
	const string headerString = "ESCALE ; pCOHP file generated by LOBSTER and cleaned by Peter's Car Cleaning. Energy is shifted such that the Fermi level lies at 0 eV.";
	outfile << headerString << endl << "  " << (theImpCar.cols() -1) / 2 << "  " << nSpins << "  "  << theImpCar.rows() << "  " << Emin << "  " << Emax << "  " << Ef << endl;
	outfile << "Weightet average " << type << endl;
	outfile << "Per atom " << type << endl;
	outfile << fixed << setprecision(5) << theImpCar.format(StreamPrecision);


	outfile.close();
}

bool checkFor( char* inString, char** argv, int argc )
{
	for ( int i = 1; i < argc; ++i )
	{
		if ( strcmp(argv[i], inString) == 0 )
			return true;
	}
	return false;
}

bool checkFor ( char* argv, char inChar )	// This function goes through a char array and
{
	for ( size_t i = 0; i < sizeof(argv) / sizeof(char); ++i )
	{
		if ( argv[i] == inChar )
			return true;
	}
	return false;
}

bool checkCutoff( char** argv, int argc )
{
#ifdef DEBUG
	cout << "Checking cutoff..." << endl;
#endif

	for ( int i = 1; i < argc; ++i )
	{
		if ( (strcmp( argv[i], (char*)"-d" ) == 0) || (strcmp(argv[i], (char*)"--digits") == 0) )
		//if ( ( argv[i] == (char*)"-d" ) == 0 || (argv[i] == (char*)"--digits") == 0 )
		{
			string cutoffString = argv[i+1];
#ifdef DEBUG
			cout << "string " << i+1 << " " << cutoffString;
#endif
			cutoff = pow( 10, -1 * stod( cutoffString ) );
#ifdef DEBUG
			cout << " cutoff " << cutoff << endl;
#endif
			return true;
		}
	}


	for ( int i = 1; i < argc; ++i )
	{
		//if ( isNumber(argv[i]) )
		if ( (strcmp( argv[i], (char*)"-c" ) == 0) || (strcmp(argv[i], (char*)"--cutoff") == 0) )
		{
			cutoff = stod(argv[i+1]);
			if ( checkFor(argv[i+1], '.') && cutoff == 0 )
			{
				string result = argv[i+1];
				result += "1";
				//cout << argv[i] << " " << i << endl;
				//cout << result << endl;
				cutoff = stod( result );
			}
			return true;
		}
	}

	return false;
}

int cleanTheCar(const string& type)
{
	if ( checkForCar(type) )	// If there is no car to clean, exit.
		return 1;

	// Read the car
	cout << "Cleaning the " << type << endl;
	fstream inFile;
	const string filename = type + ".lobster";
	inFile.open(filename);

	string inString;
	string line;
	stringstream ss;

	getline(inFile, line);

	const string headerString = line;

	getline(inFile, line);
	ss << line;
	ss >> inString;
	const size_t nCOXPs = stoi(inString);
	ss >> inString;
	const size_t nSpins = stoi(inString);
	ss >> inString;
	const size_t nPoints = stoi(inString);
	ss >> inString;
	const double Emin = stod(inString);
	ss >> inString;
	const double Emax = stod(inString);
	ss >> inString;
	const double Ef = stod(inString);

#ifdef DEBUG
	cout << nCOXPs << " coxps " << nSpins << " spins " << nPoints << " points " << endl;
	cout << "Energy from " << Emin << " to " << Emax << " with Ef @ " << Ef << endl;
#endif

	theCar.clear();

	vector<string> theLabels;
	for ( size_t i = 0; i < nCOXPs; ++i )
	{
		getline(inFile, line);
		ss.clear();
		ss.str("");
		ss << line;
		theLabels.push_back(ss.str());

		theCar.push_back(MatrixXd::Zero( nPoints, 2 ));
	}
	getline(inFile, line);

#ifdef DEBUG
	cout << "label 1: " << theLabels[0] << endl;
	cout << "label 2: " << theLabels[1] << endl;
	cout << "label " << nCOXPs << ": " << theLabels[nCOXPs-1] << endl;
#endif


	energy.resize(nPoints);

	for ( size_t i = 0; i < nPoints; ++i )
	{
		getline( inFile, line );
		ss.clear();
		ss.str("");
		ss << line;
		ss >> inString;
		energy(i) = stod(inString);

		for ( size_t j = 0; j < nCOXPs; ++j )
		{
			ss >>inString;
			theCar[j](i,0)=stod(inString);
			ss >>inString;
			theCar[j](i,1)=stod(inString);
		}


	}

//	cout << "The car: " << theCar[0] << endl;

	// Clean the car
	awayWithYou.resize(nCOXPs);
	awayWithYou.setZero();
	theCleanedCar.resize(nPoints, 1);
	theCleanedCar.block(0,0, nPoints, 1) = energy;


	for ( size_t i = 0; i < nCOXPs; ++i )
	{
		if ( (theCar[i].maxCoeff() > cutoff) || ( abs(theCar[i].minCoeff()) > cutoff) )
		{
			awayWithYou(i) = 1;
			theCleanedCar.conservativeResize(nPoints,theCleanedCar.cols()+2);
			theCleanedCar.block(0, theCleanedCar.cols()-2, nPoints, 2) = theCar[i];
		}

	}




	// Write the car
	ofstream outfile;
	string outfilename = type + ".cleaned";
	outfile.open(outfilename);
	outfile << headerString << endl << "  " << awayWithYou.sum()  << "  " << nSpins << "  "  << nPoints << "  " << Emin << "  " << Emax << "  " << Ef << endl;

	for ( size_t i = 0; i < nCOXPs; ++i )
	{
		if ( awayWithYou(i) == 1 )
		{
			outfile << theLabels[i] << endl;
		}
	}


	outfile << fixed << setprecision(5) << theCleanedCar.format(StreamPrecision);

	outfile.close();
	// Make some jokes about cars :)




	if ( nCOXPs - awayWithYou.sum() == 0 )
		cout << "Luckily, no "<< type << "s were damaged during cleanup. The cleaning was not successful, though." << endl
			<< "If you still want to clean your " << type << "s, we will be happy to welcome you again soon!" << endl;
	else
	{
		cout << nCOXPs - awayWithYou.sum() << " out of " << nCOXPs << " " << type;
		if ( nCOXPs - awayWithYou.sum() == 1 )
			cout << "s was irreparably damaged. For that, we are deeply sorry." << endl;
		else
			cout << "s were irreparably damaged. For that, we are deeply sorry." << endl;

		cout << "On the other side, we managed to keep " << awayWithYou.sum() << " " << type << "s!" << endl;
	}

	if ( imp == true )
	{
		writeImpCar( type, theCleanedCar, nSpins, Emin, Emax, Ef);
		writeSumCar( type, theCar, theCleanedCar, theLabels, nSpins, Emin, Emax, Ef);
	}

	return 0;
}

int main ( int argc, char** argv )
{
	if ( checkFor( (char*)"-imp\n", argv, argc ) || checkFor((char*)"-imp", argv, argc) )
		imp = true;
	else
		imp = false;

	printTheCar();

	string cutoffString;
	cout << endl << "Thank you for choosing Peter's car cleaning service! We clean your car.*" << endl
			<< "*(Note that we provide no warranty whatsoever. Your car may be damaged during cleaning...)" << endl;

	if ( !checkCutoff( argv, argc ) )
	{

		cout << "How clean do you want your car to be? Please enter the number of decimal digits you want your car to have" << endl << ">> ";

		cin >> cutoffString;


		if ( isNumber( cutoffString ) )
		{
			cutoff = pow(10, -1 * stod( cutoffString ));
		}
		else
		{
			cout << "You chose a non-recognized value for cleaniness. Proceeding with default value." << endl;
			cutoff = 0;
		}
	}

	size_t cleanedCars = 0;

	clearCars();

	try
	{
		if ( cleanTheCar("COBICAR") == 0 )
		{
			cout << "Cleaned COBICAR." << endl;
			++cleanedCars;
		}
	}
	catch (...){
		cout << "Something went wrong with your COBICAR." << endl;
	}
	try
	{
		if ( cleanTheCar("COBICAR.nb") == 0 )
		{
			cout << "Cleaned nonbonding-COBICAR." << endl;
			++cleanedCars;
		}
	}
	catch (...){
		cout << "Something went wrong with your nonbonding-COBICAR." << endl;
	}
	try
	{
		if ( cleanTheCar("COHPCAR") ==0 )
		{
			cout << "Cleaned COHPCAR." << endl;
			++cleanedCars;
		}
	}
	catch (...){
		cout << "Something went wrong with your COHPCAR." << endl;
	}

	try
	{
		if (cleanTheCar("COOPCAR") == 0)
		{
			cout << "Cleaned COOPCAR." << endl;
			++cleanedCars;
		}
	}
	catch (...){
		cout << "Something went wrong with your COOPCAR." << endl;
	}
	try
	{
		if (cleanTheCar("COHPCAR.MO") == 0)
		{
			cout << "Cleaned molecular COHPCAR." << endl;
			++cleanedCars;
		}
	}
	catch (...){
		cout << "Something went wrong with your molecular COHPCAR." << endl;
	}
	try
	{
		if (cleanTheCar("COBICAR.MO") == 0)
		{
			cout << "Cleaned molecular COBICAR." << endl;
			++cleanedCars;
		}
	}
	catch (...){
		cout << "Something went wrong with your molecular COBICAR." << endl;
	}
	if ( cleanedCars == 0 )
		cout << "Sadly, we did not find any cars to clean. Maybe have a look at the next parking lot?" << endl;


	return 0;
}

