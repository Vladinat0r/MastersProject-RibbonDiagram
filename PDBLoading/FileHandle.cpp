#include "FileHandle.h"
#include "DataTable.h"
#include "../ProteinRendering/Atoms.h"

#include <fstream>
#include <sstream>

using boost::trim;
using boost::find_first;
using boost::split;
using boost::is_any_of;
using std::string;
using std::ifstream;
using std::cout;
using std::endl;
using std::wcout;
using std::vector;
using std::map;

FileHandle::FileHandle()
{
	this->maxLines = 0;

	QueryPerformanceFrequency(&frequency1);

	this->data = new DataTables();
	containsHydrogen = false;

	myBackbone.init();
}

int FileHandle::getMaxLines()
{
	delete data;

	return maxLines;

}

void FileHandle::openFile(const string strl, std::string fileType)
{
	static double time = 0.0;

	QueryPerformanceCounter(&start1);

	if (fileType._Equal("PDB")) {
		processFile(strl);
	}
	else if (fileType._Equal("DSSP")) {
		processDSSPFile(strl);
	}

	QueryPerformanceCounter(&end1);
	time = ((double)end1.QuadPart - (double)start1.QuadPart)/((double)frequency1.QuadPart);
	//cout << (time / 0.001)*100.0 << endl;
	cout << "Time taken to load file: " << time << endl;



}
/*-------------------------------------------------------
processFile:
Currently this will only correctly handle PDB files
and txt files in the same format as a PDB file.

-------------------------------------------------------*/

void FileHandle::processFile(std::string str) {
	maxLines = 0;
	ifstream inFile;

	string line;
	inFile.open(str.c_str(), std::ios::in);

	if (inFile.is_open())
	{
		
		while (! inFile.eof() )
		{
			getline (inFile,line);
			
			string recordName = line.substr(0,6);
			trim(recordName);
			
			if(find_first(recordName, "ATOM") || find_first(recordName, "HETATM"))
			{
				this->add_ATOM_Information(line, true);

						
			}
		}
		inFile.close();
		Vector3d v = getMidPoint();

		//subtract v from all points
		for(int i = 0; i < maxLines; i++)
		{
			atomList[i]->subtractVectorFromLocalPosition(v);
		}
		myBackbone.subtractVectorFromLocalPosition(v);
		myBackbone.calcDistances();
		
	}
	else
	{
		cout << "Unable to open file" << endl; 
		
	}
}

void FileHandle::processDSSPFile(std::string filePath) {	//Reads in the secondary structure type for the residues
	ifstream inFile;
	int chainStartIndex = 0;	//Reduces number of iterations when searching for the same resCounter in add_DSSP_Information()

	string line, lineNext;
	inFile.open(filePath.c_str(), std::ios::in);
	//int resCounter = 1;

	if (inFile.is_open()) {
		while (getline(inFile, line) && line != "") {

			string recordLine = line.substr(5, 7);
			trim(recordLine);

			if (find_first(recordLine, "RESIDUE")) {
				break;
			}
		}

		getline(inFile, line);
		getline(inFile, lineNext);
		while (line != "") {
			int resCounter = stoi(line.substr(5, 5));
			//If chain end is found
			if (lineNext == "" || find_first(lineNext, "!")) {
				chainStartIndex = this->add_DSSP_Information(line, resCounter, chainStartIndex, true);
				//cout << "CHAIN END FOUND AT: " << chainStartIndex << endl;
				line = lineNext;
				getline(inFile, lineNext);
			}
			else {
				chainStartIndex = this->add_DSSP_Information(line, resCounter, chainStartIndex, false);
			}

			line = lineNext;
			getline(inFile, lineNext);
		}

	}
	inFile.close();
}

