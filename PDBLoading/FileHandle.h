#ifndef FILEHANDLE_H__
#define FILEHANDLE_H__

#include "..\ProteinRendering\Backbone.h"

/**********************************************************************
Boost header files required for:
:- Regular Expression parsing
:- A very useful string implimentation
:- A handy string tokenizer

***********************************************************************/
#include <boost/lambda/lambda.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/config.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/const_iterator.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/detail/predicate.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
/*******************************/

#include <windows.h>

#define foreach BOOST_FOREACH

class DataTables;
class Atom;
//class Grid;
//class Mass;
//class SpringDirect;
class Backbone;


class FileHandle
{

private:
	typedef std::vector< boost::iterator_range<std::string::iterator> > find_vector_type;
	typedef std::vector< std::string > split_vector_type;
	typedef boost::tokenizer<boost::char_separator<char> > Boost_char_tokenizer;
	typedef std::vector<Atom*> AtomList;


public:

	FileHandle();
	void openFile(const std::string strl, std::string fileType);

	int getMaxLines();
	Vector3d getMidPoint();
	float getFurthestDistanceToMidPoint();

	AtomList getAtomList(){return atomList;} 
	
	int getSize();
	bool getHydrogenResult(){return containsHydrogen;}
	Backbone getBackbone(){return myBackbone;}

private:
	
	void tokenizeString(std::string line);
	void processFile(std::string str);
	void processDSSPFile(std::string filePath);
	split_vector_type splitString(std::string str);
	find_vector_type FileHandle::splitVector(std::string str);
	void testPrintTokenizedLines();
	void add_ATOM_Information(std::string line, bool amIAnAtom);
	void add_HETATOM_Information(std::string line, bool amIAnAtom);
	int add_DSSP_Information(std::string line, int counter, int index, bool isLast);

private:
	//Some variables to store the lines
	std::vector<Boost_char_tokenizer> tokenizedLines;
	std::vector<std::string> linesArray;

	std::vector<Vector3d> allTheCoords;
	std::vector<float> allTheRads;

	AtomList atomList;
	
	LARGE_INTEGER frequency1, start1, end1;
	DataTables *data;

private:
	//A variable that stores the amount of ATOM and HETATM lines read
	int maxLines;
	Backbone myBackbone;
	bool containsHydrogen;

};


#endif