void FileHandle::testPrintTokenizedLines()
{

	//for(std::vector
	//std::vector<std::vector<int> > matrix_int;
	//BOOST_FOREACH( std::vector<Boost_char_tokenizer> & row, tokenizedLines )
	//	BOOST_FOREACH( int & i, row )
	//	cout << *row[i] << endl;
	int i = 0;
	for(std::vector<Boost_char_tokenizer>::iterator it; it != this->tokenizedLines.end(); it++)
	{
		cout << "loop: " << i << endl;
		i++;

	}


}

/*-------------------------------------------------------
splitString:
Returns a vector of string tokens split based on a ' '
character
---------------------------------------------------------*/
FileHandle::split_vector_type FileHandle::splitString(std::string str)
{  
    split_vector_type SplitVec; // #2: Search for tokens
	
	//boost::algorithm::token_compress_on;
    split( SplitVec, str, is_any_of(" ") ); // SplitVec == { "token1","token2","token3"..... }
	

	return SplitVec;
}

/*-------------------------------------------------------
splitVector:
Returns a vector of string iterators split based on a ' '
character
-------------------------------------------------------*/
FileHandle::find_vector_type FileHandle::splitVector(std::string str)
{
 
    find_vector_type FindVec; // #1: Search for separators
    ifind_all( FindVec, str, " " ); // FindVec == { [...],[...],[...]...... }

	return FindVec;

}


/*-------------------------------------------------------
tokenizeString:
This function is not currently required, when it is it
has the ability to tokenize a string based on multiple
conditions specified by:- sep
-------------------------------------------------------*/

void FileHandle::tokenizeString(std::string line)
{


	boost::tokenizer<> tok(line);


	boost::tokenizer<>::iterator beg=tok.begin(); 
	// here *beg will give us "ATOM" 
	 


	boost::char_separator<char> sep(" "); 
	Boost_char_tokenizer next(beg.base(),line.end(), sep); 

	//this->tokenizedLines.push_back(next);
	

	Boost_char_tokenizer::iterator tok_iter = next.begin(); 
	int i = 1;
	for(; tok_iter!=next.end(); ++tok_iter)
	{
		cout << "i: " << i << endl;
		string temp = *tok_iter;
		trim(temp);
		std::cout << temp << std::endl; 
		i++;
	} 



}

void FileHandle::add_ATOM_Information(std::string line, bool amIAnAtom)
{
	if(amIAnAtom)
	{
		Vector3d vec;
		string atomINFO;
		float radius;
		Vector3d colourVec;
		string residue;
		string chainID;
		string resName;

		
			string XString = line.substr(30,8);
			string YString = line.substr(38,8);
			string ZString = line.substr(46,8);

			trim(XString);
			trim(YString);
			trim(ZString);


			std::stringstream Xs;
			std::stringstream Ys;
			std::stringstream Zs;
			float Xfloat;
			float Yfloat;
			float Zfloat;
			// Cast to float...
			Xs << XString;
			Xs >> Xfloat;

			Ys << YString;
			Ys >> Yfloat;

			Zs << ZString;
			Zs >> Zfloat;

			vec.x = Xfloat*INITIAL_SCALE;
			vec.y = Yfloat*INITIAL_SCALE;
			vec.z = Zfloat*INITIAL_SCALE;

			allTheCoords.push_back(vec);

			residue = line.substr(23,3);
			chainID = line.substr(21,1);
			resName = line.substr(17,3);
		
			DataTables::colour_list colours = data->getColourList();

			DataTables::VDW_Radii_list listOfRadii = data->getVDWRadii();

			string element = " ";
			bool standardElement = false;
			try
			{
				element = line.substr(76,2);
				trim(element);
				
				int checkForElementCol = element.compare(" ");
				if(checkForElementCol != 0)
				{
					int result = element.compare("H"); 
					if(result == 0)
						containsHydrogen = true;
				}
				else
				{
					element = line.substr(13,2);
					trim(element);
					//cout << element << endl;

				}

				standardElement = true;

			}
			catch(const std::out_of_range& oor)
			{
#ifdef _DEBUG
				cout << oor.what() << endl;
#endif

				standardElement = false;

			}
			catch(const std::exception& e)
			{
#ifdef _DEBUG
				e.what();
#endif
				standardElement = false;

			}
			if(!standardElement)
			{
				try
				{
					element = line.substr(13,1);
					trim(element);
					
					int result = element.compare("H"); 
					if(result == 0)
						containsHydrogen = true;

					//cout << element << endl;

					

				}
				catch(const std::out_of_range& oor)
				{
#ifdef _DEBUG
					cout << "error in col 13" << oor.what() << endl;
#endif

				}
				catch(const std::exception& e)
				{
#ifdef _DEBUG
					cout << "error in col 13" << endl;

					e.what();
#endif

				}
			}



			//vector<float> current_colour = colours.find(element)->second;
			std::map <std::string, std::vector<float>>::iterator itr;
			vector<float> current_colour;
			itr = colours.find(element);
			if( itr == colours.end() )
			{

				current_colour.push_back(0.5);
				current_colour.push_back(0.5);
				current_colour.push_back(0.5);
			}
			else
			{
				current_colour = itr->second;
			} 
			
			colourVec.x = current_colour[0];
			colourVec.y = current_colour[1];
			colourVec.z = current_colour[2];
			//radius = listOfRadii.find(element)->second;
			std::map <std::string, float>::iterator itrRad = listOfRadii.find(element);

			if( itrRad == listOfRadii.end() )
			{
				radius = 10.0f;
			}
			else
			{
				radius = itrRad->second;
			} 

			

			allTheRads.push_back(radius);

			//here we add the full atom info including name and residue info
			atomINFO = line.substr(12,14);
			trim(atomINFO);
			string C_alpha = line.substr(13,2);

			int result = C_alpha.compare("CA"); 
			//cout << C_alpha << endl;
			if(result == 0)
			{
				//cout << "adding coords" << endl;
				myBackbone.addCoord(vec, chainID);
			}

		Atom* atom = new Atom(vec, colourVec, atomINFO, radius, maxLines/*, myMassList*/, element, residue, resName, chainID);
		
		atom->init();
		if(find_first(line, " HOH "))
		{
			//cout << "found water" << endl;
			atom->setImWater(true);
		}
		else
		{
			atom->setImWater(false);
		}
		atomList.push_back(atom);

		this->maxLines++;

	}
}

void FileHandle::add_HETATOM_Information(std::string line, bool amIAnAtom)
{
}

int FileHandle::add_DSSP_Information(std::string line, int resCounter, int index, bool isChainEnd) {
	string residue = line.substr(16, 2);
	trim(residue);

	int atomListSize = atomList.size();

	for (; index < atomListSize; index++) {
		int currRes = stoi(atomList[index]->getResidue());

		if (currRes == resCounter) {
			atomList[index]->setSecondaryElementID(residue, isChainEnd);
			//cout << atomList[i]->getSecondaryElementID() << endl;
		}
		else {
			return index;
		}

	}
	
}

Vector3d FileHandle::getMidPoint()
{
	float middleX = 0.0;
	float middleY = 0.0;
	float middleZ = 0.0;

	for(int i = 0; i < maxLines; i++)
	{
		Vector3d temp = allTheCoords[i];
		middleX += temp.x;
		middleY += temp.y;
		middleZ += temp.z;
	}

	middleX /= (float)maxLines;
	middleY /= (float)maxLines;
	middleZ /= (float)maxLines;


	Vector3d vec(middleX, middleY, middleZ);

	return vec;
}

float FileHandle::getFurthestDistanceToMidPoint()
{
	Vector3d midpoint = getMidPoint();

	float distance=0;

	for(int i = 0; i < maxLines; i++)
	{
		Vector3d temp = allTheCoords[i];
		float dist= midpoint.euclideanDistance(temp);
		if(dist > distance)
			distance = dist;
	}

	return distance;
}